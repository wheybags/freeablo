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

Level::Level getLevel(size_t levelNum, const DiabloExe::DiabloExe& exe)
{
    if(levelNum > 0) 
    {
        FALevelGen::FAsrand(time(NULL));
        return FALevelGen::generate(100, 100, levelNum, exe);
    }
    else
    {
        Level::Dun sector1("levels/towndata/sector1s.dun");
        Level::Dun sector2("levels/towndata/sector2s.dun");
        Level::Dun sector3("levels/towndata/sector3s.dun");
        Level::Dun sector4("levels/towndata/sector4s.dun");

        return Level::Level(Level::Dun::getTown(sector1, sector2, sector3, sector4), "levels/towndata/town.til", "levels/towndata/town.min", "levels/towndata/town.sol");
    }
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

    // Starts input thread
    Input::InputManager input(&keyPress, &keyRelease);


    DiabloExe::DiabloExe exe;

    Level::Level level = getLevel(levelNum, exe);

    if(!renderer.setLevel(level, levelNum))
        return 1;

    FAWorld::World world;
    world.setLevel(level, exe);

    if(levelNum == 0)
        world.addNpcs(exe);

    boost::posix_time::ptime last = boost::posix_time::microsec_clock::local_time();
    
    // Main game logic loop
    while(!done)
    {
        boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
        
        while(now.time_of_day().total_milliseconds() - last.time_of_day().total_milliseconds() < 1000/FAWorld::World::ticksPerSecond)
        {
            boost::this_thread::sleep(boost::posix_time::milliseconds(1));
            now = boost::posix_time::microsec_clock::local_time();
        }

        last = now;
        
        FAWorld::Player* player = world.getPlayer();

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

        if(!noclip && !level[player->mPos.next().first][player->mPos.next().second].passable())
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

    return 0;
}
