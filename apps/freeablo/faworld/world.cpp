#include "world.h"

#include <diabloexe/diabloexe.h>

#include <boost/tuple/tuple.hpp>

#include "../farender/renderer.h"

#include "monster.h"

//Andrettin: added the includes below to be able to read the NPCs.ini file
#include <stdint.h>

#include <iomanip>
#include <sstream>
#include <iostream>

#include <misc/fareadini.h>
#include <misc/md5.h>
#include <misc/stringops.h>

namespace FAWorld
{
    //Andrettin: added this here as part of the process of reading the NPCs.ini file
    namespace bpt = boost::property_tree;

    World::World()
    {
        mPlayer = new Player();
        mActors.push_back(mPlayer);
        mTicksSinceLastAnimUpdate = 0;
    }

    World::~World()
    {
        for(size_t i = 0; i < mActors.size(); i++)
            delete mActors[i];
    }

    void World::setLevel(const Level::Level& level, const DiabloExe::DiabloExe& exe)
    {
        const std::vector<Level::Monster>& monsters = level.getMonsters();

        for(size_t i = 0; i < monsters.size(); i++)
            mActors.push_back(new Monster(exe.getMonster(monsters[i].name), Position(monsters[i].xPos, monsters[i].yPos)));
    }

    void World::addNpcs(const DiabloExe::DiabloExe& exe)
    {
	    const std::vector<const DiabloExe::Npc*> npcs = exe.getNpcs();

	    for(size_t i = 0; i < npcs.size(); i++)
	    {
		    mActors.push_back(new Actor(npcs[i]->celPath, npcs[i]->celPath, Position(npcs[i]->x, npcs[i]->y, npcs[i]->rotation)));
	    }

    	bpt::ptree pt;
	    Misc::readIni("resources/NPCs.ini", pt);
	    for(bpt::ptree::const_iterator it = pt.begin(); it != pt.end(); ++it)
	    {
		    if(Misc::StringUtils::startsWith(it->first, "NPC"))
			{
				mActors.push_back(new Actor(it->second.get<std::string>("cel"), it->second.get<std::string>("cel"), Position(it->second.get<int>("x"), it->second.get<int>("y"), 0)));
			}
		}
    }

    void World::clear()
    {
        for(size_t i = 0; i < mActors.size(); i++)
        {
            if(mActors[i] != mPlayer)
                delete mActors[i];
        }

        mActors.clear();
        mActors.push_back(mPlayer);
    }

    void World::update()
    {
        mTicksSinceLastAnimUpdate++;

        bool advanceAnims = mTicksSinceLastAnimUpdate >= (float)ticksPerSecond*0.1;

        if(advanceAnims)
            mTicksSinceLastAnimUpdate = 0;

        for(size_t i = 0; i < mActors.size(); i++)
        {
            mActors[i]->update();

            if(advanceAnims)
                mActors[i]->mFrame = (mActors[i]->mFrame + 1) % mActors[i]->getCurrentAnim().get()->mSpriteGroup.animLength();
        }
    }

    Player* World::getPlayer()
    {
        return mPlayer;
    }

    void World::fillRenderState(FARender::RenderState* state)
    {
        state->mObjects.clear();

        for(size_t i = 0; i < mActors.size(); i++)
        {
            size_t frame = mActors[i]->mFrame + mActors[i]->mPos.mDirection * mActors[i]->getCurrentAnim().get()->mSpriteGroup.animLength();
            state->mObjects.push_back(boost::tuple<FARender::FASpriteGroup, size_t, FAWorld::Position>(mActors[i]->getCurrentAnim(), frame, mActors[i]->mPos));
        }
    }
}