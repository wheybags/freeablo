#include <render/render.h>
#include <input/inputmanager.h>
#include <level/level.h>
#include <diabloexe/diabloexe.h>
#include <misc/misc.h>

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

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;

bool done = false;
bool noclip = true;
int changeLevel = 0;
void keyPress(Input::Key key)
{
    switch(key)
    {
        case Input::KEY_q:
            done = true;
            break;
        case Input::KEY_n:
            noclip = !noclip;
            break;
        case Input::KEY_DOWN:
            changeLevel = 1;
            break;
        case Input::KEY_UP:
            changeLevel = -1;
            break;
        default:
            break;
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

void setLevel(size_t levelNum, const DiabloExe::DiabloExe& exe, FAWorld::World& world, FARender::Renderer& renderer, const Level::Level& level)
{
    world.clear();
    if(levelNum == 0)
        world.addNpcs(exe);
    renderer.setLevel(level);
    world.setLevel(level, exe);
}

Level::Level* getLevel(size_t levelNum, const DiabloExe::DiabloExe& exe)
{  
    switch(levelNum)
    {
        case 0:
        {
            Level::Dun sector1("levels/towndata/sector1s.dun");
            Level::Dun sector2("levels/towndata/sector2s.dun");
            Level::Dun sector3("levels/towndata/sector3s.dun");
            Level::Dun sector4("levels/towndata/sector4s.dun");

            return new Level::Level(Level::Dun::getTown(sector1, sector2, sector3, sector4), "levels/towndata/town.til", 
                "levels/towndata/town.min", "levels/towndata/town.sol", "levels/towndata/town.cel", std::make_pair(25,29), std::make_pair(0,0), std::map<size_t, size_t>());

            break;
        }

        case 1:
        {
            return FALevelGen::generate(100, 100, levelNum, exe, "levels/l1data/l1.cel");
            break;
        }

        case 2:
        case 3:
        case 4:
        {
            std::cerr << "level not supported yet" << std::endl;
            break;
        }
    }

    return NULL;
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
        ("level,l", bpo::value<size_t>()->default_value(0), "Level number to load (0-4)");

    try 
    { 
        bpo::store(bpo::parse_command_line(argc, argv, desc), variables);

        if(variables.count("help"))
        {
            std::cout << desc << std::endl;
            return false;
        }
        
        bpo::notify(variables);

        const size_t levelNum = variables["level"].as<size_t>();
        if(levelNum > 4)
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

    boost::thread mainThread(boost::bind(&runGameLoop, &variables));

    FARender::Renderer renderer(settings.resolutionWidth, settings.resolutionHeight);
    renderDone = true;

    mainThread.join();
}

void runGameLoop(const bpo::variables_map& variables)
{
    while(!FARender::Renderer::get()) {}

    FARender::Renderer& renderer = *FARender::Renderer::get();
    Input::InputManager input(&keyPress, NULL, &mouseClick, &mouseRelease, &mouseMove, renderer.getRocketContext());
    FAGui::GuiManager guiManager;

    DiabloExe::DiabloExe exe;
    FAWorld::World world;

    FALevelGen::FAsrand(time(NULL));

    std::vector<Level::Level*> levels(5);

    size_t currentLevel = variables["level"].as<size_t>();

    Level::Level* level;

    if(!(level = getLevel(currentLevel, exe)))
    {
        done = true;
    }
    else
    {
        levels[currentLevel] = level;
        setLevel(currentLevel, exe, world, renderer, *level);
    }
    
    FAWorld::Player* player = world.getPlayer();

    if(currentLevel == 0)
        player->mPos = FAWorld::Position(75, 68);
    else
        player->mPos = FAWorld::Position(level->upStairsPos().first, level->upStairsPos().second);

    boost::posix_time::ptime last = boost::posix_time::microsec_clock::local_time();
    
    std::pair<size_t, size_t> destination = player->mPos.current();

    guiManager.showGameBottomMenu();
    
    // Main game logic loop
    while(!done)
    {
        if(mouseDown)
        {
            destination = renderer.getClickedTile(xClick, yClick);
            if(click)
                level->activate(destination.first, destination.second);

            click = false;
        }

        renderer.lockGui();
        input.processInput();
        renderer.unlockGui();

        if(changeLevel)
        {
            int32_t tmp = currentLevel + changeLevel;
            if(tmp >= 0 && tmp < (int32_t)levels.size())
            {
                currentLevel = tmp;

                if(levels[currentLevel] == NULL)
                    levels[currentLevel] = getLevel(currentLevel == 0 ? 0 : 1, exe);

                level = levels[currentLevel];
                
                if(changeLevel == -1)
                    player->mPos = FAWorld::Position(level->downStairsPos().first, level->downStairsPos().second);
                else
                    player->mPos = FAWorld::Position(level->upStairsPos().first, level->upStairsPos().second);
                
                setLevel(currentLevel, exe, world, renderer, *level);

            }
            
            changeLevel = 0;
        }

        boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
        
        while((size_t)(now.time_of_day().total_milliseconds() - last.time_of_day().total_milliseconds()) < 1000/FAWorld::World::ticksPerSecond)
        {
            boost::this_thread::sleep(boost::posix_time::milliseconds(1));
            now = boost::posix_time::microsec_clock::local_time();
        }

        last = now;

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

        if(!noclip && !(*level)[player->mPos.next().first][player->mPos.next().second].passable())
        {
            player->mPos.mMoving = false;
            player->setAnimation(FAWorld::AnimState::idle);
        }

        world.update();
        
        guiManager.update();

        FARender::RenderState* state = renderer.getFreeState();
        
        state->mPos = player->mPos;

        world.fillRenderState(state);

        renderer.setCurrentState(state);
    }
    
    guiManager.destroy();
    renderer.stop();    

    while(!renderDone) {} // have to wait until the renderer stops before destroying all our locals

    for(size_t i = 0; i < levels.size(); i++)
        delete levels[i];
}
