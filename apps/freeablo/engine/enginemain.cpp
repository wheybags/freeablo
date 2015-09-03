#include "enginemain.h"

#include <iostream>
#include <thread>
#include <chrono>

#include "../faworld/world.h"
#include "../falevelgen/levelgen.h"
#include "../falevelgen/random.h"
#include "../fagui/guimanager.h"
#include "../faaudio/audiomanager.h"
#include "../faworld/itemmanager.h"
#include "../faworld/characterstats.h"
#include "threadmanager.h"
#include "input.h"
#include "netmanager.h"


#include <misc/misc.h>
#include <input/inputmanager.h>

#include <enet/enet.h>

namespace bpo = boost::program_options;


namespace Engine
{
    volatile bool renderDone = false;

    EngineMain::~EngineMain()
    {
        if(mInputManager != NULL)
            delete mInputManager;
    }

    void EngineMain::run(const bpo::variables_map& variables)
    {
        Settings::Settings settings;
        if(!settings.loadUserSettings())
            return;

        size_t resolutionWidth = settings.get<size_t>("Display","resolutionWidth");
        size_t resolutionHeight = settings.get<size_t>("Display","resolutionHeight");
        bool fullscreen = settings.get<size_t>("Display", "fullscreen");

        Engine::ThreadManager threadManager;
        FARender::Renderer renderer(resolutionWidth, resolutionHeight, fullscreen);

        mInputManager = new EngineInputManager(*this);

        std::thread mainThread(boost::bind(&EngineMain::runGameLoop, this, &variables));

        threadManager.run();
        renderDone = true;

        mainThread.join();
    }

    void EngineMain::runGameLoop(const bpo::variables_map& variables)
    {
        FALevelGen::FAsrand(time(NULL));

        FARender::Renderer& renderer = *FARender::Renderer::get();
        Engine::ThreadManager& threadManager = *Engine::ThreadManager::get();
        std::string character = variables["character"].as<std::string>() ;
        DiabloExe::DiabloExe exe;

        if (!exe.isLoaded())
        {
            renderer.stop();
            return;
        }
        FAWorld::ItemManager itemManager;
        itemManager.loadItems(&exe);
        FAWorld::World world(exe);

        FAWorld::Player* player = world.getCurrentPlayer();
        world.generateLevels();
        FAWorld::ActorStats * stats;
        DiabloExe::CharacterStats char_stats = exe.getCharacterStat(character);



        if(character == "Warrior")
        {            
            stats = new FAWorld::MeleeStats(char_stats);
            FAWorld::Item item = itemManager.getBaseItem(125);

            player->mInventory.putItem(
                        item,
                        FAWorld::Item::eqLEFTHAND,
                        FAWorld::Item::eqFLOOR,
                        0, 0, 0, false);
            item = itemManager.getBaseItem(18);
            player->mInventory.putItem(
                        item,
                        FAWorld::Item::eqRIGHTHAND,
                        FAWorld::Item::eqFLOOR,
                        0, 0, 0, false);
            item = itemManager.getBaseItem(26);

            player->mInventory.putItem(
                        item,
                        FAWorld::Item::eqINV,
                        FAWorld::Item::eqFLOOR,
                        0, 0, 0, false);
            item = itemManager.getBaseItem(43);
            item.setCount(100);
            player->mInventory.putItem(item,
                                       FAWorld::Item::eqINV,
                                       FAWorld::Item::eqFLOOR,
                                       3, 0, 0, false);
            item = itemManager.getBaseItem(79);

            player->mInventory.putItem(item,
                                       FAWorld::Item::eqBELT,
                                       FAWorld::Item::eqFLOOR,
                                       0, 0, 0, false);

            item = itemManager.getBaseItem(79);

            player->mInventory.putItem(item,
                                       FAWorld::Item::eqBELT,
                                       FAWorld::Item::eqFLOOR,
                                       0, 0, 1, false);
            player->setSpriteClass("warrior");
            player->setIdleAnimation("plrgfx/warrior/wld/wldst.cl2");
            player->setWalkAnimation("plrgfx/warrior/wld/wldwl.cl2");

        }

        else if(character == "Rogue")
        {
            stats = new FAWorld::RangerStats(char_stats);
            FAWorld::Item item = itemManager.getBaseItem(121);
            player->mInventory.putItem(item,
                                       FAWorld::Item::eqLEFTHAND,
                                       FAWorld::Item::eqFLOOR,
                                       0, 0, 0, false);
            item = itemManager.getBaseItem(43);
            item.setCount(100);
            player->mInventory.putItem(item,
                                       FAWorld::Item::eqINV,
                                       FAWorld::Item::eqFLOOR,
                                       3, 0, 0,false);
            item = itemManager.getBaseItem(79);
            player->mInventory.putItem(item,
                                       FAWorld::Item::eqBELT,
                                       FAWorld::Item::eqFLOOR,
                                       0, 0, 0, false);
            item = itemManager.getBaseItem(79);
            player->mInventory.putItem(item,
                                       FAWorld::Item::eqBELT,
                                       FAWorld::Item::eqFLOOR,
                                       0, 0, 1, false);
            player->setSpriteClass("rogue");
            player->setIdleAnimation("plrgfx/rogue/rlb/rlbst.cl2");
            player->setWalkAnimation("plrgfx/rogue/rlb/rlbwl.cl2");

        }
        else
        {
            stats = new FAWorld::MageStats(char_stats);
            FAWorld::Item item = itemManager.getBaseItem(124);
            player->mInventory.putItem(item,
                                       FAWorld::Item::eqLEFTHAND,
                                       FAWorld::Item::eqFLOOR,
                                       0, 0, 0, false);
            item = itemManager.getBaseItem(43);
            item.setCount(100);
            player->mInventory.putItem(item,
                                       FAWorld::Item::eqINV,
                                       FAWorld::Item::eqFLOOR,
                                       3, 0, 0, false);
            item = itemManager.getBaseItem(81);
            player->mInventory.putItem(item,
                                       FAWorld::Item::eqBELT,
                                       FAWorld::Item::eqFLOOR,
                                       0, 0, 0, false);
            item = itemManager.getBaseItem(81);
            player->mInventory.putItem(item,
                                       FAWorld::Item::eqBELT,
                                       FAWorld::Item::eqBELT,
                                       0, 0, 1, false);
            player->setSpriteClass("sorceror");
            player->setIdleAnimation("plrgfx/sorceror/slt/sltst.cl2");
            player->setWalkAnimation("plrgfx/sorceror/slt/sltwl.cl2");
        }


        world.setStatsObject(stats);        
        stats->setActor(player);
        stats->recalculateDerivedStats();
        player->mInventory.collectEffects();

        int32_t currentLevel = variables["level"].as<int32_t>();


        FAGui::GuiManager guiManager(player->mInventory, *this, character);

        // -1 represents the main menu
        if(currentLevel != -1)
        {
            world.setLevel(currentLevel);
            Level::Level& level = *world.getCurrentLevel();            
            player->mPos = FAWorld::Position(level.upStairsPos().first, level.upStairsPos().second);
            guiManager.showIngameGui();
        }
        else
        {
            pause();
            guiManager.showMainMenu();
            threadManager.playMusic("music/dintro.wav");
        }

        auto last = std::chrono::system_clock::now();

        bool isServer = variables["mode"].as<std::string>() == "server";

        NetManager netManager(isServer);

        // Main game logic loop
        while(!mDone)
        {
            auto now = std::chrono::system_clock::now();

            while(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch() - last.time_since_epoch()).count() < (int64_t)(1000/FAWorld::World::ticksPerSecond))
            {
                std::this_thread::yield();
                now = std::chrono::system_clock::now();
            }


            last = now;

            mInputManager->update(mPaused);            
            if(!mPaused)
                world.update(mNoclip);
            netManager.update();
            guiManager.updateGui();

            Level::Level* level = world.getCurrentLevel();
            FARender::RenderState* state = renderer.getFreeState();
            if(state)
            {
                state->mPos = player->mPos;
                if(level != NULL)
                    state->tileset = renderer.getTileset(*level);
                state->level = level;
                if(!FAGui::cursorPath.empty())
                    state->mCursorEmpty = false;
                else
                    state->mCursorEmpty = true;                
                state->mCursorFrame = FAGui::cursorFrame;
                state->mCursorSpriteGroup = renderer.loadImage("data/inv/objcurs.cel");                
                world.fillRenderState(state);                
                Render::updateGuiBuffer(&state->guiDrawBuffer);                
            }
            else
            {
                Render::updateGuiBuffer(NULL);

            }
            renderer.setCurrentState(state);
        }

        renderer.stop();
        renderer.waitUntilDone();
    }

    void EngineMain::stop()
    {
        mDone = true;
    }

    void EngineMain::pause()
    {
        mPaused = true;
    }

    void EngineMain::unPause()
    {
        mPaused = false;
    }

    void EngineMain::toggleNoclip()
    {
        mNoclip = !mNoclip;
    }
}
