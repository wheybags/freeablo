#include "player.h"
#include "world.h"
#include "actorstats.h"
#include "../falevelgen/random.h"
#include "../engine/threadmanager.h"
#include <random>

namespace FAWorld
{
    Player::Player() : Actor(), mInventory(this)
    {
        mAnimTimeMap[AnimState::dead] = 10;
        mAnimTimeMap[AnimState::walk] = 10;
        mAnimTimeMap[AnimState::attack] = 16;
        mAnimTimeMap[AnimState::idle] = 10;


    }
    bool Player::attack(Monster *enemy)
    {
        if(!(World::get()->getTicksPassed() % (size_t)(World::get()->secondsToTicks(mStats->getAttackSpeed() * 0.05))))
        {
            if(enemy->isDead() && enemy->mStats != nullptr)
                return false;
            isAttacking = true;
            Engine::ThreadManager::get()->playSound(FALevelGen::chooseOne({"sfx/misc/swing2.wav", "sfx/misc/swing.wav"}));
            setAnimation(AnimState::attack, true);
            mAnimPlaying = true;
            if(FALevelGen::percentageChance(mStats->getChanceToHitMelee()))
            {
                //Critical hit chance, %clvl. Do 200% damage
                if(FALevelGen::percentageChance(mStats->getLevel()))
                    enemy->takeDamage(mStats->getMeleeDamage()*2);
                else
                    enemy->takeDamage(mStats->getMeleeDamage());
                if(enemy->getCurrentHP() <= 0)
                    enemy->die();
                return true;
            }
            else
                return false;
        }
        else
            return false;
    }

    bool Player::attack(Player *enemy)
    {
        UNUSED_PARAM(enemy);
        return false;
    }

    void Player::setSpriteClass(std::string className)
    {
        mClassName = className;
        mClassCode = className[0];
    }

    FARender::FASpriteGroup Player::getCurrentAnim()
    {
        updateSprite();
        boost::format fmt("plrgfx/%s/%s%s%s/%s%s%s%s.cl2");
        fmt % mClassName % mClassCode % mArmourCode % mWeaponCode % mClassCode % mArmourCode % mWeaponCode;
        switch(mAnimState)
        {
            case AnimState::dead:
            {
                mWeaponCode = "n";
                fmt % "dt";
                break;
            }
            case AnimState::walk:
            {
                if (mInDungeon)
                    fmt % "aw";
                else
                    fmt % "wl";
                break;
            }
            case AnimState::attack:
            {
                fmt % "at";
                break;
            }

            default:
            case AnimState::idle:
            {
                if (mInDungeon)
                    fmt % "as";
                else
                    fmt % "st";
                break;
            }
        }
        return FARender::Renderer::get()->loadImage(fmt.str());
    }
    void Player::updateSprite()
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
        mWeaponCode = weapon;
        mArmourCode = armour;
    }
}
