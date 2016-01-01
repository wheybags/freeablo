#include "monster.h"

#include <diabloexe/monster.h>

#include "../falevelgen/random.h"
#include "../engine/threadmanager.h"
#include <boost/format.hpp>

namespace FAWorld
{
    STATIC_HANDLE_NET_OBJECT(Monster)

    void Monster::init()
    {
        mAnimTimeMap[AnimState::dead] = 10;
        mAnimTimeMap[AnimState::idle] = 10;
        mAnimTimeMap[AnimState::dead] = 10;
        mAnimTimeMap[AnimState::hit] = 10;

        mIsEnemy = true;
    }

    Monster::Monster()
    {
        init();
    }

    Monster::Monster(const DiabloExe::Monster& monster, Position pos, ActorStats *stats):
        Actor("", "", pos, "", stats), mAnimPath(monster.cl2Path), mSoundPath(monster.soundPath)
    {
        init();

        boost::format fmt(mAnimPath); 
        mWalkAnim = FARender::Renderer::get()->loadImage((fmt % 'w').str());
        mIdleAnim = FARender::Renderer::get()->loadImage((fmt % 'n').str());
        mDieAnim =  FARender::Renderer::get()->loadImage((fmt % 'd').str());
        mHitAnim =  FARender::Renderer::get()->loadImage((fmt % 'h').str());
    }

    std::string Monster::getDieWav()
    {
        boost::format fmt(mSoundPath);
        fmt % 'd';
        return (fmt % FALevelGen::randomInRange(1, 2)).str();

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
