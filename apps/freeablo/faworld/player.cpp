#include "player.h"
#include "../engine/enginemain.h"
#include "../engine/threadmanager.h"
#include "../fagui/dialogmanager.h"
#include "../fagui/guimanager.h"
#include "actorstats.h"
#include "diabloexe/characterstats.h"
#include "equiptarget.h"
#include "item/equipmentitem.h"
#include "item/equipmentitembase.h"
#include "missile/missile.h"
#include "playerbehaviour.h"
#include "spells.h"
#include "world.h"
#include <engine/debugsettings.h>
#include <misc/assert.h>
#include <misc/stringops.h>
#include <render/spritegroup.h>
#include <string>

namespace FAWorld
{
    const std::string Player::typeId = "player";

    Player::Player(World& world, PlayerClass playerClass, const DiabloExe::CharacterStats& charStats) : Actor(world), mPlayerClass(playerClass)
    {
        mStats.initialise(initialiseActorStats(charStats));
        mStats.mLevelXpCounts = charStats.mNextLevelExp;
        switch (mPlayerClass)
        {
            // https://wheybags.gitlab.io/jarulfs-guide/#maximum-stats for max base stats numbers
            case PlayerClass::warrior:
            {
                mStats.baseStats.maxStrength = 250;
                mStats.baseStats.maxMagic = 50;
                mStats.baseStats.maxDexterity = 60;
                mStats.baseStats.maxVitality = 100;
                break;
            }
            case PlayerClass::rogue:
            {
                mStats.baseStats.maxStrength = 50;
                mStats.baseStats.maxMagic = 70;
                mStats.baseStats.maxDexterity = 250;
                mStats.baseStats.maxVitality = 80;
                break;
            }
            case PlayerClass::sorceror:
            {
                mStats.baseStats.maxStrength = 45;
                mStats.baseStats.maxMagic = 250;
                mStats.baseStats.maxDexterity = 85;
                mStats.baseStats.maxVitality = 80;
                break;
            }
            case PlayerClass::none:
                break;
        }

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
        mInventory.mInventoryChanged = [this](EquipTargetType inventoryType, const Item* removed, const Item* added) {
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

            if (added && mPlayerInitialised && this == mWorld.getCurrentPlayer())
            {
                // Play inventory place/grab sound.
                switch (inventoryType)
                {
                    case EquipTargetType::cursor:
                        Engine::ThreadManager::get()->playSound("sfx/items/invgrab.wav");
                        break;
                    default:
                        std::string soundPath = added->getBase()->mInventoryPlaceItemSoundPath;
                        Engine::ThreadManager::get()->playSound(soundPath);
                        break;
                }
            }
        };

        updateSprites();

        if (DebugSettings::PlayersInvuln)
            mInvuln = true;
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

        stats.baseStats.strength = charStats.strength + itemStats.magicStatModifiers.baseStats.strength;
        stats.baseStats.magic = charStats.magic + itemStats.magicStatModifiers.baseStats.magic;
        stats.baseStats.dexterity = charStats.dexterity + itemStats.magicStatModifiers.baseStats.dexterity;
        stats.baseStats.vitality = charStats.vitality + itemStats.magicStatModifiers.baseStats.vitality;

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
                stats.maxLife =
                    (int32_t)(FixedPoint(2) * FixedPoint(charStats.vitality) + FixedPoint(2) * FixedPoint(itemStats.magicStatModifiers.baseStats.vitality) +
                              FixedPoint(2) * FixedPoint(actorStats.mLevel) + FixedPoint(itemStats.magicStatModifiers.maxLife) + 18)
                        .floor();

                stats.maxMana =
                    (int32_t)(FixedPoint(1) * FixedPoint(charStats.magic) + FixedPoint(1) * FixedPoint(itemStats.magicStatModifiers.baseStats.magic) +
                              FixedPoint(1) * FixedPoint(actorStats.mLevel) + FixedPoint(itemStats.magicStatModifiers.maxMana) - 1)
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
                    switch (handItems.meleeWeapon->item->getBase()->mType)
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
                            invalid_enum(ItemType, handItems.meleeWeapon->item->getBase()->mType);
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
                stats.maxLife =
                    (int32_t)(FixedPoint(1) * FixedPoint(charStats.vitality) + FixedPoint("1.5") * FixedPoint(itemStats.magicStatModifiers.baseStats.vitality) +
                              FixedPoint(2) * FixedPoint(actorStats.mLevel) + FixedPoint(itemStats.magicStatModifiers.maxLife) + 23)
                        .floor();

                stats.maxMana =
                    (int32_t)(FixedPoint(1) * FixedPoint(charStats.magic) + FixedPoint("1.5") * FixedPoint(itemStats.magicStatModifiers.baseStats.magic) +
                              FixedPoint(2) * FixedPoint(actorStats.mLevel) + FixedPoint(itemStats.magicStatModifiers.maxMana) + 5)
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
                    switch (handItems.meleeWeapon->item->getBase()->mType)
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
                            invalid_enum(ItemType, handItems.meleeWeapon->item->getBase()->mType);
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
                stats.maxLife =
                    (int32_t)(FixedPoint(1) * FixedPoint(charStats.vitality) + FixedPoint(1) * FixedPoint(itemStats.magicStatModifiers.baseStats.vitality) +
                              FixedPoint(1) * FixedPoint(actorStats.mLevel) + FixedPoint(itemStats.magicStatModifiers.maxLife) + 9)
                        .floor();

                stats.maxMana =
                    (int32_t)(FixedPoint(2) * FixedPoint(charStats.magic) + FixedPoint(2) * FixedPoint(itemStats.magicStatModifiers.baseStats.magic) +
                              FixedPoint(2) * FixedPoint(actorStats.mLevel) + FixedPoint(itemStats.magicStatModifiers.maxMana) - 2)
                        .floor();

                stats.meleeDamage = (int32_t)((FixedPoint(charStats.strength) * actorStats.mLevel) / FixedPoint(100)).floor();
                stats.rangedDamage = (int32_t)((FixedPoint(charStats.strength) * actorStats.mLevel) / FixedPoint(200)).floor();

                stats.toHitMagic.bonus = 20;
                stats.blockChance = mInventory.isShieldEquipped() ? stats.baseStats.dexterity + 10 : 0;

                // https://wheybags.gitlab.io/jarulfs-guide/#weapon-speed
                if (handItems.meleeWeapon)
                {
                    switch (handItems.meleeWeapon->item->getBase()->mType)
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
                            invalid_enum(ItemType, handItems.meleeWeapon->item->getBase()->mType);
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

        stats.toHitMelee.bonus += actorStats.mLevel;
        stats.toHitRanged.bonus += actorStats.mLevel;

        // TODOHELLFIRE: Add in bonuses for barbarians and monks here, see https://wheybags.gitlab.io/jarulfs-guide/#monster-versus-player
        stats.armorClass = (int32_t)(FixedPoint(stats.baseStats.dexterity) / FixedPoint(5) + itemStats.magicStatModifiers.armorClass).floor();
        stats.toHitMelee.base = (int32_t)(FixedPoint(50) + FixedPoint(stats.baseStats.dexterity) / FixedPoint(2) + itemStats.magicStatModifiers.toHit).floor();
        stats.toHitRanged.base = (int32_t)(FixedPoint(50) + FixedPoint(stats.baseStats.dexterity) + itemStats.magicStatModifiers.toHit).floor();
        stats.toHitMagic.base = (int32_t)(FixedPoint(50) + FixedPoint(stats.baseStats.magic)).floor();
        stats.toHitMinMaxCap = {5, 95};

        stats.meleeDamageBonusRange = itemStats.meleeDamageBonusRange;

        // https://wheybags.gitlab.io/jarulfs-guide/#damage-done
        if (!handItems.weapon)
        {
            // TODOHELLFIRE: monks get a bonus here
            if (handItems.shield)
                stats.meleeDamageBonusRange = IntRange(1, 3);
            else
                stats.meleeDamageBonusRange = IntRange(1, 1);
        }

        stats.rangedDamageBonusRange = itemStats.rangedDamageBonusRange;
        stats.hitRecoveryDamageThreshold = actorStats.mLevel;
    }

    DamageType Player::getMeleeDamageType() const
    {
        EquippedInHandsItems handsItems = mInventory.getItemsInHands();

        if (handsItems.weapon)
        {
            if (handsItems.weapon->item->getBase()->mType == ItemType::mace)
                return DamageType::Club;
            if (handsItems.weapon->item->getBase()->mType == ItemType::sword)
                return DamageType::Sword;
            if (handsItems.weapon->item->getBase()->mType == ItemType::axe)
                return DamageType::Axe;
            if (handsItems.weapon->item->getBase()->mType == ItemType::bow)
                return DamageType::Bow;
            if (handsItems.weapon->item->getBase()->mType == ItemType::staff)
                return DamageType::Staff;
        }

        return DamageType::Unarmed;
    }

    Player::Player(World& world, FASaveGame::GameLoader& loader) : Actor(world, loader)
    {
        mPlayerClass = static_cast<PlayerClass>(loader.load<int32_t>());
        initCommon();
        mPlayerInitialised = true;

        mAnimation.markAnimationsRestoredAfterGameLoad();
        loader.addFunctionToRunAtEnd([this]() { updateSprites(); });
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
        std::string classCode = Misc::StringUtils::toLower(playerClassToString(this->getClass()));

        std::string armor;
        {
            if (!mInventory.getBody())
            {
                armor = "none";
            }
            else
            {
                switch (mInventory.getBody()->getBase()->mType)
                {
                    case ItemType::heavyArmor:
                        armor = "heavy-armor";
                        break;

                    case ItemType::mediumArmor:
                        armor = "medium-armor";
                        break;

                    case ItemType::lightArmor:
                        armor = "light-armor";
                        break;

                    default:
                        invalid_enum(ItemType, mInventory.getBody()->getBase()->mType);
                }
            }
        }

        std::string weapon;
        {
            EquippedInHandsItems handsItems = mInventory.getItemsInHands();

            if (!handsItems.weapon)
                weapon = "none";
            else
            {
                switch (handsItems.weapon.value().item->getBase()->mType)
                {
                    case ItemType::sword:
                        weapon = "sword";
                        break;
                    case ItemType::mace:
                        weapon = "mace";
                        break;
                    case ItemType::axe:
                        weapon = "axe";
                        break;
                    case ItemType::staff:
                        weapon = "staff";
                        break;
                    case ItemType::bow:
                        weapon = "bow";
                        break;
                    default:
                        invalid_enum(ItemType, handsItems.weapon.value().item->getBase()->mType);
                }

                if (handsItems.weapon.value().item->getBase()->getEquipType() == ItemEquipType::twoHanded)
                    weapon = weapon + "-2h";
                else
                    weapon = weapon + "-1h";
            }

            if (handsItems.shield)
                weapon = weapon + "-shield";
        }

        FARender::Renderer* renderer = FARender::Renderer::get();
        if (!renderer) // TODO: some sort of headless mode for tests
            return;

        auto getAnimation = [&](const std::string& animation) {
            FARender::SpriteLoader::PlayerSpriteKey spriteLookupKey({{"animation", animation}, {"class", classCode}, {"armor", armor}, {"weapon", weapon}});
            return renderer->mSpriteLoader.getSprite(renderer->mSpriteLoader.mPlayerSpriteDefinitions.at(spriteLookupKey));
        };

        mAnimation.setAnimationSprites(AnimState::dead, getAnimation("dead"));
        mAnimation.setAnimationSprites(AnimState::attack, getAnimation("attack"));
        mAnimation.setAnimationSprites(AnimState::hit, getAnimation("hit"));
        mAnimation.setAnimationSprites(AnimState::spellLightning, getAnimation("cast-lightning"));
        mAnimation.setAnimationSprites(AnimState::spellFire, getAnimation("cast-fire"));
        mAnimation.setAnimationSprites(AnimState::spellOther, getAnimation("cast-magic"));
        mAnimation.setAnimationSprites(AnimState::block, getAnimation("block"));

        if (getLevel() && getLevel()->isTown())
        {
            mAnimation.setAnimationSprites(AnimState::walk, getAnimation("walk-town"));
            mAnimation.setAnimationSprites(AnimState::idle, getAnimation("idle-town"));
        }
        else
        {
            mAnimation.setAnimationSprites(AnimState::walk, getAnimation("walk-dungeon"));
            mAnimation.setAnimationSprites(AnimState::idle, getAnimation("idle-dungeon"));
        }

        // TODO: Is this actually correct? It seems kind of odd, but it is what is listed in Jarulf's guide
        // https://wheybags.gitlab.io/jarulfs-guide/#player-timing-information
        mMeleeHitFrame = mAnimation.getAnimationSprites(AnimState::attack)->getAnimationLength() - 1;
    }

    bool Player::dropItem(const Misc::Point& clickedPoint)
    {
        Misc::Direction initialDir = (Vec2Fix(clickedPoint.x, clickedPoint.y) - Vec2Fix(getPos().current().x, getPos().current().y)).getDirection();
        Vec2i curPos = getPos().current();
        Misc::Direction direction = (curPos == clickedPoint) ? Misc::Direction(Misc::Direction8::none) : initialDir;

        std::unique_ptr<Item> tmp = mInventory.remove(MakeEquipTarget<EquipTargetType::cursor>());
        bool retval = getLevel()->dropItemClosestEmptyTile(tmp, *this, curPos, direction);
        mInventory.setCursorHeld(std::move(tmp));

        return retval;
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

    void Player::handleTargetingLevelTransitions()
    {
        for (const auto* transition : {&getLevel()->upStairsArea(), &getLevel()->downStairsArea()})
        {
            Vec2i exitPoint = transition->offset + transition->exitOffset;

            // If the player is targeting anywhere near the exit, target the exit
            for (int32_t y = 0; y < transition->triggerMask.height(); y++)
            {
                for (int32_t x = 0; x < transition->triggerMask.width(); x++)
                {
                    if (transition->triggerMask.get(x, y) && mMoveHandler.getDestination() == transition->offset + Vec2i(x, y))
                        mMoveHandler.setDestination(exitPoint);
                }
            }

            if (getPos().current() == exitPoint && mMoveHandler.getDestination() == exitPoint)
            {
                if (GameLevel* level = getWorld()->getLevel(transition->targetLevelIndex))
                    moveToLevel(level, transition == &getLevel()->downStairsArea());
            }
        }
    }

    void Player::update(bool noclip)
    {
        handleTargetingLevelTransitions();
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

    void Player::addStrength(int32_t delta) { mStats.baseStats.strength = std::min(mStats.baseStats.strength + delta, mStats.baseStats.maxStrength); }
    void Player::addMagic(int32_t delta) { mStats.baseStats.magic = std::min(mStats.baseStats.magic + delta, mStats.baseStats.maxMagic); }
    void Player::addDexterity(int32_t delta) { mStats.baseStats.dexterity = std::min(mStats.baseStats.dexterity + delta, mStats.baseStats.maxDexterity); }
    void Player::addVitality(int32_t delta) { mStats.baseStats.vitality = std::min(mStats.baseStats.vitality + delta, mStats.baseStats.maxVitality); }

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

    void Player::moveToLevel(GameLevel* level, bool placeAtUpStairs)
    {
        const Level::LevelTransitionArea& targetArea = placeAtUpStairs ? level->upStairsArea() : level->downStairsArea();
        Vec2i targetPoint = level->getFreeSpotNear(targetArea.offset + targetArea.playerSpawnOffset, std::numeric_limits<int32_t>::max());
        teleport(level, Position(targetPoint));
    }
}
