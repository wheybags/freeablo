#include "enginemain.h"
#include "../faaudio/audiomanager.h"
#include "../fagui/guimanager.h"
#include "../falevelgen/levelgen.h"
#include "../falevelgen/random.h"
#include "../fasavegame/gameloader.h"
#include "../faworld/itemmanager.h"
#include "../faworld/player.h"
#include "../faworld/playerbehaviour.h"
#include "../faworld/playerfactory.h"
#include "../faworld/world.h"
#include "threadmanager.h"
#include <boost/asio.hpp>
#include <boost/make_unique.hpp>
#include <enet/enet.h>
#include <functional>
#include <input/inputmanager.h>
#include <iostream>
#include <misc/misc.h>
#include <serial/textstream.h>
#include <thread>

namespace bpo = boost::program_options;

namespace Engine
{
    volatile bool renderDone = false;

    EngineMain::EngineMain() {}

    EngineMain::~EngineMain() {}

    EngineInputManager& EngineMain::inputManager() { return *(mInputManager.get()); }

    void EngineMain::run(const bpo::variables_map& variables)
    {
        Settings::Settings settings;
        if (!settings.loadUserSettings())
            return;

        size_t resolutionWidth = settings.get<size_t>("Display", "resolutionWidth");
        size_t resolutionHeight = settings.get<size_t>("Display", "resolutionHeight");
        std::string fullscreen = settings.get<std::string>("Display", "fullscreen");
        std::string pathEXE = settings.get<std::string>("Game", "PathEXE");
        if (pathEXE == "")
        {
            pathEXE = "Diablo.exe";
        }

        Engine::ThreadManager threadManager;
        FARender::Renderer renderer(resolutionWidth, resolutionHeight, fullscreen == "true");
        mInputManager = std::make_shared<EngineInputManager>(renderer.getNuklearContext());
        mInputManager->registerKeyboardObserver(this);
        std::thread mainThread(std::bind(&EngineMain::runGameLoop, this, &variables, pathEXE));
        threadManager.run();
        renderDone = true;

        mainThread.join();
    }

    void EngineMain::runGameLoop(const bpo::variables_map& variables, const std::string& pathEXE)
    {
        FALevelGen::FAsrand(static_cast<int>(time(nullptr)));

        FARender::Renderer& renderer = *FARender::Renderer::get();

        Settings::Settings settings;
        if (!settings.loadUserSettings())
            return;

        std::string characterClass = variables["character"].as<std::string>();

        mExe = boost::make_unique<DiabloExe::DiabloExe>(pathEXE);
        if (!mExe->isLoaded())
        {
            renderer.stop();
            return;
        }

        FAWorld::ItemManager& itemManager = FAWorld::ItemManager::get();
        mPlayerFactory = boost::make_unique<FAWorld::PlayerFactory>(*mExe);
        renderer.loadFonts(*mExe);

        FILE* f = fopen("save.sav", "rb");
        mGuiManager = boost::make_unique<FAGui::GuiManager>(*this);
        mInputManager->registerKeyboardObserver(mGuiManager.get());
        mInputManager->setGuiManager(mGuiManager.get());

        if (f)
        {
            fseek(f, 0, SEEK_END);
            size_t size = ftell(f);
            fseek(f, 0, SEEK_SET);

            std::string tmp;
            tmp.resize(size);

            fread((void*)tmp.data(), 1, size, f);

            Serial::TextReadStream stream(tmp);
            FASaveGame::GameLoader loader(stream);

            mWorld.reset(new FAWorld::World(loader, *mExe));
            mWorld->setGuiManager(mGuiManager.get());

            mPlayer = mWorld->getCurrentPlayer();
            setupNewPlayer(mPlayer);
            inGame = true;
        }
        else
        {
            mWorld.reset(new FAWorld::World(*mExe));
            mWorld->setGuiManager(mGuiManager.get());

            itemManager.loadItems(mExe.get());

            int32_t currentLevel = variables["level"].as<int32_t>();
            mWorld->generateLevels(); // TODO: not generate levels while game hasn't started

            if (currentLevel != -1)
            {
                inGame = true;
                setupNewPlayer(mPlayerFactory->create(characterClass));
                mWorld->setLevel(currentLevel);
                if (variables["invuln"].as<std::string>() == "on")
                    mPlayer->mInvuln = true;
            }
        }
        if (inGame)
        {
            mInputManager->registerKeyboardObserver(mWorld.get());
            // mInputManager->registerMouseObserver(mWorld.get());
        }

        boost::asio::io_service io;

        // Main game logic loop
        while (!mDone)
        {
            boost::asio::deadline_timer timer(io, boost::posix_time::milliseconds(1000 / FAWorld::World::ticksPerSecond));

            mInputManager->update(mPaused);
            if (!mPaused && inGame)
                mWorld->update(mNoclip);

            nk_context* ctx = renderer.getNuklearContext();
            mGuiManager->update(inGame, mPaused, ctx);

            FARender::RenderState* state = renderer.getFreeState();
            if (state)
            {
                if (mPlayer)
                {
                    auto level = mWorld->getCurrentLevel();
                    state->mPos = mPlayer->getPos();
                    if (level != NULL)
                        state->tileset = renderer.getTileset(*level);
                    state->level = level;
                    mWorld->fillRenderState(state);
                }
                else
                    state->level = nullptr;
                if (!FAGui::cursorPath.empty())
                    state->mCursorEmpty = false;
                else
                    state->mCursorEmpty = true;
                state->mCursorFrame = FAGui::cursorFrame;
                state->mCursorSpriteGroup = renderer.loadImage("data/inv/objcurs.cel");
                state->mCursorHotspot = FAGui::cursorHotspot;
                state->nuklearData.fill(ctx);
            }

            std::vector<uint32_t> spritesToPreload;
            if (renderer.getAndClearSpritesNeedingPreloading(spritesToPreload))
                ThreadManager::get()->sendSpritesForPreload(spritesToPreload);

            nk_clear(ctx);

            renderer.setCurrentState(state);

            auto remainingTickTime = timer.expires_from_now().total_milliseconds();

            if (remainingTickTime < 0)
                std::cerr << "tick time exceeded by " << -remainingTickTime << "ms" << std::endl;

            timer.wait();
        }

        renderer.stop();
        renderer.waitUntilDone();
    }

    void EngineMain::notify(KeyboardInputAction action)
    {
        if (mGuiManager->isModalDlgShown())
            return;
        if (mPaused && action != KeyboardInputAction::pause)
            return;

        if (action == KeyboardInputAction::pause)
        {
            togglePause();
        }
        if (action == KeyboardInputAction::quit)
        {
            stop();
        }
        else if (action == KeyboardInputAction::noclip)
        {
            toggleNoclip();
        }
    }

    void EngineMain::setupNewPlayer(FAWorld::Player* player)
    {
        mPlayer = player;
        mWorld->addCurrentPlayer(mPlayer);
        mWorld->setLevel(0);
        mGuiManager->setPlayer(mPlayer);
        mInputManager->registerMouseObserver(mPlayer->getPlayerBehaviour());
    }

    void EngineMain::startGame(const std::string& characterClass)
    {
        inGame = true;
        mInputManager->registerKeyboardObserver(mWorld.get());
        // mInputManager->registerMouseObserver(mWorld.get());

        // TODO: fix that variables like invuln are not applied in this case
        setupNewPlayer(mPlayerFactory->create(characterClass));
    }

    const DiabloExe::DiabloExe& EngineMain::exe() const { return *mExe; }

    void EngineMain::stop() { mDone = true; }

    void EngineMain::togglePause() { mPaused = !mPaused; }

    void EngineMain::toggleNoclip() { mNoclip = !mNoclip; }
}
