#include <iostream>
#include <thread>
#include <functional>
#include <boost/asio.hpp>
#include <misc/misc.h>
#include <input/inputmanager.h>
#include <enet/enet.h>
#include "../faworld/world.h"
#include "../faworld/player.h"
#include "../falevelgen/levelgen.h"
#include "../falevelgen/random.h"
#include "../fagui/guimanager.h"
#include "../faaudio/audiomanager.h"
#include "../faworld/itemmanager.h"
#include "../faworld/characterstats.h"
#include "../faworld/playerfactory.h"
#include "threadmanager.h"
#include "net/netmanager.h"
#include "enginemain.h"

namespace bpo = boost::program_options;


namespace Engine
{
    volatile bool renderDone = false;

    EngineInputManager& EngineMain::inputManager()
    {
        return *(mInputManager.get());
    }

    void EngineMain::run(const bpo::variables_map& variables)
    {
        Settings::Settings settings;
        if(!settings.loadUserSettings())
            return;

        size_t resolutionWidth = settings.get<size_t>("Display","resolutionWidth");
        size_t resolutionHeight = settings.get<size_t>("Display","resolutionHeight");
        std::string fullscreen = settings.get<std::string>("Display", "fullscreen");
        std::string pathEXE = settings.get<std::string>("Game", "PathEXE");
        if (pathEXE == "")
        {
            pathEXE = "Diablo.exe";
        }

        Engine::ThreadManager threadManager;
        FARender::Renderer renderer(resolutionWidth, resolutionHeight, fullscreen == "true");
        mInputManager = std::make_shared<EngineInputManager>();
        mInputManager->registerKeyboardObserver(this);
        std::thread mainThread(std::bind(&EngineMain::runGameLoop, this, &variables, pathEXE));
        threadManager.run();
        renderDone = true;

        mainThread.join();
    }

    void EngineMain::runGameLoop(const bpo::variables_map& variables, const std::string& pathEXE)
    {
        FALevelGen::FAsrand(static_cast<int> (time(nullptr)));

        FAWorld::Player* player;
        FARender::Renderer& renderer = *FARender::Renderer::get();

        Settings::Settings settings;
        if(!settings.loadUserSettings())
            return;

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

        bool isServer = variables["mode"].as<std::string>() == "server";
        if(isServer)
            world.generateLevels();

        itemManager.loadItems(&exe);
        player = playerFactory.create(characterClass);
        world.addCurrentPlayer(player);
        world.generateLevels();
        mInputManager->registerKeyboardObserver(&world);
        mInputManager->registerMouseObserver(&world);

        int32_t currentLevel = variables["level"].as<int32_t>();

        FAGui::GuiManager guiManager(player->mInventory, *this, characterClass);

        // -1 represents the main menu
        if(currentLevel != -1 && isServer)
        {
            world.setLevel(currentLevel);

            FAWorld::GameLevel& level = *world.getCurrentLevel();

            player->mPos = FAWorld::Position(level.upStairsPos().first, level.upStairsPos().second);
            guiManager.showIngameGui();
        }
        else
        {
            pause();
            bool showTitleScreen = settings.get<bool>("Game", "showTitleScreen");
            if(showTitleScreen)
            {
                guiManager.showTitleScreen();
            }
            else
            {
                guiManager.showMainMenu();
            }
        }

        boost::asio::io_service io;

        NetManager netManager(isServer, playerFactory);

        // Main game logic loop
        while(!mDone)
        {
            boost::asio::deadline_timer timer(io, boost::posix_time::milliseconds(1000/FAWorld::World::ticksPerSecond));

            mInputManager->update(mPaused);
            if(!mPaused)
            {
                world.update(mNoclip);
            }

            netManager.update();
            guiManager.update(mPaused);

            FAWorld::GameLevel* level = world.getCurrentLevel();
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
                world.fillRenderState(state, *player);
                Render::updateGuiBuffer(&state->guiDrawBuffer);
            }
            else
            {
                Render::updateGuiBuffer(NULL);
            }
            renderer.setCurrentState(state);

            auto remainingTickTime = timer.expires_from_now().total_milliseconds();

            if(remainingTickTime < 0)
                std::cerr << "tick time exceeded by " << -remainingTickTime << "ms" << std::endl;

            timer.wait();
        }

        renderer.stop();
        renderer.waitUntilDone();
    }

    void EngineMain::notify(KeyboardInputAction action)
    {
        if(action == QUIT)
        {
            stop();
        }
        else if(action == NOCLIP)
        {
            toggleNoclip();
        }
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
