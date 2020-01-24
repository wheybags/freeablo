#include "player.h"
#include "../engine/enginemain.h"
#include "../engine/threadmanager.h"
#include "../fagui/dialogmanager.h"
#include "../fagui/guimanager.h"
#include "../fasavegame/gameloader.h"
#include "actorstats.h"
#include "boost/algorithm/clamp.hpp"
#include "diabloexe/characterstats.h"
#include "equiptarget.h"
#include "itemenums.h"
#include "itemmap.h"
#include "missile/missile.h"
#include "playerbehaviour.h"
#include "world.h"
#include <misc/assert.h>
#include <misc/stringops.h>
#include <misc/vec2fix.h>
#include <random/random.h>
#include <string>

namespace FAWorld
{
    const std::string Player::typeId = "player";

    const char* toString(PlayerClass value)
    {
        switch (value)
        {
            case PlayerClass::warrior:
                return "warrior";
            case PlayerClass::rogue:
                return "rogue";
            case PlayerClass::sorcerer:
                return "sorceror";
            case PlayerClass::none:
                break;
        }
        return "unknown";
    }

    Player::Player(World& world) : Actor(world)
    {
        // TODO: hack - need to think of some more elegant way of handling Actors in general
        DiabloExe::CharacterStats stats;
        init(stats);
        initCommon();
    }

    Player::Player(World& world, const DiabloExe::CharacterStats& charStats) : Actor(world)
    {
        init(charStats);
        initCommon();
    }

    void Player::initCommon()
    {
        mWorld.registerPlayer(this);
        mInventory.mInventoryChanged.connect([this](EquipTargetType inventoryType, Item const& removed, Item const& added) {
            (void)removed;

            // Update player graphics.
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
        });
    }

    void Player::setPlayerClass(PlayerClass playerClass)
    {
        mPlayerClass = playerClass;
        updateSprites();
    }

    int32_t Player::meleeDamageVs(const Actor* /*actor*/) const
    {
        const LiveActorStats& stats = mStats.getCalculatedStats();
        int32_t damage = stats.meleeDamage;
        damage += mWorld.mRng->randomInRange(stats.meleeDamageBonusRange.start, stats.meleeDamageBonusRange.end);

        if (mPlayerClass == PlayerClass::warrior && mWorld.mRng->randomInRange(0, 99) < mStats.mLevel)
            damage *= 2;

        return damage;
    }

    void Player::calculateStats(LiveActorStats& stats) const
    {
        BaseStats charStats = mStats.baseStats;

        ItemStats itemStats;
        mInventory.calculateItemBonuses(itemStats);

        stats.baseStats.strength = charStats.strength + itemStats.baseStats.strength;
        stats.baseStats.magic = charStats.magic + itemStats.baseStats.magic;
        stats.baseStats.dexterity = charStats.dexterity + itemStats.baseStats.dexterity;
        stats.baseStats.vitality = charStats.vitality + itemStats.baseStats.vitality;

        stats.toHitMelee.bonus = 0;
        stats.toHitRanged.bonus = 0;
        stats.toHitMagic.bonus = 0;

        // TODO: make sure all the following calculations should be rounded

        switch (mPlayerClass)
        {
            case PlayerClass::warrior:
            {
                stats.maxLife = (int32_t)(MakeFixed(2) * FixedPoint(charStats.vitality) + MakeFixed(2) * FixedPoint(itemStats.baseStats.vitality) +
                                          MakeFixed(2) * FixedPoint(mStats.mLevel) + FixedPoint(itemStats.maxLife) + 18)
                                    .round();

                stats.maxMana = (int32_t)(MakeFixed(1) * FixedPoint(charStats.magic) + MakeFixed(1) * FixedPoint(itemStats.baseStats.magic) +
                                          MakeFixed(1) * FixedPoint(mStats.mLevel) + FixedPoint(itemStats.maxMana) - 1)
                                    .round();

                stats.meleeDamage = (int32_t)((FixedPoint(charStats.strength) * mStats.mLevel) / FixedPoint(100)).round();
                stats.rangedDamage = (int32_t)((FixedPoint(charStats.strength) * mStats.mLevel) / FixedPoint(200)).round();

                stats.toHitMelee.bonus = 20;
                stats.toHitRanged.bonus = 10;
                break;
            }
            case PlayerClass::rogue:
            {
                stats.maxLife = (int32_t)(MakeFixed(1) * FixedPoint(charStats.vitality) + MakeFixed(1, 5) * FixedPoint(itemStats.baseStats.vitality) +
                                          MakeFixed(2) * FixedPoint(mStats.mLevel) + FixedPoint(itemStats.maxLife) + 23)
                                    .round();

                stats.maxMana = (int32_t)(MakeFixed(1) * FixedPoint(charStats.magic) + MakeFixed(1, 5) * FixedPoint(itemStats.baseStats.magic) +
                                          MakeFixed(2) * FixedPoint(mStats.mLevel) + FixedPoint(itemStats.maxMana) + 5)
                                    .round();

                stats.meleeDamage = (int32_t)(((FixedPoint(charStats.strength) + FixedPoint(charStats.dexterity)) * mStats.mLevel) / FixedPoint(100)).round();
                stats.rangedDamage = (int32_t)(((FixedPoint(charStats.strength) + FixedPoint(charStats.dexterity)) * mStats.mLevel) / FixedPoint(100)).round();

                stats.toHitRanged.bonus = 20;
                break;
            }
            case PlayerClass::sorcerer:
            {
                stats.maxLife = (int32_t)(MakeFixed(1) * FixedPoint(charStats.vitality) + MakeFixed(1) * FixedPoint(itemStats.baseStats.vitality) +
                                          MakeFixed(1) * FixedPoint(mStats.mLevel) + FixedPoint(itemStats.maxLife) + 9)
                                    .round();

                stats.maxMana = (int32_t)(MakeFixed(2) * FixedPoint(charStats.magic) + MakeFixed(2) * FixedPoint(itemStats.baseStats.magic) +
                                          MakeFixed(2) * FixedPoint(mStats.mLevel) + FixedPoint(itemStats.maxMana) - 2)
                                    .round();

                stats.meleeDamage = (int32_t)((FixedPoint(charStats.strength) * mStats.mLevel) / FixedPoint(100)).round();
                stats.rangedDamage = (int32_t)((FixedPoint(charStats.strength) * mStats.mLevel) / FixedPoint(200)).round();

                stats.toHitMagic.bonus = 20;
                break;
            }
            case PlayerClass::none:
                invalid_enum(PlayerClass, mPlayerClass);
        }

        stats.armorClass = (int32_t)(FixedPoint(stats.baseStats.dexterity) / MakeFixed(5) + itemStats.armorClass).round();
        stats.toHitMelee.base = (int32_t)(FixedPoint(50) + FixedPoint(stats.baseStats.dexterity) / MakeFixed(2) + itemStats.toHit).round();
        stats.toHitRanged.base = (int32_t)(FixedPoint(50) + FixedPoint(stats.baseStats.dexterity) + itemStats.toHit).round();
        stats.toHitMagic.base = (int32_t)(FixedPoint(50) + FixedPoint(stats.baseStats.magic)).round();

        stats.meleeDamageBonusRange = itemStats.meleeDamageBonusRange;
        stats.rangedDamageBonusRange = itemStats.rangedDamageBonusRange;
    }

    void Player::init(const DiabloExe::CharacterStats& charStats)
    {
        initialiseActorStats(mStats, charStats);

        mFaction = Faction::heaven();
        mMoveHandler = MovementHandler(World::getTicksInPeriod("0.1")); // allow players to repath much more often than other actors
        mBehaviour.reset(new PlayerBehaviour(this));
    }

    Player::Player(World& world, FASaveGame::GameLoader& loader) : Actor(world, loader)
    {
        mPlayerClass = static_cast<PlayerClass>(loader.load<int32_t>());
        mActiveMissileIndex = loader.load<uint32_t>();
        initCommon();
        mPlayerInitialised = true;
    }

    void Player::save(FASaveGame::GameSaver& saver)
    {
        release_assert(mPlayerInitialised);

        Serial::ScopedCategorySaver cat("Player", saver);

        Actor::save(saver);
        saver.save(static_cast<int32_t>(mPlayerClass));
        saver.save(mActiveMissileIndex);
    }

    bool Player::checkHit(Actor* enemy)
    {
        UNUSED_PARAM(enemy); // TODO: this should take into account target's AC when attacking a player

        int32_t roll = mWorld.mRng->randomInRange(0, 99);
        int32_t toHit = boost::algorithm::clamp(mStats.getCalculatedStats().toHitMelee.getCombined(), 5, 95);

        return roll < toHit;
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
            case PlayerClass::sorcerer:
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

        auto helper = [&](bool isDie) {
            std::string weapFormat = weaponCode;

            if (isDie)
                weapFormat = "n";

            boost::format fmt("plrgfx/%s/%s%s%s/%s%s%s%s.cl2");
            fmt % toString(mPlayerClass) % classCode % armourCode % weapFormat % classCode % armourCode % weapFormat;
            return fmt;
        };

        auto renderer = FARender::Renderer::get();

        // TODO: Spell animations: lightning "lm", fire "fm", other "qm"
        mAnimation.setAnimation(AnimState::dead, renderer->loadImage((helper(true) % "dt").str()));
        mAnimation.setAnimation(AnimState::attack, renderer->loadImage((helper(false) % "at").str()));
        mAnimation.setAnimation(AnimState::hit, renderer->loadImage((helper(false) % "ht").str()));

        if (getLevel() && getLevel()->isTown())
        {
            mAnimation.setAnimation(AnimState::walk, renderer->loadImage((helper(false) % "wl").str()));
            mAnimation.setAnimation(AnimState::idle, renderer->loadImage((helper(false) % "st").str()));
        }
        else
        {
            mAnimation.setAnimation(AnimState::walk, renderer->loadImage((helper(false) % "aw").str()));
            mAnimation.setAnimation(AnimState::idle, renderer->loadImage((helper(false) % "as").str()));
        }
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
        if (Monster* monster = dynamic_cast<Monster*>(enemy))
        {
            addExperience(*monster);
            // TODO: intimidate close fallen demons.
            // TODO: notify quests.
            // TODO: if enemy is Diablo game complete.
        }
    }

    void Player::addExperience(Monster& enemy)
    {
        int32_t exp = enemy.getKillExp();

        // Adjust exp based on difference in level between player and monster.
        exp = (int32_t)(FixedPoint(exp) * (MakeFixed(1) + (FixedPoint(enemy.getMonsterStats().level) - mStats.mLevel) / 10)).round();
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

        // Increase HP/Mana.
        switch (mPlayerClass)
        {
            case PlayerClass::warrior:
                mStats.mHp.max += 2;
                mStats.mMana.max += 1;
                break;
            case PlayerClass::rogue:
                mStats.mHp.max += 2;
                mStats.mMana.max += 2;
                break;
            case PlayerClass::sorcerer:
                mStats.mHp.max += 1;
                mStats.mMana.max += 2;
                break;
            case PlayerClass::none:
                invalid_enum(PlayerClass, mPlayerClass);
        }

        // Restore HP/Mana.
        heal();
        restoreMana();
    }

    void Player::initialiseActorStats(ActorStats& stats, const DiabloExe::CharacterStats& from)
    {
        BaseStats baseStats;
        baseStats.strength = from.mStrength;
        baseStats.dexterity = from.mDexterity;
        baseStats.magic = from.mMagic;

        stats = ActorStats(*stats.mActor, baseStats, from.mNextLevelExp);
    }

    static const std::vector<MissileId> mImplementedMissiles = {
        MissileId::arrow, MissileId::firebolt, MissileId::firewall, MissileId::manashield, MissileId::farrow, MissileId::larrow};

    void Player::setActiveSpellNumber(int32_t spellNumber)
    {
        (void)spellNumber;
        // Hack for testing, loop through implemented missiles.
        mActiveMissileIndex++;
        if (mActiveMissileIndex >= mImplementedMissiles.size())
            mActiveMissileIndex = 0;
    }

    void Player::castActiveSpell(Misc::Point targetPoint)
    {
        // Hack for testing, loop through implemented missiles.
        auto missileId = mImplementedMissiles[mActiveMissileIndex];
        switch (missileId)
        {
            case MissileId::arrow:
            case MissileId::farrow:
            case MissileId::larrow:
                // Arrow sounds will need to be implemented like Actor::doMeleeHit().
                Engine::ThreadManager::get()->playSound("sfx/misc/bfire.wav");
                break;
            default:
                // Spell sounds will come from DiabloExe::getSpellsDataTable()[spellId].mSoundEffect.
                break;
        }
        activateMissile(missileId, targetPoint);
    }
}
