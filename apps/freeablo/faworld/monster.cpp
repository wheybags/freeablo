#include "monster.h"
#include "behaviour.h"

#include <diabloexe/monster.h>

#include "../falevelgen/random.h"
#include "../engine/threadmanager.h"

#include "world.h"
#include "actorstats.h"

#include <boost/format.hpp>

namespace FAWorld
{
    STATIC_HANDLE_NET_OBJECT_IN_IMPL(Monster)

    void Monster::init()
    {
        mAnimTimeMap[AnimState::dead] = FAWorld::World::getTicksInPeriod(0.5f);
        mAnimTimeMap[AnimState::idle] = FAWorld::World::getTicksInPeriod(0.5f);
        mAnimTimeMap[AnimState::dead] = FAWorld::World::getTicksInPeriod(0.5f);
        mAnimTimeMap[AnimState::attack] = FAWorld::World::getTicksInPeriod(1.0f);
        mAnimTimeMap[AnimState::hit] = FAWorld::World::getTicksInPeriod(0.5f);

        mFaction = Faction::hell();
    }

    bool Monster::attack(Actor *enemy)
    {
        if(enemy->isDead() && enemy->mStats != nullptr)
            return false;
        isAttacking = true;
        Engine::ThreadManager::get()->playSound(FALevelGen::chooseOne({"sfx/misc/swing2.wav", "sfx/misc/swing.wav"}));
        playAnimation(AnimState::attack, AnimationPlayer::AnimationType::Once);
        return true;
    }

    bool Monster::canIAttack(Actor * actor)
    {
        if (actor == nullptr)
            return false;

        if (this == actor)
            return false;

        if (!isEnemy(actor))
            return false;

        if (actor->isDead())
            return false;

        if (mPos.distanceFrom(actor->mPos) >= 2)
            return false;

        if (isAttacking)
            return false;

        return true;
    }

    Monster::Monster()
    {
        init();

        DiabloExe::Monster dMonster; //TODO: hack
        mStats = new FAWorld::ActorStats(dMonster);
    }

    Monster::Monster(const DiabloExe::Monster& monster, Position pos):
        Actor("", "", pos, ""), mSoundPath(monster.soundPath)
    {
        init();

        mStats = new FAWorld::ActorStats(monster);

        boost::format fmt(monster.cl2Path);
        mWalkAnim = FARender::Renderer::get()->loadImage((fmt % 'w').str());
        mIdleAnim = FARender::Renderer::get()->loadImage((fmt % 'n').str());
        mDieAnim =  FARender::Renderer::get()->loadImage((fmt % 'd').str());
        mAttackAnim =  FARender::Renderer::get()->loadImage((fmt % 'a').str());
        mHitAnim =  FARender::Renderer::get()->loadImage((fmt % 'h').str());

        setIdleAnimation((fmt % 'n').str());
    }

    std::string Monster::getDieWav()
    {
        if (mSoundPath.empty())
        {
            printf("No sound for caller\n");
            return "";
        }
        else
        {
            boost::format fmt(mSoundPath);
            fmt % 'd';
            return (fmt % FALevelGen::randomInRange(1, 2)).str();
        }
    }

    std::string Monster::getHitWav()
    {
        if(mSoundPath.empty())
        {
            printf("No sound for caller\n");
            return "";
        }
        else
        {
            boost::format fmt(mSoundPath);
            fmt % 'h';
            return (fmt % FALevelGen::randomInRange(1, 2)).str();
        }
    }
}
