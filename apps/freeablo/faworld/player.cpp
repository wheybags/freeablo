#include "player.h"
#include "world.h"
#include "actorstats.h"
#include "characterstats.h"
#include "../falevelgen/random.h"
#include "../engine/threadmanager.h"
#include <boost/python.hpp>

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
    }

    void Player::init(const std::string& className, const DiabloExe::CharacterStats& charStats)
    {
        if (className == "Warrior")
            mStats = new FAWorld::MeleeStats(charStats, this);
        else if (className == "Rogue")
            mStats = new FAWorld::RangerStats(charStats, this);
        else if (className == "Sorcerer")
            mStats = new FAWorld::MageStats(charStats, this);

        mStats->setActor(this);
        mStats->recalculateDerivedStats();

        mAnimTimeMap[AnimState::dead] = FAWorld::World::getTicksInPeriod(0.1f);
        mAnimTimeMap[AnimState::walk] = FAWorld::World::getTicksInPeriod(0.1f);
        mAnimTimeMap[AnimState::attack] = FAWorld::World::getTicksInPeriod(0.2f);
        mAnimTimeMap[AnimState::idle] = FAWorld::World::getTicksInPeriod(0.1f);

        FAWorld::World::get()->registerPlayer(this);
    }

    Player::~Player()
    {
        FAWorld::World::get()->deregisterPlayer(this);
    }

  bool Player::isMoving() {
      return mPos.mMoving;
  }

  bool Player::attack(Actor *enemy)
    {
        if(enemy->isDead() && enemy->mStats != nullptr)
            return false;
        isAttacking = true;
        Engine::ThreadManager::get()->playSound(FALevelGen::chooseOne({"sfx/misc/swing2.wav", "sfx/misc/swing.wav"}));
        enemy->takeDamage((uint32_t)mStats->getMeleeDamage());
        if(enemy->getCurrentHP() <= 0)
            enemy->die();
        setAnimation(AnimState::attack, true);
        mAnimPlaying = true;
        return true;
    }

    bool Player::attack(Player *enemy)
    {
        UNUSED_PARAM(enemy);
        return false;
    }

    bool Player::talk(Actor * actor)
    {
        isTalking = true;

        using namespace boost::python;

        try
        {
            object module = import("dialogues");
            object talkTo = module.attr("talkTo");

            talkTo(actor->getActorId().c_str());
        }
        catch(...)
        {
                PyErr_Print();
                PyErr_Clear();
        }

        return true;
    }

    void Player::setSpriteClass(std::string className)
    {
        mFmtClassName = className;
        mFmtClassCode = className[0];
    }

    FARender::FASpriteGroup* Player::getCurrentAnim()
    {
        auto lastClassName = mFmtClassName;
        auto lastClassCode = mFmtClassCode;
        auto lastArmourCode = mFmtArmourCode;
        auto lastWeaponCode = mFmtWeaponCode;
        auto lastInDungeon = mFmtInDungeon;

        updateSpriteFormatVars();

        if( lastClassName   != mFmtClassName   ||
            lastClassCode   != mFmtClassCode   ||
            lastWeaponCode  != mFmtWeaponCode  ||
            lastArmourCode  != mFmtArmourCode  ||
            lastInDungeon   != mFmtInDungeon   )
        {
            auto helper = [&] (bool isDie = false)
            {
                std::string weapFormat = mFmtWeaponCode;

                if(isDie)
                    weapFormat = "n";

                boost::format fmt("plrgfx/%s/%s%s%s/%s%s%s%s.cl2");
                fmt % mFmtClassName % mFmtClassCode % mFmtArmourCode % weapFormat % mFmtClassCode % mFmtArmourCode % weapFormat;
                return fmt;
            };

            auto renderer = FARender::Renderer::get();

            mDieAnim = renderer->loadImage((helper(true) % "dt").str());
            mAttackAnim = renderer->loadImage((helper() % "at").str());

            if(mFmtInDungeon)
            {
                mWalkAnim = renderer->loadImage((helper() % "aw").str());
                mIdleAnim = renderer->loadImage((helper() % "as").str());
            }
            else
            {
                mWalkAnim = renderer->loadImage((helper() % "wl").str());
                mIdleAnim = renderer->loadImage((helper() % "st").str());
            }
        }

        return Actor::getCurrentAnim();
    }

    void Player::updateSpriteFormatVars()
    {
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
        }
        mFmtWeaponCode = weapon;
        mFmtArmourCode = armour;

        if(mLevel && mLevel->getLevelIndex() != 0)
            mFmtInDungeon=true;
        else
            mFmtInDungeon=false;
    }

    void Player::setLevel(GameLevel *level)
    {
        Actor::setLevel(level);
    }
}
