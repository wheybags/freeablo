#include "enginemain.h"

#include <iostream>

#include "../faworld/world.h"
#include "../falevelgen/levelgen.h"
#include "../falevelgen/random.h"
#include "../fagui/guimanager.h"
#include "../faaudio/audiomanager.h"
#include "threadmanager.h"
#include "input.h"

#include <misc/misc.h>
#include <input/inputmanager.h>


#include <boost/thread.hpp>

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

        Engine::ThreadManager threadManager;
        FARender::Renderer renderer(resolutionWidth, resolutionHeight);

        mInputManager = new EngineInputManager();

        boost::thread mainThread(boost::bind(&EngineMain::runGameLoop, this, &variables));

        threadManager.run();
        renderDone = true;

        mainThread.join();
    }

    void EngineMain::runGameLoop(const bpo::variables_map& variables)
    {
        FALevelGen::FAsrand(time(NULL));

        FARender::Renderer& renderer = *FARender::Renderer::get();
        Input::InputManager& input = *Input::InputManager::get();
        Engine::ThreadManager& threadManager = *Engine::ThreadManager::get();

        DiabloExe::DiabloExe exe;

        if (!exe.isLoaded())
        {
            renderer.stop();
            return;
        }
        Level::ItemManager itemManager;

        itemManager.loadItems(&exe);


        FAWorld::World world(exe);
        world.generateLevels();


        int32_t currentLevel = variables["level"].as<int32_t>();

        FAWorld::Player* player = world.getPlayer();

        FAGui::initGui(player->mInventory);

        // -1 represents the main menu
        if(currentLevel != -1)
        {
            world.setLevel(currentLevel);
            Level::Level& level = *world.getCurrentLevel();

            player->mPos = FAWorld::Position(level.upStairsPos().first, level.upStairsPos().second);

            FAGui::showIngameGui();
        }
        else
        {
            Engine::paused = true;
            FAGui::showMainMenu();
            threadManager.playMusic("music/dintro.wav");
        }

        boost::posix_time::ptime last = boost::posix_time::microsec_clock::local_time();

        Engine::quit_key = Input::Hotkey("Quit");
        Engine::noclip_key = Input::Hotkey("Noclip");
        Engine::changelvlup_key = Input::Hotkey("Changelvlup");
        Engine::changelvldwn_key = Input::Hotkey("Changelvldwn");

        // Main game logic loop
        while(!Engine::done)
        {
            mInputManager->update(Engine::paused);

            std::pair<size_t, size_t>& destination = player->destination();

            boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();

            while((size_t)(now.time_of_day().total_milliseconds() - last.time_of_day().total_milliseconds()) < 1000/FAWorld::World::ticksPerSecond)
            {
                boost::this_thread::yield();
                now = boost::posix_time::microsec_clock::local_time();
            }

            last = now;

            Level::Level* level = world.getCurrentLevel();

            if(!Engine::paused)
            {
                if(Engine::mouseDown)
                {
                    destination = renderer.getClickedTile(Engine::xClick, Engine::yClick, *level, player->mPos);
                    if(Engine::click)
                        level->activate(destination.first, destination.second);

                    Engine::click = false;
                }

                if(player->mPos.current() != destination)
                {
                    if(player->mPos.mDist == 0)
                    {
                        std::pair<float, float> vector = Misc::getVec(player->mPos.current(), destination);

                        if(!player->mPos.mMoving)
                        {
                            player->mPos.mMoving = true;
                            player->setAnimation(FAWorld::AnimState::walk);
                        }

                        player->mPos.mDirection = Misc::getVecDir(vector);
                    }
                }
                else if(player->mPos.mMoving && player->mPos.mDist == 0)
                {
                    player->mPos.mMoving = false;
                    player->setAnimation(FAWorld::AnimState::idle);
                }

                FAWorld::Actor* actorAtNext = world.getActorAt(player->mPos.next().first, player->mPos.next().second);

                if(!Engine::noclip && (!(*level)[player->mPos.next().first][player->mPos.next().second].passable() ||
                   (actorAtNext != NULL && actorAtNext != player)))
                {
                    player->mPos.mMoving = false;
                    destination = player->mPos.current();
                    player->setAnimation(FAWorld::AnimState::idle);
                }

                world.update();
            }

            FAGui::updateGui();

            FARender::RenderState* state = renderer.getFreeState();

            if(state)
            {
                state->mPos = player->mPos;
                if(level != NULL)
                    state->tileset = renderer.getTileset(*level);;

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
}