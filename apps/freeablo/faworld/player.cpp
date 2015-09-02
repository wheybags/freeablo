#include "player.h"
#include "world.h"
#include "actorstats.h"
#include "../falevelgen/random.h"
#include "../engine/threadmanager.h"

namespace FAWorld
{
    Player::Player() : Actor()
    {

    }
    bool Player::attack(Actor *enemy)
    {
        mStats->printStats();
        if(enemy->isDead())
            return false;
        isAttacking = true;        
        Engine::ThreadManager::get()->playSound(FALevelGen::chooseOne({"sfx/misc/swing2.wav", "sfx/misc/swing.wav"}));
        enemy->takeDamage((uint32_t)mStats->getMeleeDamage());
        if(enemy->getCurrentHP() <= 0)
            enemy->die();
        else
            printf("HP: %u\n",  enemy->getCurrentHP());
        setAnimation(AnimState::meleeAttack, true);
        mAnimPlaying = true;
        mAnimStep = 2.25;
        return true;
    }

    void Player::setSpriteClass(std::string className)
    {
        mActorSpriteState.setClass(className);
    }

    FARender::FASpriteGroup Player::getCurrentAnim()
    {
        return FARender::Renderer::get()->loadImage(mActorSpriteState.getAnimPath(mAnimState));
    }
}
