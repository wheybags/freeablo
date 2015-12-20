#include "player.h"
#include "world.h"
#include "actorstats.h"
#include "../falevelgen/random.h"
#include "../engine/threadmanager.h"

namespace FAWorld
{
    Player::Player() : Actor(), mInventory(this)
    {
        mAnimTimeMap[AnimState::dead] = 10;
        mAnimTimeMap[AnimState::walk] = 10;
        mAnimTimeMap[AnimState::attack] = 16;
        mAnimTimeMap[AnimState::idle] = 10;


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

    void Player::setSpriteClass(std::string className)
    {
        mFmtClassName = className;
        mFmtClassCode = className[0];
    }

    FARender::FASpriteGroup Player::getCurrentAnim()
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
