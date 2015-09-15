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
#include "../faworld/playerfactory.h"
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
        std::string pathEXE = settings.get<std::string>("Game", "PathEXE");
        if (pathEXE == "")
        {
            pathEXE = "Diablo.exe";
        }

        Engine::ThreadManager threadManager;
        FARender::Renderer renderer(resolutionWidth, resolutionHeight, fullscreen);

        mInputManager = new EngineInputManager(*this);

        std::thread mainThread(boost::bind(&EngineMain::runGameLoop, this, &variables, pathEXE));

        threadManager.run();
        renderDone = true;

        mainThread.join();
    }

    void EngineMain::runGameLoop(const bpo::variables_map& variables, const std::string& pathEXE)
    {
        FALevelGen::FAsrand(time(NULL));

        FAWorld::Player* player;
        FARender::Renderer& renderer = *FARender::Renderer::get();
        Engine::ThreadManager& threadManager = *Engine::ThreadManager::get();      

        std::string characterClass = variables["character"].as<std::string>();

        DiabloExe::DiabloExe exe(pathEXE);
        if (!exe.isLoaded())
        {
            renderer.stop();
            return;
        }

        FAWorld::ItemManager& itemManager = FAWorld::ItemManager::get();
        FAWorld::World world(exe);
        FAWorld::PlayerFactory playerFactory(exe);

        itemManager.loadItems(&exe);
        player = playerFactory.create(characterClass);
        world.addCurrentPlayer(player);
        world.generateLevels();

        int32_t currentLevel = variables["level"].as<int32_t>();

        FAGui::GuiManager guiManager(player->mInventory, *this, characterClass);

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
