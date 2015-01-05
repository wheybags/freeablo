#include <render/render.h>
#include <input/inputmanager.h>
#include <level/level.h>
#include <diabloexe/diabloexe.h>
#include <misc/misc.h>

#include "engine/threadmanager.h"

#include "falevelgen/levelgen.h"
#include "falevelgen/random.h"

#include "farender/renderer.h"

#include "faworld/world.h"

#include "fagui/guimanager.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

#include <misc/fareadini.h>
#include <boost/property_tree/ptree.hpp>

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;
namespace bpt = boost::property_tree;

bool done = false;
bool paused = false;
bool noclip = false;
int changeLevel = 0;

int quit_key;
int noclip_key;
int changelvldwn_key;
int changelvlup_key;

void keyPress(Input::Key key)
{
    if (key == quit_key)
    {
        done = true;
    }
    else if (key == noclip_key)
    {
        noclip = !noclip;
    }
    else if (key == changelvldwn_key)
    {
        changeLevel = 1;
    }
    else if (key == changelvlup_key)
    {
        changeLevel = -1;
    }
}

size_t xClick = 0, yClick = 0;
bool mouseDown = false;
bool click = false;
void mouseClick(size_t x, size_t y, Input::Key key)
{
    if(key == Input::KEY_LEFT_MOUSE)
    {
        xClick = x;
        yClick = y;
        mouseDown = true;
        click = true;
    }
}

void mouseRelease(size_t, size_t, Input::Key key)
{
    if(key == Input::KEY_LEFT_MOUSE)
        mouseDown = false;
}

void mouseMove(size_t x, size_t y)
{
    xClick = x;
    yClick = y;
}

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
    else if(dLvl < 9)
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
 * @brief Handle parsing of command line arguments.
 * @return True if no problems occurred and execution should continue.
 */
bool parseOptions(int argc, char** argv, bpo::variables_map& variables)
{
    boost::program_options::options_description desc("Options");

    desc.add_options()
        ("help,h", "Print help")
        // -1 represents the main menu
        ("level,l", bpo::value<int32_t>()->default_value(-1), "Level number to load (0-16)");

    try 
    { 
        bpo::store(bpo::parse_command_line(argc, argv, desc), variables);

        if(variables.count("help"))
        {
            std::cout << desc << std::endl;
            return false;
        }
        
        bpo::notify(variables);

        const int32_t dLvl = variables["level"].as<int32_t>();
        if(dLvl > 16)
            throw bpo::validation_error(
                bpo::validation_error::invalid_option_value, "level");
    }
    catch(bpo::error& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        std::cerr << desc << std::endl;
        return false;
    }

    return true;
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
bool loadSettings(StartupSettings& settings)
{
    // TODO: handling of application paths via FAIO interface
    const std::string settingsDefaultPath = "resources/settings-default.ini";
    const std::string settingsUserPath = "resources/settings-user.ini";

    bpo::variables_map variables;
    bpo::options_description desc("Settings");

    desc.add_options()
        ("Display.resolutionWidth", bpo::value<size_t>())
        ("Display.resolutionHeight", bpo::value<size_t>());

    const bool allowUnregisteredOptions = true;

    // User settings - handle first to give priority over default settings.
    try
    {
        std::ifstream settingsFile(settingsUserPath.c_str());

        bpo::store(
            bpo::parse_config_file(settingsFile, desc, allowUnregisteredOptions),
            variables);
    }
    catch(bpo::error& e)
    {
        std::cerr << "Unable to process settings file \"" + settingsUserPath + "\"." << std::endl;
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        return false;
    }

    // Default settings.
    try
    {
        if (!bfs::exists(settingsDefaultPath))
        {
            std::cerr << "Default settings file not found. Please verify that \"" + settingsDefaultPath + "\" exists." << std::endl;
            return false;
        }

        std::ifstream settingsFile(settingsDefaultPath.c_str());

        bpo::store(
            bpo::parse_config_file(settingsFile, desc, allowUnregisteredOptions),
            variables);

        bpo::notify(variables);

        // Parameter parsing.
        {
            settings.resolutionWidth = variables["Display.resolutionWidth"].as<size_t>();
            settings.resolutionHeight = variables["Display.resolutionHeight"].as<size_t>();
        }
    }
    catch(bpo::error& e)
    {
        std::cerr << "Unable to process settings file \"" + settingsDefaultPath + "\"." << std::endl;
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        return false;
    }

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
    boost::program_options::variables_map variables;

    if (parseOptions(argc, argv, variables))
    {
        run(variables);
    }

    return 0;
}

void run(const bpo::variables_map& variables)
{
    StartupSettings settings;
    if (!loadSettings(settings))
        return;


    Engine::ThreadManager threadManager;
    FARender::Renderer renderer(settings.resolutionWidth, settings.resolutionHeight);
    Input::InputManager input(&keyPress, NULL, &mouseClick, &mouseRelease, &mouseMove, renderer.getRocketContext());

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
    FAWorld::World world;

    FALevelGen::FAsrand(time(NULL));

    std::vector<Level::Level*> levels(9);

    int32_t currentLevel = variables["level"].as<int32_t>();

    Level::Level* level = NULL;
    FARender::Tileset tileset;
    
    FAWorld::Player* player = world.getPlayer();
    FAGui::initGui();
    
    // -1 represents the main menu
    if(currentLevel != -1)
    {
        if(!(level = getLevel(currentLevel, exe)))
        {
            done = true;
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
        paused = true;
        FAGui::showMainMenu();
        threadManager.playMusic("music/dintro.wav");
    }
    
    boost::posix_time::ptime last = boost::posix_time::microsec_clock::local_time();
    
    std::pair<size_t, size_t> destination = player->mPos.current();
    
    bpt::ptree hotkeypt;
    Misc::readIni("resources/hotkeys.ini", hotkeypt);
    
    quit_key = hotkeypt.get<int>("Hotkeys.quit");
    noclip_key = hotkeypt.get<int>("Hotkeys.noclip");
    changelvldwn_key = hotkeypt.get<int>("Hotkeys.changelvldwn");
    changelvlup_key = hotkeypt.get<int>("Hotkeys.changelvlup");

    
    // Main game logic loop
    while(!done)
    {
        input.processInput(paused);

        boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
        
        while((size_t)(now.time_of_day().total_milliseconds() - last.time_of_day().total_milliseconds()) < 1000/FAWorld::World::ticksPerSecond)
        {
            boost::this_thread::yield();
            now = boost::posix_time::microsec_clock::local_time();
        }

        last = now;

        
        if(!paused)
        {
            if(mouseDown)
            {
                destination = renderer.getClickedTile(xClick, yClick, *level, player->mPos);
                if(click)
                    level->activate(destination.first, destination.second);

                click = false;
            }

            if(changeLevel)
            {
                int32_t tmp = currentLevel + changeLevel;
                if(tmp >= 0 && tmp < (int32_t)levels.size())
                {
                    currentLevel = tmp;

                    if(levels[currentLevel] == NULL)
                        levels[currentLevel] = getLevel(currentLevel, exe);

                    level = levels[currentLevel];
                    
                    if(changeLevel == -1)
                        player->mPos = FAWorld::Position(level->downStairsPos().first, level->downStairsPos().second);
                    else
                        player->mPos = FAWorld::Position(level->upStairsPos().first, level->upStairsPos().second);

                    destination = player->mPos.current();
                    
                    setLevel(currentLevel, exe, world, level);
                    tileset = renderer.getTileset(*level);
                    playLevelMusic(currentLevel, threadManager);
                }
                
                changeLevel = 0;
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
            
            if(!noclip && (!(*level)[player->mPos.next().first][player->mPos.next().second].passable() || 
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
        
        state->mPos = player->mPos;
        state->tileset = tileset;
        state->level = level;

        world.fillRenderState(state);

        Render::updateGuiBuffer(state->guiDrawBuffer);

        renderer.setCurrentState(state);
    }
    
    FAGui::destroyGui();
    renderer.stop();    

    for(size_t i = 0; i < levels.size(); i++)
    {
        if(levels[i])
            delete levels[i];
    }
}
