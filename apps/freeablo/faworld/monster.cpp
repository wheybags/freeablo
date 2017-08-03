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
        mFaction = Faction::hell();
    }

    Monster::Monster()
    {
        init();

        DiabloExe::Monster dMonster; //TODO: hack
        mStats = new FAWorld::ActorStats(dMonster);
    }

    Monster::Monster(const DiabloExe::Monster& monster):
        Actor("", "", ""), mSoundPath(monster.soundPath)
    {
        init();

        mStats = new FAWorld::ActorStats(monster);

        boost::format fmt(monster.cl2Path);
        getAnimationManager().setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage((fmt % 'w').str()));
        getAnimationManager().setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage((fmt % 'n').str()));
        getAnimationManager().setAnimation(AnimState::dead, FARender::Renderer::get()->loadImage((fmt % 'd').str()));
        getAnimationManager().setAnimation(AnimState::attack, FARender::Renderer::get()->loadImage((fmt % 'a').str()));
        getAnimationManager().setAnimation(AnimState::hit, FARender::Renderer::get()->loadImage((fmt % 'h').str()));
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
