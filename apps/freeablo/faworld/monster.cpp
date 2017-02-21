#include "monster.h"

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
        mAnimTimeMap[AnimState::dead] = FAWorld::World::getTicksInPeriod(0.1f);
        mAnimTimeMap[AnimState::idle] = FAWorld::World::getTicksInPeriod(0.1f);
        mAnimTimeMap[AnimState::dead] = FAWorld::World::getTicksInPeriod(0.1f);
        mAnimTimeMap[AnimState::hit] = FAWorld::World::getTicksInPeriod(0.1f);

        mIsEnemy = true;
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
        mHitAnim =  FARender::Renderer::get()->loadImage((fmt % 'h').str());
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
