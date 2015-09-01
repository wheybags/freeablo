#include "player.h"
#include "world.h"
#include "actorstats.h"
#include "../falevelgen/random.h"
#include "../engine/threadmanager.h"
namespace FAWorld
{

    bool Player::attack(Actor *enemy)
    {
        if(enemy->isDead())
            return false;
        isAttacking = true;
        printf("dist  = %f\n", mPos.distanceFrom(enemy->mPos));
        printf("attackSpeed = %u\n", mStats->getAttackSpeed());
        printf("ticks = %f\n", World::get()->ticksPerSecond * mStats->getAttackSpeed() * 0.05);
        printf("??? = %f\n",World::get()->ticksPerSecond * (mStats->getAttackSpeed() * 0.05));
        //setAnimation(AnimState::attackMelee);
        Engine::ThreadManager::get()->playSound(FALevelGen::chooseOne({"sfx/misc/swing2.wav", "sfx/misc/swing.wav"}));
        enemy->takeDamage(mStats->getMeleeDamage());
        if(enemy->getCurrentHP() == 0)
            enemy->die();
        return true;



    }




}
