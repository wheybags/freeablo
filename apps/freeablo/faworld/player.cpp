#include "player.h"
#include "world.h"
#include "actorstats.h"
#include "../falevelgen/random.h"
#include "../engine/threadmanager.h"

namespace FAWorld
{
    Player::Player() : Actor("plrgfx/warrior/wls/wlswl.cl2", "plrgfx/warrior/wls/wlsst.cl2", Position(0,0))
    {

    }
    bool Player::attack(Actor *enemy)
    {
        if(enemy->isDead())
            return false;
        isAttacking = true;
        //setAnimation(AnimState::attackMelee);
        Engine::ThreadManager::get()->playSound(FALevelGen::chooseOne({"sfx/misc/swing2.wav", "sfx/misc/swing.wav"}));
        enemy->takeDamage(mStats->getMeleeDamage());
        if(enemy->getCurrentHP() == 0)
            enemy->die();
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
