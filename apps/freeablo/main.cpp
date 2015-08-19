#include <render/render.h>
#include <input/inputmanager.h>
#include <level/level.h>
#include <diabloexe/diabloexe.h>
#include <misc/misc.h>

#include "engine/threadmanager.h"
#include "engine/input.h"
#include "engine/enginemain.h"

#include "falevelgen/levelgen.h"
#include "falevelgen/random.h"

#include "farender/renderer.h"

#include "faworld/world.h"

#include <level/itemmanager.h>

#include "fagui/guimanager.h"


#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <fstream>

#include <settings/settings.h>

#include <input/hotkey.h>

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;
namespace bpt = boost::property_tree;

void setLevel(size_t dLvl, const DiabloExe::DiabloExe& exe, FAWorld::World& world, Level::Level* level)
{
    world.clear();
    world.setLevel(*level, exe);

    if(dLvl == 0)
        world.addNpcs(exe);
}

Level::Level* getLevel(size_t dLvl, const DiabloExe::DiabloExe& exe)
{  
    if(dLvl == 0)
    {
        Level::Dun sector1("levels/towndata/sector1s.dun");
        Level::Dun sector2("levels/towndata/sector2s.dun");
        Level::Dun sector3("levels/towndata/sector3s.dun");
        Level::Dun sector4("levels/towndata/sector4s.dun");

        return new Level::Level(Level::Dun::getTown(sector1, sector2, sector3, sector4), "levels/towndata/town.til",
            "levels/towndata/town.min", "levels/towndata/town.sol", "levels/towndata/town.cel", std::make_pair(25,29), std::make_pair(75,68), std::map<size_t, size_t>());
    }
    else if(dLvl < 13)
    {
        return FALevelGen::generate(100, 100, dLvl, exe);
    }
    else
    {
        std::cerr << "level not supported yet" << std::endl;
        exit(1);
        return NULL;
    }
}



/**
 * @brief Holds startup settings read from settings files.
 */
struct StartupSettings
{
    size_t resolutionWidth;
    size_t resolutionHeight;
};

/**
 * @brief Load and parse settings files.
 */
bool loadSettings(StartupSettings& startupSettings)
{
    Settings::Settings settings;
    if(!settings.loadUserSettings())
        return false;

    startupSettings.resolutionWidth = settings.get<size_t>("Display","resolutionWidth");
    startupSettings.resolutionHeight = settings.get<size_t>("Display","resolutionHeight");

    return true;
}

void playLevelMusic(int32_t currentLevel, Engine::ThreadManager& threadManager)
{
    switch(currentLevel)
    {
        case 0:
        {
            threadManager.playMusic("music/dtowne.wav");
            break;
        }
        case 1: case 2: case 3: case 4:
        {
            threadManager.playMusic("music/dlvla.wav");
            break;
        }
        case 5: case 6: case 7: case 8:
        {
            threadManager.playMusic("music/dlvlb.wav");
            break;
        }
        case 9: case 10: case 11: case 12:
        {
            threadManager.playMusic("music/dlvlc.wav");
            break;
        }
        case 13: case 14: case 15: case 16:
        {
            threadManager.playMusic("music/dlvld.wav");
            break;
        }
    }
}

void run(const bpo::variables_map& variables);
void runGameLoop(const bpo::variables_map& variables);

volatile bool renderDone = false;

/**
 * @brief Main entry point.
 */
int main(int argc, char** argv)
{
    if (!FAIO::init())
    {
        return EXIT_FAILURE;
    }

    boost::program_options::variables_map variables;

    Engine::EngineMain engine;

    if (engine.parseOptions(argc, argv, variables))
    {
        run(variables);
    }

    FAIO::quit();
    return 0;
}

void run(const bpo::variables_map& variables)
{
    StartupSettings settings;
    if (!loadSettings(settings))
        return;


    Engine::ThreadManager threadManager;
    FARender::Renderer renderer(settings.resolutionWidth, settings.resolutionHeight);

    Input::InputManager input(&Engine::keyPress, NULL, &Engine::mouseClick, &Engine::mouseRelease, &Engine::mouseMove, renderer.getRocketContext());

    boost::thread mainThread(boost::bind(&runGameLoop, &variables));

    threadManager.run();
    renderDone = true;

    mainThread.join();
}

void runGameLoop(const bpo::variables_map& variables)
{
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


    FAWorld::World world;

    FALevelGen::FAsrand(time(NULL));

    std::vector<Level::Level*> levels(13);

    int32_t currentLevel = variables["level"].as<int32_t>();

    Level::Level* level = NULL;
    FARender::Tileset tileset;
    
    FAWorld::Player* player = world.getPlayer();

    FAGui::initGui(player->mInventory);
    
    // -1 represents the main menu
    if(currentLevel != -1)
    {
        if(!(level = getLevel(currentLevel, exe)))
        {
            Engine::done = true;
        }
        else
        {
            tileset = renderer.getTileset(*level);
            levels[currentLevel] = level;
            setLevel(currentLevel, exe, world, level);
        }
        
        player->mPos = FAWorld::Position(level->upStairsPos().first, level->upStairsPos().second);

        FAGui::showIngameGui();

        playLevelMusic(currentLevel, threadManager);
    }
    else
    {
        Engine::paused = true;
        FAGui::showMainMenu();
        threadManager.playMusic("music/dintro.wav");
    }
    
    boost::posix_time::ptime last = boost::posix_time::microsec_clock::local_time();
    
    std::pair<size_t, size_t> destination = player->mPos.current();
            
    Engine::quit_key = Input::Hotkey("Quit");
    Engine::noclip_key = Input::Hotkey("Noclip");
    Engine::changelvlup_key = Input::Hotkey("Changelvlup");
    Engine::changelvldwn_key = Input::Hotkey("Changelvldwn");
    
    // Main game logic loop
    while(!Engine::done)
    {

        input.processInput(Engine::paused);

        boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
        
        while((size_t)(now.time_of_day().total_milliseconds() - last.time_of_day().total_milliseconds()) < 1000/FAWorld::World::ticksPerSecond)
        {
            boost::this_thread::yield();
            now = boost::posix_time::microsec_clock::local_time();
        }

        last = now;

        
        if(!Engine::paused)
        {
            if(Engine::mouseDown)
            {
                destination = renderer.getClickedTile(Engine::xClick, Engine::yClick, *level, player->mPos);
                if(Engine::click)
                    level->activate(destination.first, destination.second);

                Engine::click = false;
            }

            if(Engine::changeLevel)
            {
                int32_t tmp = currentLevel + Engine::changeLevel;
                if(tmp >= 0 && tmp < (int32_t)levels.size())
                {
                    currentLevel = tmp;

                    if(levels[currentLevel] == NULL)
                        levels[currentLevel] = getLevel(currentLevel, exe);

                    level = levels[currentLevel];
                    
                    if(Engine::changeLevel == -1)
                        player->mPos = FAWorld::Position(level->downStairsPos().first, level->downStairsPos().second);
                    else
                        player->mPos = FAWorld::Position(level->upStairsPos().first, level->upStairsPos().second);

                    destination = player->mPos.current();
                    
                    setLevel(currentLevel, exe, world, level);
                    tileset = renderer.getTileset(*level);
                    playLevelMusic(currentLevel, threadManager);
                }
                
                Engine::changeLevel = 0;
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
            state->tileset = tileset;
            state->level = level;
            if(!FAGui::cursorPath.empty())
                state->mCursorEmpty = false;
            else
                state->mCursorEmpty = true;
            state->mCursorFrame = FAGui::cursorFrame;
            state->mCursorSpriteGroup = renderer.loadImage("data/inv/objcurs.cel");//.operator [](FAGui::cursorFrame);



            //state->mCursorSpritePtr =

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

    for(size_t i = 0; i < levels.size(); i++)
    {
        if(levels[i])
            delete levels[i];
    }
}
