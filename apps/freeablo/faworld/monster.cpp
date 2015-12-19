#include "monster.h"

#include <diabloexe/monster.h>

#include "../falevelgen/random.h"
#include "../engine/threadmanager.h"
#include <boost/format.hpp>

namespace FAWorld
{
    Monster::Monster(const DiabloExe::Monster& monster, Position pos, ActorStats *stats):
        Actor("", "", pos, "", stats), mAnimPath(monster.cl2Path), mSoundPath(monster.soundPath)
    {
        mAnimTimeMap[AnimState::dead] = 10;
        mAnimTimeMap[AnimState::idle] = 10;
        mAnimTimeMap[AnimState::dead] = 10;
        mAnimTimeMap[AnimState::hit] = 10;
        
        boost::format fmt(mAnimPath); 
        mWalkAnim = FARender::Renderer::get()->loadImage((fmt % 'w').str());
        mIdleAnim = FARender::Renderer::get()->loadImage((fmt % 'n').str());
        mDieAnim =  FARender::Renderer::get()->loadImage((fmt % 'd').str());
        mHitAnim =  FARender::Renderer::get()->loadImage((fmt % 'h').str());

        mIsEnemy = true;
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
