#include "monster.h"

#include <diabloexe/monster.h>

#include "../falevelgen/random.h"
#include "../engine/threadmanager.h"
#include <boost/format.hpp>

namespace FAWorld
{
    Monster::Monster(const DiabloExe::Monster& monster, Position pos, ActorStats *stats):
        Actor(getWalkCl2(monster), getIdleCl2(monster), pos, getDieCl2(monster), stats, monster.soundPath), mAnimPath(monster.cl2Path)
    {}

    std::string Monster::getWalkCl2(const DiabloExe::Monster& monster)
    {
        boost::format fmt(monster.cl2Path);
        fmt % 'w';

        return fmt.str();
    }
    
    std::string Monster::getIdleCl2(const DiabloExe::Monster& monster)
    {
        boost::format fmt(monster.cl2Path);
        fmt % 'n';

        return fmt.str();
    }


    std::string Monster::getDieCl2(const DiabloExe::Monster & monster)
    {
        boost::format fmt(monster.cl2Path);
        fmt % 'd';

        return fmt.str();

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
}
