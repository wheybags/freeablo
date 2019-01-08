#include "enginemain.h"
#include "../faaudio/audiomanager.h"
#include "../fagui/guimanager.h"
#include "../falevelgen/levelgen.h"
#include "../fasavegame/gameloader.h"
#include "../faworld/itemfactory.h"
#include "../faworld/player.h"
#include "../faworld/playerbehaviour.h"
#include "../faworld/playerfactory.h"
#include "../faworld/world.h"
#include "localinputhandler.h"
#include "net/client.h"
#include "net/server.h"
#include "threadmanager.h"
#include <boost/asio.hpp>
#include <boost/make_unique.hpp>
#include <enet/enet.h>
#include <functional>
#include <input/inputmanager.h>
#include <iostream>
#include <misc/misc.h>
#include <random/random.h>
#include <serial/textstream.h>
#include <thread>

namespace bpo = boost::program_options;

namespace Engine
{
    EngineMain* EngineMain::singletonInstance = nullptr;

    EngineMain::EngineMain()
    {
        release_assert(singletonInstance == nullptr);
        singletonInstance = this;
    }

    EngineMain::~EngineMain() { singletonInstance = nullptr; }

    EngineInputManager& EngineMain::inputManager() { return *(mInputManager.get()); }

    void EngineMain::run(const bpo::variables_map& variables)
    {
        Settings::Settings settings;
        if (!settings.loadUserSettings())
            return;

        size_t resolutionWidth = settings.get<size_t>("Display", "resolutionWidth");
        size_t resolutionHeight = settings.get<size_t>("Display", "resolutionHeight");
        const bool fullscreen = settings.get<bool>("Display", "fullscreen");
        std::string pathEXE = settings.get<std::string>("Game", "PathEXE");
        if (pathEXE == "")
        {
            pathEXE = "Diablo.exe";
        }

        Engine::ThreadManager threadManager;
        FARender::Renderer renderer(resolutionWidth, resolutionHeight, fullscreen);
        mInputManager = std::make_shared<EngineInputManager>(renderer.getNuklearContext());
        mInputManager->registerKeyboardObserver(this);
        std::thread mainThread(std::bind(&EngineMain::runGameLoop, this, &variables, pathEXE));
        threadManager.run();

        mainThread.join();
    }

    void EngineMain::playVideo(const std::string& path)
    {
        Engine::ThreadManager::get()->playVideo(std::string(path));
        while (!Engine::ThreadManager::get()->waitForVideoComplete(100))
        {
            mInputManager->update(false);
        }
    }

    void EngineMain::runGameLoop(const bpo::variables_map& variables, const std::string& pathEXE)
    {
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

        // Play the intro cinematics.
        playVideo("gendata/logo.smk");
        playVideo("gendata/diablo1.smk");

        FAWorld::ItemFactory itemFactory(*mExe, Random::DummyRng::instance);
        mPlayerFactory = boost::make_unique<FAWorld::PlayerFactory>(*mExe, itemFactory);
        renderer.loadFonts(*mExe);

        FAWorld::Player* player = nullptr;
        int32_t currentLevel = -1;
        mWorld.reset(new FAWorld::World(*mExe, uint32_t(time(nullptr))));

        if (!variables["client"].as<bool>())
        {
            FILE* f = fopen("save.sav", "rb");
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

                mWorld->load(loader);
                mWorld->setFirstPlayerAsCurrent();

                mInGame = true;
            }
            else
            {
                currentLevel = variables["level"].as<int32_t>();

                mWorld->generateLevels(); // TODO: not generate levels while game hasn't started

                if (currentLevel != -1)
                {
                    mInGame = true;
                    player = mPlayerFactory->create(*mWorld, characterClass);
                    if (variables["invuln"].as<std::string>() == "on")
                        player->mInvuln = true;
                }
            }
        }

        mGuiManager.reset(new FAGui::GuiManager(*this));
        mInputManager->registerKeyboardObserver(mGuiManager.get());
        mInputManager->setGuiManager(mGuiManager.get());
        if (player)
            setupNewPlayer(player);

        if (currentLevel != -1)
            mWorld->setLevel(currentLevel);

        boost::asio::io_service io;

        mLocalInputHandler.reset(new LocalInputHandler(*mWorld));
        mInputManager->registerMouseObserver(mLocalInputHandler.get());
        mInputManager->registerKeyboardObserver(mLocalInputHandler.get());

        if (variables["client"].as<bool>())
        {
            mMultiplayer.reset(new Client(*mLocalInputHandler.get()));
            mInGame = false;
        }
        else
        {
            mMultiplayer.reset(new Server(*mWorld.get(), *mLocalInputHandler.get()));
        }

        int32_t lastLevelIndex = -1;

        // Main game logic loop
        while (!mDone)
        {
            boost::asio::deadline_timer timer(io, boost::posix_time::milliseconds(1000 / FAWorld::World::ticksPerSecond));

            mInputManager->update(mPaused);
            mLocalInputHandler->update();
            mMultiplayer->update();

            if ((!mPaused || mMultiplayer->isMultiplayer()) && mInGame)
            {
                boost::optional<std::vector<FAWorld::PlayerInput>> inputs;

                do
                {
                    inputs = mMultiplayer->getAndClearInputs(mWorld->getCurrentTick());

                    if (inputs)
                    {
                        mMultiplayer->verify(mWorld->getCurrentTick());
                        mWorld->update(mNoclip, inputs.get());

                        if (mWorld->getCurrentLevelIndex() != lastLevelIndex)
                        {
                            mWorld->playLevelMusic(mWorld->getCurrentLevelIndex());
                            lastLevelIndex = mWorld->getCurrentLevelIndex();
                        }
                    }

                } while (inputs);
            }

            nk_context* ctx = renderer.getNuklearContext();
            mGuiManager->update(mInGame, mPaused, ctx, mLocalInputHandler->getHoverStatus());

            FARender::RenderState* state = renderer.getFreeState();
            if (state)
            {
                if (mWorld->getCurrentPlayer())
                {
                    auto level = mWorld->getCurrentLevel();
                    state->mPos = mWorld->getCurrentPlayer()->getPos();
                    if (level != nullptr)
                        state->tileset = renderer.getTileset(*level);
                    state->level = level;
                    mWorld->fillRenderState(state, mLocalInputHandler->getHoverStatus());
                }
                else
                    state->level = nullptr;

                state->mCursorPath = FAGui::cursorPath;

                if (!mPaused && mWorld->getCurrentPlayer())
                {
                    auto item = mWorld->getCurrentPlayer()->mInventory.getCursorHeld();
                    state->mCursorFrame = item.getGraphicValue();
                    // When items are held, their sprites are centered around the cursor (rather then top left).
                    state->mCursorCentered = !item.isEmpty();
                }
                else
                {
                    state->mCursorFrame = 0;
                    state->mCursorCentered = false;
                }
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
        // Only handle the esc key when playing video.
        if (Engine::ThreadManager::get()->videoInProgress())
        {
            if (action == KeyboardInputAction::pause)
                Engine::ThreadManager::get()->stopVideo();
            return;
        }

        if (mGuiManager->isPauseBlocked())
            return;

        if (action == KeyboardInputAction::pause)
        {
            if (mGuiManager->anyPanelIsOpen())
                mGuiManager->closeAllPanels();
            else if (mGuiManager->isModalDlgShown())
                mGuiManager->popModalDlg();
            else
                togglePause();
        }
        else if (mPaused)
            return;

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
        mWorld->addCurrentPlayer(player);
        mGuiManager->setPlayer(player);
    }

    void EngineMain::startGame(const std::string& characterClass)
    {
        mInGame = true;

        // TODO: fix that variables like invuln are not applied in this case
        auto player = mPlayerFactory->create(*mWorld, characterClass);
        setupNewPlayer(player);
        mWorld->setLevel(0);
    }

    const DiabloExe::DiabloExe& EngineMain::exe() const { return *mExe; }

    bool EngineMain::isPaused() const { return mPaused; }

    void EngineMain::stop() { mDone = true; }

    void EngineMain::togglePause() { mPaused = !mPaused; }

    void EngineMain::toggleNoclip() { mNoclip = !mNoclip; }
}
