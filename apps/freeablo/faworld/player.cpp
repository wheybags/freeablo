#include "player.h"
#include "../engine/enginemain.h"
#include "../engine/threadmanager.h"
#include "../fagui/dialogmanager.h"
#include "../fagui/guimanager.h"
#include "../fasavegame/gameloader.h"
#include "actorstats.h"
#include "diabloexe/characterstats.h"
#include "equiptarget.h"
#include "itemenums.h"
#include "itemmap.h"
#include "missile/missile.h"
#include "playerbehaviour.h"
#include "spells.h"
#include "world.h"
#include <fmt/format.h>
#include <misc/assert.h>
#include <misc/stringops.h>
#include <random/random.h>
#include <string>

namespace FAWorld
{
    const std::string Player::typeId = "player";

    Player::Player(World& world, PlayerClass playerClass, const DiabloExe::CharacterStats& charStats) : Actor(world), mPlayerClass(playerClass)
    {
        mStats.initialise(initialiseActorStats(charStats));
        mStats.mLevelXpCounts = charStats.mNextLevelExp;

        mFaction = Faction::heaven();
        mMoveHandler.mPathRateLimit = World::getTicksInPeriod("0.1"); // allow players to repath much more often than other actors
        mBehaviour.reset(new PlayerBehaviour(this));

        initCommon();
    }

    void Player::initCommon()
    {
        mMoveHandler.mSpeedTilesPerSecond = FixedPoint(1) / FixedPoint("0.4"); // https://wheybags.gitlab.io/jarulfs-guide/#player-timing-information
        mName = "Player";
        mWorld.registerPlayer(this);
        mInventory.mInventoryChanged = [this](EquipTargetType inventoryType, Item const& removed, Item const& added) {
            (void)removed;

            mInventoryChangedCallCount++;

            updateSprites();

            switch (inventoryType)
            {
                case EquipTargetType::body:
                case EquipTargetType::leftHand:
                case EquipTargetType::rightHand:
                    // Update player graphics.
                    updateSprites();
                    break;
                default:
                    break;
            }

            if (!added.isEmpty() && mPlayerInitialised && this == mWorld.getCurrentPlayer())
            {
                // Play inventory place/grab sound.
                switch (inventoryType)
                {
                    case EquipTargetType::cursor:
                        Engine::ThreadManager::get()->playSound("sfx/items/invgrab.wav");
                        break;
                    default:
                        std::string soundPath = added.getInvPlaceSoundPath();
                        Engine::ThreadManager::get()->playSound(soundPath);
                        break;
                }
            }
        };
    }

    void Player::calculateStats(LiveActorStats& stats, const ActorStats& actorStats) const
    {
        CalculateStatsCacheKey statsCacheKey;
        statsCacheKey.baseStats = actorStats.baseStats;
        statsCacheKey.gameLevel = getLevel();
        statsCacheKey.level = actorStats.mLevel;
        statsCacheKey.inventoryChangedCallCount = mInventoryChangedCallCount;

        if (statsCacheKey == mLastStatsKey)
            return;
        mLastStatsKey = statsCacheKey;

        stats = LiveActorStats(); // clear before we start

        BaseStats charStats = actorStats.baseStats;

        ItemStats itemStats;
        mInventory.calculateItemBonuses(itemStats);

        stats.baseStats.strength = charStats.strength + itemStats.baseStats.strength;
        stats.baseStats.magic = charStats.magic + itemStats.baseStats.magic;
        stats.baseStats.dexterity = charStats.dexterity + itemStats.baseStats.dexterity;
        stats.baseStats.vitality = charStats.vitality + itemStats.baseStats.vitality;

        stats.toHitMelee.bonus = 0;
        stats.toHitRanged.bonus = 0;
        stats.toHitMagic.bonus = 0;

        EquippedInHandsItems handItems = mInventory.getItemsInHands();

        // TODO: make sure all the following calculations should be floored.
        // Flooring for melee damage produces the same numbers as displayed in the character GUI in the original game.
        // I'm not sure if a higher precision is used in the actual game update code.

        switch (mPlayerClass)
        {
            case PlayerClass::warrior:
            {
                stats.maxLife = (int32_t)(FixedPoint(2) * FixedPoint(charStats.vitality) + FixedPoint(2) * FixedPoint(itemStats.baseStats.vitality) +
                                          FixedPoint(2) * FixedPoint(actorStats.mLevel) + FixedPoint(itemStats.maxLife) + 18)
                                    .floor();

                stats.maxMana = (int32_t)(FixedPoint(1) * FixedPoint(charStats.magic) + FixedPoint(1) * FixedPoint(itemStats.baseStats.magic) +
                                          FixedPoint(1) * FixedPoint(actorStats.mLevel) + FixedPoint(itemStats.maxMana) - 1)
                                    .floor();

                stats.meleeDamage = (int32_t)((FixedPoint(charStats.strength) * actorStats.mLevel) / FixedPoint(100)).floor();
                stats.rangedDamage = (int32_t)((FixedPoint(charStats.strength) * actorStats.mLevel) / FixedPoint(200)).floor();

                stats.toHitMelee.bonus = 20;
                stats.toHitRanged.bonus = 10;
                stats.blockChance =
                    mInventory.isShieldEquipped() ? stats.baseStats.dexterity + 30 : 0; // TODOHELLFIRE: monks can block with staffs and hand to hand

                // https://wheybags.gitlab.io/jarulfs-guide/#weapon-speed
                if (handItems.meleeWeapon)
                {
                    switch (handItems.meleeWeapon->item->getType())
                    {
                        case ItemType::sword:
                        case ItemType::mace:
                            stats.meleeAttackSpeedInTicks = World::getTicksInPeriod(FixedPoint("0.45"));
                            break;
                        case ItemType::axe:
                            stats.meleeAttackSpeedInTicks = World::getTicksInPeriod(FixedPoint("0.5"));
                            break;
                        case ItemType::staff:
                            stats.meleeAttackSpeedInTicks = World::getTicksInPeriod(FixedPoint("0.55"));
                            break;
                        default:
                            invalid_enum(ItemType, handItems.meleeWeapon->item->getType());
                    }
                }
                else
                {
                    stats.meleeAttackSpeedInTicks = World::getTicksInPeriod(FixedPoint("0.45"));
                }

                if (handItems.rangedWeapon)
                    stats.rangedAttackSpeedInTicks = World::getTicksInPeriod(FixedPoint("0.55"));

                stats.spellAttackSpeedInTicks = World::getTicksInPeriod(FixedPoint("0.7"));

                break;
            }
            case PlayerClass::rogue:
            {
                stats.maxLife = (int32_t)(FixedPoint(1) * FixedPoint(charStats.vitality) + FixedPoint("1.5") * FixedPoint(itemStats.baseStats.vitality) +
                                          FixedPoint(2) * FixedPoint(actorStats.mLevel) + FixedPoint(itemStats.maxLife) + 23)
                                    .floor();

                stats.maxMana = (int32_t)(FixedPoint(1) * FixedPoint(charStats.magic) + FixedPoint("1.5") * FixedPoint(itemStats.baseStats.magic) +
                                          FixedPoint(2) * FixedPoint(actorStats.mLevel) + FixedPoint(itemStats.maxMana) + 5)
                                    .floor();

                stats.meleeDamage =
                    (int32_t)(((FixedPoint(charStats.strength) + FixedPoint(charStats.dexterity)) * actorStats.mLevel) / FixedPoint(100)).floor();
                stats.rangedDamage =
                    (int32_t)(((FixedPoint(charStats.strength) + FixedPoint(charStats.dexterity)) * actorStats.mLevel) / FixedPoint(100)).floor();

                stats.toHitRanged.bonus = 20;
                stats.blockChance = mInventory.isShieldEquipped() ? stats.baseStats.dexterity + 20 : 0;

                // https://wheybags.gitlab.io/jarulfs-guide/#weapon-speed
                if (handItems.meleeWeapon)
                {
                    switch (handItems.meleeWeapon->item->getType())
                    {
                        case ItemType::sword:
                        case ItemType::mace:
                            stats.meleeAttackSpeedInTicks = World::getTicksInPeriod(FixedPoint("0.5"));
                            break;
                        case ItemType::axe:
                            stats.meleeAttackSpeedInTicks = World::getTicksInPeriod(FixedPoint("0.65"));
                            break;
                        case ItemType::staff:
                            stats.meleeAttackSpeedInTicks = World::getTicksInPeriod(FixedPoint("0.55"));
                            break;
                        default:
                            invalid_enum(ItemType, handItems.meleeWeapon->item->getType());
                    }
                }
                else
                {
                    stats.meleeAttackSpeedInTicks = World::getTicksInPeriod(FixedPoint("0.5"));
                }

                if (handItems.rangedWeapon)
                    stats.rangedAttackSpeedInTicks = World::getTicksInPeriod(FixedPoint("0.55"));

                stats.spellAttackSpeedInTicks = World::getTicksInPeriod(FixedPoint("0.6"));

                break;
            }
            case PlayerClass::sorceror:
            {
                stats.maxLife = (int32_t)(FixedPoint(1) * FixedPoint(charStats.vitality) + FixedPoint(1) * FixedPoint(itemStats.baseStats.vitality) +
                                          FixedPoint(1) * FixedPoint(actorStats.mLevel) + FixedPoint(itemStats.maxLife) + 9)
                                    .floor();

                stats.maxMana = (int32_t)(FixedPoint(2) * FixedPoint(charStats.magic) + FixedPoint(2) * FixedPoint(itemStats.baseStats.magic) +
                                          FixedPoint(2) * FixedPoint(actorStats.mLevel) + FixedPoint(itemStats.maxMana) - 2)
                                    .floor();

                stats.meleeDamage = (int32_t)((FixedPoint(charStats.strength) * actorStats.mLevel) / FixedPoint(100)).floor();
                stats.rangedDamage = (int32_t)((FixedPoint(charStats.strength) * actorStats.mLevel) / FixedPoint(200)).floor();

                stats.toHitMagic.bonus = 20;
                stats.blockChance = mInventory.isShieldEquipped() ? stats.baseStats.dexterity + 10 : 0;

                // https://wheybags.gitlab.io/jarulfs-guide/#weapon-speed
                if (handItems.meleeWeapon)
                {
                    switch (handItems.meleeWeapon->item->getType())
                    {
                        case ItemType::sword:
                        case ItemType::mace:
                            stats.meleeAttackSpeedInTicks = World::getTicksInPeriod(FixedPoint("0.6"));
                            break;
                        case ItemType::axe:
                            stats.meleeAttackSpeedInTicks = World::getTicksInPeriod(FixedPoint("0.8"));
                            break;
                        case ItemType::staff:
                            stats.meleeAttackSpeedInTicks = World::getTicksInPeriod(FixedPoint("0.6"));
                            break;
                        default:
                            invalid_enum(ItemType, handItems.meleeWeapon->item->getType());
                    }
                }
                else if (handItems.shield)
                {
                    stats.meleeAttackSpeedInTicks = World::getTicksInPeriod(FixedPoint("0.45"));
                }
                else
                {
                    stats.meleeAttackSpeedInTicks = World::getTicksInPeriod(FixedPoint("0.6"));
                }

                if (handItems.rangedWeapon)
                    stats.rangedAttackSpeedInTicks = World::getTicksInPeriod(FixedPoint("0.8"));

                stats.spellAttackSpeedInTicks = World::getTicksInPeriod(FixedPoint("0.4"));

                break;
            }
            case PlayerClass::none:
                invalid_enum(PlayerClass, mPlayerClass);
        }

        // TODOHELLFIRE: Add in bonuses for barbarians and monks here, see https://wheybags.gitlab.io/jarulfs-guide/#monster-versus-player
        stats.armorClass = (int32_t)(FixedPoint(stats.baseStats.dexterity) / FixedPoint(5) + itemStats.armorClass).floor();
        stats.toHitMelee.base = (int32_t)(FixedPoint(50) + FixedPoint(stats.baseStats.dexterity) / FixedPoint(2) + itemStats.toHit).floor();
        stats.toHitMeleeMinMaxCap = {5, 95};
        stats.toHitRanged.base = (int32_t)(FixedPoint(50) + FixedPoint(stats.baseStats.dexterity) + itemStats.toHit).floor();
        stats.toHitMagic.base = (int32_t)(FixedPoint(50) + FixedPoint(stats.baseStats.magic)).floor();

        stats.meleeDamageBonusRange = itemStats.meleeDamageBonusRange;

        // https://wheybags.gitlab.io/jarulfs-guide/#damage-done
        if (!Item::isItemAWeapon(mInventory.getLeftHand().getType()) && !Item::isItemAWeapon(mInventory.getRightHand().getType()))
        {
            // TODOHELLFIRE: monks get a bonus here
            if (mInventory.getLeftHand().getType() == ItemType::shield || mInventory.getRightHand().getType() == ItemType::shield)
                stats.meleeDamageBonusRange = IntRange(1, 3);
            else
                stats.meleeDamageBonusRange = IntRange(1, 1);
        }

        stats.rangedDamageBonusRange = itemStats.rangedDamageBonusRange;
        stats.hitRecoveryDamageThreshold = actorStats.mLevel;
    }

    DamageType Player::getMeleeDamageType() const
    {
        const Item& left = mInventory.getLeftHand();
        const Item& right = mInventory.getRightHand();

        if (left.getType() == ItemType::mace || right.getType() == ItemType::mace)
            return DamageType::Club;
        if (left.getType() == ItemType::sword || right.getType() == ItemType::sword)
            return DamageType::Sword;
        if (left.getType() == ItemType::axe || right.getType() == ItemType::axe)
            return DamageType::Axe;
        if (left.getType() == ItemType::bow || right.getType() == ItemType::bow)
            return DamageType::Bow;
        if (left.getType() == ItemType::staff || right.getType() == ItemType::staff)
            return DamageType::Staff;

        return DamageType::Unarmed;
    }

    Player::Player(World& world, FASaveGame::GameLoader& loader) : Actor(world, loader)
    {
        mPlayerClass = static_cast<PlayerClass>(loader.load<int32_t>());
        initCommon();
        mPlayerInitialised = true;
    }

    void Player::save(FASaveGame::GameSaver& saver) const
    {
        release_assert(mPlayerInitialised);

        Serial::ScopedCategorySaver cat("Player", saver);

        Actor::save(saver);
        saver.save(static_cast<int32_t>(mPlayerClass));
    }

    Player::~Player() { mWorld.deregisterPlayer(this); }

    char getClassCode(PlayerClass playerClass)
    {
        switch (playerClass)
        {
            case PlayerClass::warrior:
                return 'w';
            case PlayerClass::rogue:
                return 'r';
            case PlayerClass::sorceror:
                return 's';
            case PlayerClass::none:
                break;
        }

        invalid_enum(PlayerClass, playerClass);
    }

    void Player::updateSprites()
    {
        auto classCode = getClassCode(mPlayerClass);

        std::string armour = "l", weapon;
        if (!mInventory.getBody().isEmpty())
        {
            switch (mInventory.getBody().getType())
            {
                case ItemType::heavyArmor:
                {
                    armour = "h";
                    break;
                }

                case ItemType::mediumArmor:
                {
                    armour = "m";
                    break;
                }

                case ItemType::lightArmor:
                default:
                {
                    armour = "l";
                    break;
                }
            }
        }
        if (mInventory.getLeftHand().isEmpty() && mInventory.getRightHand().isEmpty())
        {
            weapon = "n";
        }
        else if ((mInventory.getLeftHand().isEmpty() && !mInventory.getRightHand().isEmpty()) ||
                 (!mInventory.getLeftHand().isEmpty() && mInventory.getRightHand().isEmpty()))
        {
            const Item* hand = nullptr;

            if (!mInventory.getLeftHand().isEmpty())
                hand = &mInventory.getLeftHand();
            else
                hand = &mInventory.getRightHand();
            switch (hand->getType())
            {
                case ItemType::axe:
                {
                    if (hand->getEquipLoc() == ItemEquipType::oneHanded)
                        weapon = "s";
                    else
                        weapon = "a";
                    break;
                }

                case ItemType::mace:
                {
                    weapon = "m";
                    break;
                }

                case ItemType::bow:
                {
                    weapon = "b";
                    break;
                }

                case ItemType::shield:
                {
                    weapon = "u";
                    break;
                }

                case ItemType::sword:
                {
                    weapon = "s";
                    break;
                }

                default:
                {
                    weapon = "n";
                    break;
                }
            }
        }

        else if (!mInventory.getLeftHand().isEmpty() && !mInventory.getRightHand().isEmpty())
        {
            if ((mInventory.getLeftHand().getType() == ItemType::sword && mInventory.getRightHand().getType() == ItemType::shield) ||
                (mInventory.getLeftHand().getType() == ItemType::shield && mInventory.getRightHand().getType() == ItemType::sword))
                weapon = "d";

            else if (mInventory.getLeftHand().getType() == ItemType::bow && mInventory.getRightHand().getType() == ItemType::bow)
                weapon = "b";
            else if (mInventory.getLeftHand().getType() == ItemType::axe && mInventory.getRightHand().getType() == ItemType::axe)
                weapon = "a";

            else if (mInventory.getLeftHand().getType() == ItemType::staff && mInventory.getRightHand().getType() == ItemType::staff)
                weapon = "t";
            else if (mInventory.getLeftHand().getType() == ItemType::mace || mInventory.getRightHand().getType() == ItemType::mace)
                weapon = "h";

            release_assert(!weapon.empty()); // Empty weapon format
        }
        auto weaponCode = weapon;
        auto armourCode = armour;

        auto helper = [&](bool isDie, const char* animCode) {
            std::string weapFormat = weaponCode;

            if (isDie)
                weapFormat = "n";

            return fmt::format(FMT_STRING("plrgfx/{}/{}{}{}/{}{}{}{}.cl2"),
                               playerClassToString(mPlayerClass),
                               classCode,
                               armourCode,
                               weapFormat,
                               classCode,
                               armourCode,
                               weapFormat,
                               animCode);
        };

        auto renderer = FARender::Renderer::get();
        if (!renderer) // TODO: some sort of headless mode for tests
            return;

        mAnimation.setAnimationSprites(AnimState::dead, renderer->loadImage(helper(true, "dt")));
        mAnimation.setAnimationSprites(AnimState::attack, renderer->loadImage(helper(false, "at")));
        mAnimation.setAnimationSprites(AnimState::hit, renderer->loadImage(helper(false, "ht")));
        mAnimation.setAnimationSprites(AnimState::spellLightning, renderer->loadImage(helper(false, "lm")));
        mAnimation.setAnimationSprites(AnimState::spellFire, renderer->loadImage(helper(false, "fm")));
        mAnimation.setAnimationSprites(AnimState::spellOther, renderer->loadImage(helper(false, "qm")));

        if (mInventory.isShieldEquipped())
            mAnimation.setAnimationSprites(AnimState::block, renderer->loadImage(helper(false, "bl")));
        else
            mAnimation.setAnimationSprites(AnimState::block, renderer->loadImage(helper(false, "ht")));

        if (getLevel() && getLevel()->isTown())
        {
            mAnimation.setAnimationSprites(AnimState::walk, renderer->loadImage(helper(false, "wl")));
            mAnimation.setAnimationSprites(AnimState::idle, renderer->loadImage(helper(false, "st")));
        }
        else
        {
            mAnimation.setAnimationSprites(AnimState::walk, renderer->loadImage(helper(false, "aw")));
            mAnimation.setAnimationSprites(AnimState::idle, renderer->loadImage(helper(false, "as")));
        }

        // TODO: Is this actually correct? It seems kind of odd, but it is what is listed in Jarulf's guide
        // https://wheybags.gitlab.io/jarulfs-guide/#player-timing-information
        mMeleeHitFrame = mAnimation.getAnimationSprites(AnimState::attack)->getAnimLength() - 1;
    }

    bool Player::dropItem(const Misc::Point& clickedPoint)
    {
        auto cursorItem = mInventory.getCursorHeld();
        auto initialDir = (Vec2Fix(clickedPoint.x, clickedPoint.y) - Vec2Fix(getPos().current().x, getPos().current().y)).getDirection();
        auto curPos = getPos().current();
        auto direction = (curPos == clickedPoint) ? Misc::Direction(Misc::Direction8::none) : initialDir;

        if (getLevel()->dropItemClosestEmptyTile(cursorItem, *this, curPos, direction))
        {
            mInventory.setCursorHeld({});
            return true;
        }
        return false;
    }

    bool Player::canTalkTo(Actor* actor)
    {
        if (actor == nullptr)
            return false;

        if (this == actor)
            return false;

        if (!actor->canTalk())
            return false;

        if (isEnemy(actor))
            return false;

        return true;
    }

    void Player::update(bool noclip)
    {
        Actor::update(noclip);

        // handle talking to npcs
        if (mTarget.getType() == Target::Type::Actor)
        {
            Actor* target = mTarget.get<Actor*>();

            if (target && target->getPos().isNear(this->getPos()) && canTalkTo(target))
            {
                if (mWorld.getCurrentPlayer() == this)
                {
                    auto& guiManager = Engine::EngineMain::get()->mGuiManager;
                    guiManager->closeAllPanels();
                    guiManager->mDialogManager.talk(target);
                }
                mTarget.clear();
            }
        }
    }

    void Player::onEnemyKilled(Actor* enemy)
    {
        addExperience(*enemy);
        // TODO: intimidate close fallen demons.
        // TODO: notify quests.
        // TODO: if enemy is Diablo game complete.
    }

    void Player::addExperience(Actor& enemy)
    {
        int32_t exp = enemy.getOnKilledExperience();

        // Adjust exp based on difference in level between player and monster.
        exp = (int32_t)(FixedPoint(exp) * (FixedPoint(1) + (FixedPoint(enemy.getStats().mLevel) - mStats.mLevel) / 10)).round();
        exp = std::max(0, exp);

        mStats.mExperience = std::min(mStats.mExperience + exp, ActorStats::MAXIMUM_EXPERIENCE_POINTS);
        int32_t newLevel = mStats.experiencePointsToLevel(mStats.mExperience);

        // Level up if applicable (it's possible to level up more than once).
        for (int32_t i = mStats.mLevel; i < newLevel; i++)
            levelUp(newLevel);
    }

    void Player::levelUp(int32_t newLevel)
    {
        mStats.mLevel = newLevel;

        // Restore HP/Mana.
        heal();
        restoreMana();
    }

    BaseStats Player::initialiseActorStats(const DiabloExe::CharacterStats& from)
    {
        BaseStats baseStats;
        baseStats.strength = from.mStrength;
        baseStats.dexterity = from.mDexterity;
        baseStats.magic = from.mMagic;
        baseStats.vitality = from.mVitality;

        return baseStats;
    }

    bool Player::castSpell(SpellId spell, Misc::Point targetPoint)
    {
        if (spell == SpellId::null)
        {
            // TODO: Play player sound #34: "I don't have a spell ready"
            return false;
        }

        auto spellData = SpellData(spell);
        auto& mana = mStats.getMana();
        auto manaCost = spellData.manaCost();

        // Note: These checks have temporarily been removed for easier testing/development
        if (!getLevel() || (getLevel()->isTown() && !spellData.canCastInTown()))
        {
            // TODO: Play player sound #27: "I can't cast that here"
            // return false;
        }
        if (mana.current < manaCost)
        {
            // TODO: Play player sound #35: "Not enough mana"
            // return false;
        }

        return Actor::castSpell(spell, targetPoint);
    }

    void Player::doSpellEffect(SpellId spell, Misc::Point targetPoint)
    {
        auto spellData = SpellData(spell);
        auto& mana = mStats.getMana();
        auto manaCost = spellData.manaCost();
        mana.add(-manaCost);
        Actor::doSpellEffect(spell, targetPoint);
    }

    SpellId Player::defaultSkill() const
    {
        switch (mPlayerClass)
        {
            case PlayerClass::warrior:
                return SpellId::repair;
            case PlayerClass::rogue:
                return SpellId::disarm;
            case PlayerClass::sorceror:
                return SpellId::recharge;
            case PlayerClass::none:
                invalid_enum(PlayerClass, mPlayerClass);
        }
        return SpellId::null;
    }
}
