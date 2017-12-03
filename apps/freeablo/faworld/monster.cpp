#include "monster.h"
#include "behaviour.h"

#include <diabloexe/monster.h>

#include "../engine/threadmanager.h"
#include "../falevelgen/random.h"
#include "../fasavegame/gameloader.h"

#include "actorstats.h"
#include "world.h"

#include <boost/format.hpp>

namespace FAWorld
{
    const std::string Monster::typeId = "monster";

    void Monster::init() { mFaction = Faction::hell(); }

    Monster::Monster()
    {
        init();

        DiabloExe::Monster dMonster; // TODO: hack
        // mStats = new FAWorld::ActorStats(dMonster);
    }

    Monster::Monster(const DiabloExe::Monster& monster) : Actor("", "", ""), mSoundPath(monster.soundPath)
    {
        init();

        // mStats = new FAWorld::ActorStats(monster);

        boost::format fmt(monster.cl2Path);
        getAnimationManager().setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage((fmt % 'w').str()));
        getAnimationManager().setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage((fmt % 'n').str()));
        getAnimationManager().setAnimation(AnimState::dead, FARender::Renderer::get()->loadImage((fmt % 'd').str()));
        getAnimationManager().setAnimation(AnimState::attack, FARender::Renderer::get()->loadImage((fmt % 'a').str()));
        getAnimationManager().setAnimation(AnimState::hit, FARender::Renderer::get()->loadImage((fmt % 'h').str()));
    }

    Monster::Monster(FASaveGame::GameLoader& loader) : Actor(loader) { mSoundPath = loader.load<std::string>(); }

    void Monster::save(FASaveGame::GameSaver& saver)
    {
        Serial::ScopedCategorySaver cat("Monster", saver);

        Actor::save(saver);
        saver.save(mSoundPath);
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
        if (mSoundPath.empty())
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
