#include "player.h"
#include "world.h"
#include "actorstats.h"
#include "characterstats.h"
#include "../falevelgen/random.h"
#include "../engine/threadmanager.h"
#include <misc/stringops.h>
#include <string>
#include "itemmap.h"

namespace FAWorld
{
    STATIC_HANDLE_NET_OBJECT_IN_IMPL(Player)

    Player::Player() : Actor(), mInventory(this)
    {
        //TODO: hack - need to think of some more elegant way of handling Actors in general
        DiabloExe::CharacterStats stats;
        init("Warrior", stats);
    }

    Player::Player(const std::string& className, const DiabloExe::CharacterStats& charStats) : Actor(), mInventory(this)
    {
        init(className, charStats);
        mMoveHandler = MovementHandler(World::getTicksInPeriod(0.1f)); // allow players to repath much more often than other actors
    }

    void Player::init(const std::string& className, const DiabloExe::CharacterStats& charStats)
    {
        if (className == "Warrior")
            mStats = new FAWorld::MeleeStats(charStats, this);
        else if (className == "Rogue")
            mStats = new FAWorld::RangerStats(charStats, this);
        else if (className == "Sorcerer")
            mStats = new FAWorld::MageStats(charStats, this);

        setSpriteClass(Misc::StringUtils::lowerCase(className));

        mStats->setActor(this);
        mStats->recalculateDerivedStats();

        mFaction = Faction::heaven();

        FAWorld::World::get()->registerPlayer(this);
    }

    Player::~Player()
    {
        FAWorld::World::get()->deregisterPlayer(this);
    }

    bool Player::talk(Actor* actor)
    {
        UNUSED_PARAM(actor);
        //assert(false);
        return true;
    }

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
        switch(mInventory.mBody.getCode())
        {
               case Item::icHeavyArmour:
               {
                    armour="h";
                    break;
               }

               case Item::icMidArmour:
               {
                    armour="m";
                    break;
               }

               case Item::icLightArmour:
               default:
               {
                    armour="l";
                    break;
               }
        }
        if(mInventory.mLeftHand.isEmpty() && mInventory.mRightHand.isEmpty())
        {
            weapon = "n";
        }
        else if((mInventory.mLeftHand.isEmpty() && !mInventory.mRightHand.isEmpty()) || (!mInventory.mLeftHand.isEmpty() && mInventory.mRightHand.isEmpty()))
        {
            Item hand;

            if(mInventory.mRightHand.isEmpty())
                hand = mInventory.mLeftHand;
            else
                hand = mInventory.mRightHand;
            switch(hand.getCode())
            {
                case Item::icAxe:
                {
                    if(hand.getEquipLoc() == Item::eqONEHAND)
                        weapon = "s";
                    else
                        weapon = "a";
                    break;
                }

                case Item::icBlunt:
                {
                    weapon = "m";
                    break;
                }

                case Item::icBow:
                {
                    weapon = "b";
                    break;
                }

                case Item::icShield:
                {
                    weapon = "u";
                    break;
                }

                case Item::icSword:
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

        else if(!mInventory.mLeftHand.isEmpty() && !mInventory.mRightHand.isEmpty())
        {
            if((mInventory.mLeftHand.getCode() == Item::icSword && mInventory.mRightHand.getCode() == Item::icShield) || (mInventory.mLeftHand.getCode() == Item::icShield && mInventory.mRightHand.getCode() == Item::icSword))
                weapon = "d";

            else if(mInventory.mLeftHand.getCode() == Item::icBow && mInventory.mRightHand.getCode() == Item::icBow)
                weapon = "b";

            else if(mInventory.mLeftHand.getCode() == Item::icStave && mInventory.mRightHand.getCode() == Item::icStave)
                weapon = "t";
            else if(mInventory.mLeftHand.getCode() == Item::icBlunt || mInventory.mRightHand.getCode() == Item::icBlunt)
                weapon = "h";

            assert (!weapon.empty ()); // Empty weapon format
        }
        weaponCode = weapon;
        armourCode = armour;

        if(getLevel() && getLevel()->getLevelIndex() != 0)
            inDungeon=true;
        else
            inDungeon=false;

        auto helper = [&](bool isDie)
        {
            std::string weapFormat = weaponCode;

            if (isDie)
                weapFormat = "n";

            boost::format fmt("plrgfx/%s/%s%s%s/%s%s%s%s.cl2");
            fmt % mClassName % classCode % armourCode % weapFormat % classCode % armourCode % weapFormat;
            return fmt;
        };

        auto renderer = FARender::Renderer::get();

        getAnimationManager().setAnimation(AnimState::dead, renderer->loadImage((helper(true) % "dt").str()));
        getAnimationManager().setAnimation(AnimState::attack, renderer->loadImage((helper(false) % "at").str()));
        getAnimationManager().setAnimation(AnimState::hit, renderer->loadImage((helper(false) % "ht").str()));

        if (inDungeon)
        {
            getAnimationManager().setAnimation(AnimState::walk, renderer->loadImage((helper(false) % "aw").str()));
            getAnimationManager().setAnimation(AnimState::idle, renderer->loadImage((helper(false) % "as").str()));
        }
        else
        {
            getAnimationManager().setAnimation(AnimState::walk, renderer->loadImage((helper(false) % "wl").str()));
            getAnimationManager().setAnimation(AnimState::idle, renderer->loadImage((helper(false) % "st").str()));
        }
    }

    void Player::pickupItem(ItemTarget target)
    {
      auto &itemMap = getLevel()->getItemMap();
      auto tile = target.item->getTile();
      auto item = itemMap.takeItemAt (tile);
      auto dropBack = [&](){ itemMap.dropItem(std::move (item), *this, tile); };
      switch (target.action)
      {
      case ItemTarget::ActionType::autoEquip:
          if (!getInventory().autoPlaceItem(*item))
            dropBack ();
          break;
      case ItemTarget::ActionType::toCursor:
          auto cursorItem = getInventory ().getItemAt(MakeEquipTarget<Item::eqCURSOR> ());
          if (!cursorItem.isEmpty ())
              return dropBack ();

          getInventory ().setCursorHeld(*item);
          break;
      }
    }

    bool Player::dropItem(const FAWorld::Tile& clickedTile)
    {
       auto cursorItem = getInventory ().getItemAt(MakeEquipTarget<Item::eqCURSOR> ());
       auto initialDir = Misc::getVecDir (Misc::getVec (getPos().current(), {clickedTile.x, clickedTile.y}));
       auto curPos = getPos().current ();
       auto tryDrop = [&](const std::pair<int32_t, int32_t> &pos){
           if (getLevel()->dropItem (std::unique_ptr<Item> {new Item (cursorItem)}, *this, {pos.first, pos.second}))
               {
                   getInventory ().setCursorHeld({});
                   return true;
               }
           return false;
       };

       auto isPosOk = [&](std::pair<int32_t, int32_t> pos)
       {
           return getLevel()->isPassableFor(pos.first, pos.second, this) && !getLevel()->getItemMap().getItemAt({pos.first, pos.second});
       };

       if (clickedTile == FAWorld::Tile {curPos.first, curPos.second})
       {
           if (isPosOk (curPos))
             return tryDrop (curPos);
           initialDir = 7; // this is hack to emulate original game behavior, diablo's 0th direction is our 7th unfortunately
       }

        for (auto diff : {0, -1, 1})
        {
            auto dir = (initialDir + diff + 8) % 8;
            auto pos = Misc::getNextPosByDir (curPos, dir);
            if (isPosOk (pos))
                return tryDrop (pos);
        }

      if (isPosOk (curPos))
          return tryDrop (curPos);
      return false;
    }
}
