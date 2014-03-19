#include <render/render.h>
#include <input/inputmanager.h>
#include <level/level.h>
#include <diabloexe/diabloexe.h>

#include "falevelgen/levelgen.h"
#include "falevelgen/random.h"

#include "farender/renderer.h"

#include "faworld/world.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>

bool done = false;
int lr = 0, ud = 0;
bool noclip = true;
int changeLevel = 0;
void keyPress(Input::Key key)
{
    switch(key)
    {
        case Input::KEY_w:
            ud--;
            break;
        case Input::KEY_s:
            ud++;
            break;
        case Input::KEY_a:
            lr--;
            break;
        case Input::KEY_d:
            lr++;
            break;
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
void keyRelease(Input::Key key)
{
    switch(key)
    {
        case Input::KEY_w:
            ud++;
            break;
        case Input::KEY_s:
            ud--;
            break;
        case Input::KEY_a:
            lr++;
            break;
        case Input::KEY_d:
            lr--;
            break;
        default:
            break;
    }
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
                "levels/towndata/town.min", "levels/towndata/town.sol", "levels/towndata/town.cel", std::make_pair(25,29), std::make_pair(0,0));

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

int main(int argc, char** argv)
{
    size_t levelNum;

    boost::program_options::options_description desc("Options");
    desc.add_options()
        ("help,h", "Print help")
        ("level,l", boost::program_options::value<size_t>(&levelNum)->default_value(0), "Level number to load (0-4)");

    boost::program_options::variables_map vm; 
    try 
    { 
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);

        if(vm.count("help"))
        {
            std::cout << desc << std::endl;
            return 0;
        }
        
        boost::program_options::notify(vm);

        if(levelNum > 4)
            throw boost::program_options::validation_error(
                boost::program_options::validation_error::invalid_option_value, "level");
    }
    catch(boost::program_options::error& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        std::cerr << desc << std::endl;
        return 1;
    }

    // Starts rendering thread
    FARender::Renderer renderer;

    DiabloExe::DiabloExe exe;
    FAWorld::World world;

    FALevelGen::FAsrand(time(NULL));

    std::vector<Level::Level*> levels(5);

    size_t currentLevel = levelNum;

    Level::Level* level;

    if(!(level = getLevel(levelNum, exe)))
    {
        done = true;
    }
    else
    {
        levels[currentLevel] = level;
        setLevel(levelNum, exe, world, renderer, *level);
    }
    
    FAWorld::Player* player = world.getPlayer();

    if(levelNum == 0)
        player->mPos = FAWorld::Position(75, 68);

    boost::posix_time::ptime last = boost::posix_time::microsec_clock::local_time();
    
    Input::InputManager input(&keyPress, &keyRelease, NULL, NULL, NULL);
    
    // Main game logic loop
    while(!done)
    {
        input.poll();

        if(changeLevel)
        {
            int32_t tmp = currentLevel + changeLevel;
            if(tmp >= 0 && tmp < levels.size())
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
        
        while(now.time_of_day().total_milliseconds() - last.time_of_day().total_milliseconds() < 1000/FAWorld::World::ticksPerSecond)
        {
            boost::this_thread::sleep(boost::posix_time::milliseconds(1));
            now = boost::posix_time::microsec_clock::local_time();
        }

        last = now;

        if(player->mPos.mDist == 0)
        {
            if(lr || ud)
            {
                if(!player->mPos.mMoving)
                {
                    player->mPos.mMoving = true;
                    player->setAnimation(FAWorld::AnimState::walk);
                }

                switch(lr)
                {
                    case -1:
                    {
                        switch(ud)
                        {
                            case -1:
                            {
                                player->mPos.mDirection = 4;
                                break;
                            }

                            case 0:
                            {
                                player->mPos.mDirection = 3;
                                break;
                            }

                            case 1:
                            {
                                player->mPos.mDirection = 2;
                                break;
                            }
                        }
                        break;
                    }
                    
                    case 0:
                    {
                        switch(ud)
                        {
                            case -1:
                            {
                                player->mPos.mDirection = 5;
                                break;
                            }

                            case 0:
                            {
                                player->mPos.mMoving = false;
                                break;
                            }
                            
                            case 1:
                            {
                                player->mPos.mDirection = 1;
                                break;
                            }
                        }
                        break;
                    }

                    case 1:
                    {
                        switch(ud)
                        {
                            case -1:
                            {
                                player->mPos.mDirection = 6;
                                break;
                            }

                            case 0:
                            {
                                player->mPos.mDirection = 7;
                                break;
                            }
                            
                            case 1:
                            {
                                player->mPos.mDirection = 0;
                                break;
                            }
                        }
                        break;
                    }
                }
            }

            else if(player->mPos.mMoving)
            {
                player->mPos.mMoving = false;
                player->setAnimation(FAWorld::AnimState::idle);
            }
        }

        if(!noclip && !(*level)[player->mPos.next().first][player->mPos.next().second].passable())
        {
            player->mPos.mMoving = false;
            player->setAnimation(FAWorld::AnimState::idle);
        }

        world.update();

        FARender::RenderState* state = renderer.getFreeState();
        
        state->mPos = player->mPos;

        world.fillRenderState(state);

        renderer.setCurrentState(state);
    }

    for(size_t i = 0; i < levels.size(); i++)
        delete levels[i];

    return 0;
}
