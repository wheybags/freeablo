#include "monster.h"

#include <diabloexe/monster.h>

#include "../falevelgen/random.h"
#include "../engine/threadmanager.h"
#include <boost/format.hpp>

namespace FAWorld
{
    Monster::Monster(const DiabloExe::Monster& monster, Position pos, ActorStats *stats):
        Actor("", "", pos, ""), mAnimPath(monster.cl2Path), mSoundPath(monster.soundPath)
    {
        mAnimTimeMap[AnimState::dead] = 10;
        mAnimTimeMap[AnimState::idle] = 10;
        mAnimTimeMap[AnimState::dead] = 10;
        mAnimTimeMap[AnimState::hit] = 10;
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

    void Monster::takeDamage(double amount)
    {

    }

    FARender::FASpriteGroup Monster::getCurrentAnim()
    {
        boost::format fmt(mAnimPath);
        switch(mAnimState)
        {
            case AnimState::walk:
                return FARender::Renderer::get()->loadImage((fmt % 'w').str());

            case AnimState::idle:
                return FARender::Renderer::get()->loadImage((fmt % 'n').str());

            case AnimState::dead:
                return FARender::Renderer::get()->loadImage((fmt % 'd').str());

            case AnimState::hit:
                return FARender::Renderer::get()->loadImage((fmt % 'h').str());

            default:
                return FARender::Renderer::get()->loadImage((fmt % 'n').str());
        }



    }


    int32_t Monster::getCurrentHP()
    {
        return 0;
    }
}
