#include "player.h"
#include "../engine/threadmanager.h"
#include "../fagui/dialogmanager.h"
#include "../falevelgen/random.h"
#include "../fasavegame/gameloader.h"
#include "actorstats.h"
#include "diabloexe/characterstats.h"
#include "equiptarget.h"
#include "itemenums.h"
#include "itemmap.h"
#include "playerbehaviour.h"
#include "world.h"
#include <misc/assert.h>
#include <misc/stringops.h>
#include <string>

namespace FAWorld
{
    const std::string Player::typeId = "player";

    Player::Player() : Actor(), mInventory(this)
    {
        // TODO: hack - need to think of some more elegant way of handling Actors in general
        DiabloExe::CharacterStats stats;
        init("Warrior", stats);
        initCommon();
    }

    Player::Player(const std::string& className, const DiabloExe::CharacterStats& charStats) : Actor(), mInventory(this)
    {
        init(className, charStats);
        initCommon();
    }

    void Player::initCommon()
    {
        FAWorld::World::get()->registerPlayer(this);
        mInventory.equipChanged.connect([this]() { updateSprites(); });
    }

    void Player::init(const std::string& className, const DiabloExe::CharacterStats& charStats)
    {
        UNUSED_PARAM(className);
        UNUSED_PARAM(charStats);

        mFaction = Faction::heaven();
        mMoveHandler = MovementHandler(World::getTicksInPeriod(0.1f)); // allow players to repath much more often than other actors

        mStats.mAttackDamage = 60;

        mBehaviour = new PlayerBehaviour(this);
    }

    Player::Player(FASaveGame::GameLoader& loader) : Actor(loader), mInventory(this)
    {
        mClassName = loader.load<std::string>();
        FAWorld::World::get()->registerPlayer(this);
    }

    void Player::save(FASaveGame::GameSaver& saver)
    {
        Serial::ScopedCategorySaver cat("Player", saver);

        Actor::save(saver);
        saver.save(mClassName);
    }

    Player::~Player() { FAWorld::World::get()->deregisterPlayer(this); }

    void Player::setSpriteClass(std::string className)
    {
        mClassName = className;
        updateSprites();
    }

    void Player::updateSprites()
    {
        std::string classCode;
        classCode = mClassName[0];
        std::string armourCode;
        std::string weaponCode;
        bool inDungeon = false;

        std::string armour, weapon;
        switch (mInventory.mBody.getType())
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
        if (mInventory.mLeftHand.isEmpty() && mInventory.mRightHand.isEmpty())
        {
            weapon = "n";
        }
        else if ((mInventory.mLeftHand.isEmpty() && !mInventory.mRightHand.isEmpty()) || (!mInventory.mLeftHand.isEmpty() && mInventory.mRightHand.isEmpty()))
        {
            Item hand;

            if (mInventory.mRightHand.isEmpty())
                hand = mInventory.mLeftHand;
            else
                hand = mInventory.mRightHand;
            switch (hand.getType())
            {
                case ItemType::axe:
                {
                    if (hand.getEquipLoc() == ItemEquipType::oneHanded)
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

        else if (!mInventory.mLeftHand.isEmpty() && !mInventory.mRightHand.isEmpty())
        {
            if ((mInventory.mLeftHand.getType() == ItemType::sword && mInventory.mRightHand.getType() == ItemType::shield) ||
                (mInventory.mLeftHand.getType() == ItemType::shield && mInventory.mRightHand.getType() == ItemType::sword))
                weapon = "d";

            else if (mInventory.mLeftHand.getType() == ItemType::bow && mInventory.mRightHand.getType() == ItemType::bow)
                weapon = "b";

            else if (mInventory.mLeftHand.getType() == ItemType::staff && mInventory.mRightHand.getType() == ItemType::staff)
                weapon = "t";
            else if (mInventory.mLeftHand.getType() == ItemType::mace || mInventory.mRightHand.getType() == ItemType::mace)
                weapon = "h";

            release_assert(!weapon.empty()); // Empty weapon format
        }
        weaponCode = weapon;
        armourCode = armour;

        if (getLevel() && getLevel()->getLevelIndex() != 0)
            inDungeon = true;
        else
            inDungeon = false;

        auto helper = [&](bool isDie) {
            std::string weapFormat = weaponCode;

            if (isDie)
                weapFormat = "n";

            boost::format fmt("plrgfx/%s/%s%s%s/%s%s%s%s.cl2");
            fmt % mClassName % classCode % armourCode % weapFormat % classCode % armourCode % weapFormat;
            return fmt;
        };

        auto renderer = FARender::Renderer::get();

        mAnimation.setAnimation(AnimState::dead, renderer->loadImage((helper(true) % "dt").str()));
        mAnimation.setAnimation(AnimState::attack, renderer->loadImage((helper(false) % "at").str()));
        mAnimation.setAnimation(AnimState::hit, renderer->loadImage((helper(false) % "ht").str()));

        if (inDungeon)
        {
            mAnimation.setAnimation(AnimState::walk, renderer->loadImage((helper(false) % "aw").str()));
            mAnimation.setAnimation(AnimState::idle, renderer->loadImage((helper(false) % "as").str()));
        }
        else
        {
            mAnimation.setAnimation(AnimState::walk, renderer->loadImage((helper(false) % "wl").str()));
            mAnimation.setAnimation(AnimState::idle, renderer->loadImage((helper(false) % "st").str()));
        }
    }

    void Player::pickupItem(ItemTarget target)
    {
        auto& itemMap = getLevel()->getItemMap();
        auto tile = target.item->getTile();
        auto item = itemMap.takeItemAt(tile);
        auto dropBack = [&]() { itemMap.dropItem(std::move(item), *this, tile); };
        switch (target.action)
        {
            case ItemTarget::ActionType::autoEquip:
                if (!getInventory().autoPlaceItem(*item))
                    dropBack();
                break;
            case ItemTarget::ActionType::toCursor:
                auto cursorItem = getInventory().getItemAt(MakeEquipTarget<EquipTargetType::cursor>());
                if (!cursorItem.isEmpty())
                    return dropBack();

                getInventory().setCursorHeld(*item);
                break;
        }
    }

    bool Player::dropItem(const FAWorld::Tile& clickedTile)
    {
        auto cursorItem = getInventory().getItemAt(MakeEquipTarget<EquipTargetType::cursor>());
        auto initialDir = Misc::getVecDir(Misc::getVec(getPos().current(), {clickedTile.x, clickedTile.y}));
        auto curPos = getPos().current();
        auto tryDrop = [&](const std::pair<int32_t, int32_t>& pos) {
            if (getLevel()->dropItem(std::unique_ptr<Item>{new Item(cursorItem)}, *this, FAWorld::Tile(pos.first, pos.second)))
            {
                getInventory().setCursorHeld({});
                return true;
            }
            return false;
        };

        auto isPosOk = [&](std::pair<int32_t, int32_t> pos) {
            return getLevel()->isPassableFor(pos.first, pos.second, this) && !getLevel()->getItemMap().getItemAt({pos.first, pos.second});
        };

        if (clickedTile == FAWorld::Tile{curPos.first, curPos.second})
        {
            if (isPosOk(curPos))
                return tryDrop(curPos);
            initialDir = 7; // this is hack to emulate original game behavior, diablo's 0th direction is our 7th unfortunately
        }

        for (auto diff : {0, -1, 1})
        {
            auto dir = (initialDir + diff + 8) % 8;
            auto pos = Misc::getNextPosByDir(curPos, dir);
            if (isPosOk(pos))
                return tryDrop(pos);
        }

        if (isPosOk(curPos))
            return tryDrop(curPos);
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

        if (isTalking)
            return false;

        if (isEnemy(actor))
            return false;

        return true;
    }

    void Player::update(bool noclip)
    {
        Actor::update(noclip);

        // handle talking to npcs
        if (mTarget.which() == 1) // targeting actor
        {
            Actor* target = boost::get<Actor*>(mTarget);

            if (target && target->getPos().isNear(this->getPos()) && canTalkTo(target))
            {
                World::get()->mDlgManager->talk(target);
                mTarget = boost::blank{};
            }
        }
    }
}
