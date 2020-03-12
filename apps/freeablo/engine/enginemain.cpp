#include "enginemain.h"
#include "../faaudio/audiomanager.h"
#include "../fagui/guimanager.h"
#include "../falevelgen/levelgen.h"
#include "../fasavegame/gameloader.h"
#include "../faworld/enums.h"
#include "../faworld/itemfactory.h"
#include "../faworld/player.h"
#include "../faworld/playerbehaviour.h"
#include "../faworld/playerfactory.h"
#include "../faworld/world.h"
#include "localinputhandler.h"
#include "net/client.h"
#include "net/server.h"
#include "threadmanager.h"
#include <cxxopts.hpp>
#include <enet/enet.h>
#include <functional>
#include <input/inputmanager.h>
#include <iostream>
#include <misc/misc.h>
#include <random/random.h>
#include <serial/textstream.h>
#include <thread>

namespace Engine
{
    EngineMain* EngineMain::singletonInstance = nullptr;

    EngineMain::EngineMain()
    {
        release_assert(singletonInstance == nullptr);
        singletonInstance = this;
    }

    EngineMain::~EngineMain() { singletonInstance = nullptr; }

    EngineInputManager& EngineMain::inputManager() { return *mInputManager; }

    void EngineMain::run(const cxxopts::ParseResult& variables)
    {
        if (!mSettings.loadUserSettings())
            return;

        auto resolutionWidth = mSettings.get<size_t>("Display", "resolutionWidth");
        auto resolutionHeight = mSettings.get<size_t>("Display", "resolutionHeight");
        const bool fullscreen = mSettings.get<bool>("Display", "fullscreen");
        auto pathEXE = mSettings.get<std::string>("Game", "PathEXE");
        if (pathEXE.empty())
            pathEXE = "Diablo.exe";

        Engine::ThreadManager threadManager;
        FARender::Renderer renderer(resolutionWidth, resolutionHeight, fullscreen);
        mInputManager = std::make_shared<EngineInputManager>(renderer.getNuklearContext());
        mInputManager->registerKeyboardObserver(this);
        std::thread mainThread([&] { this->runGameLoop(variables, pathEXE); });
        threadManager.run();

        mainThread.join();
    }

    void EngineMain::runGameLoop(const cxxopts::ParseResult& variables, const std::string& pathEXE)
    {
        FARender::Renderer& renderer = *FARender::Renderer::get();

        FAWorld::PlayerClass characterClass = FAWorld::playerClassFromString(variables["character"].as<std::string>());

        mExe = std::make_unique<DiabloExe::DiabloExe>(pathEXE);
        if (!mExe->isLoaded())
        {
            renderer.stop();
            return;
        }

        FAWorld::ItemFactory itemFactory(*mExe, Random::DummyRng::instance);
        mPlayerFactory = std::make_unique<FAWorld::PlayerFactory>(*mExe, itemFactory);
        renderer.loadFonts(*mExe);

        FAWorld::Player* player = nullptr;
        int32_t currentLevel = -1;

        auto seed = uint32_t(time(nullptr));
        if (variables["seed"].as<uint32_t>() != 0)
            seed = variables["seed"].as<uint32_t>();

        mWorld = std::make_unique<FAWorld::World>(*mExe, seed);

        mLocalInputHandler = std::make_unique<LocalInputHandler>(*mWorld);
        mInputManager->registerMouseObserver(mLocalInputHandler.get());
        mInputManager->registerKeyboardObserver(mLocalInputHandler.get());

        if (variables["connect"].as<std::string>().empty())
        {
            currentLevel = variables["level"].as<int32_t>();
            if (currentLevel != -1)
            {
                mWorld->generateLevels(); // TODO: not generate levels while game hasn't started

                mInGame = true;
                mMultiplayer = std::make_unique<Server>(*mWorld, *mLocalInputHandler);

                player = mPlayerFactory->create(*mWorld, characterClass);
                if (variables["invuln"].as<std::string>() == "on")
                    player->mInvuln = true;
            }
        }
        else
        {
            mMultiplayer = std::make_unique<Client>(*mLocalInputHandler, variables["connect"].as<std::string>());
        }

        mGuiManager = std::make_unique<FAGui::GuiManager>(*this);
        mInputManager->registerKeyboardObserver(mGuiManager.get());
        mInputManager->setGuiManager(mGuiManager.get());

        if (mMultiplayer && !mMultiplayer->isServer())
            mGuiManager->connectingScreen();

        if (player)
            setupNewPlayer(player);

        if (currentLevel != -1)
            mWorld->setLevel(currentLevel);

        using clock = std::chrono::high_resolution_clock;

        int32_t lastLevelIndex = -1;

        // Main game logic loop
        while (!mDone)
        {
            clock::time_point frameStartTime = clock::now();

            mInputManager->update(mPaused);
            mLocalInputHandler->update();

            if (mMultiplayer)
                mMultiplayer->update();

            if (mInGame && (!mPaused || mMultiplayer->isMultiplayer()))
            {
                std::optional<std::vector<FAWorld::PlayerInput>> inputs;

                do
                {
                    inputs = mMultiplayer->getAndClearInputs(mWorld->getCurrentTick());

                    if (inputs)
                    {
                        mMultiplayer->verify(mWorld->getCurrentTick());
                        mWorld->update(mNoclip, *inputs);

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

            if (mMultiplayer && mMultiplayer->isMultiplayer())
                mMultiplayer->doMultiplayerGui(ctx);

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

            if (state)
                renderer.setCurrentState(state);

            clock::time_point frameEndTargetTime = frameStartTime + std::chrono::milliseconds(1000 / FAWorld::World::ticksPerSecond);
            clock::duration remainingTickTime = frameEndTargetTime - clock::now();
            auto remainingMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(remainingTickTime);

            if (remainingMilliseconds.count() < 0)
                std::cerr << "tick time exceeded by " << -remainingMilliseconds.count() << "ms" << std::endl;
            else
                std::this_thread::sleep_until(frameEndTargetTime);
        }

        renderer.stop();
        renderer.waitUntilDone();
    }

    void EngineMain::notify(KeyboardInputAction action)
    {
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

    void EngineMain::startGame(FAWorld::PlayerClass characterClass)
    {
        mWorld->generateLevels();

        mInGame = true;
        mMultiplayer = std::make_unique<Server>(*mWorld, *mLocalInputHandler);

        // TODO: fix that variables like invuln are not applied in this case
        auto player = mPlayerFactory->create(*mWorld, characterClass);
        setupNewPlayer(player);

        mWorld->setLevel(0);
    }

    void EngineMain::startGameFromSave(const std::string& savePath)
    {
        FILE* saveFile = fopen(savePath.c_str(), "rb");
        release_assert(saveFile);

        fseek(saveFile, 0, SEEK_END);
        size_t size = ftell(saveFile);
        fseek(saveFile, 0, SEEK_SET);

        std::string tmp;
        tmp.resize(size);

        fread((void*)tmp.data(), 1, size, saveFile);
        fclose(saveFile);

        Serial::TextReadStream stream(tmp);
        FASaveGame::GameLoader loader(stream);

        mWorld->load(loader);
        mWorld->setFirstPlayerAsCurrent();

        mInGame = true;
        mMultiplayer = std::make_unique<Server>(*mWorld, *mLocalInputHandler);
    }

    void EngineMain::startMultiplayerGame(const std::string& serverAddress) { mMultiplayer = std::make_unique<Client>(*mLocalInputHandler, serverAddress); }

    const DiabloExe::DiabloExe& EngineMain::exe() const { return *mExe; }

    bool EngineMain::isPaused() const { return mPaused; }

    void EngineMain::stop() { mDone = true; }

    void EngineMain::togglePause() { mPaused = !mPaused; }

    void EngineMain::toggleNoclip() { mNoclip = !mNoclip; }
}
