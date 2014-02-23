#include <render/render.h>
#include <input/inputmanager.h>
#include <level/dun.h>

#include "falevelgen/levelgen.h"
#include "falevelgen/random.h"

#include "farender/renderer.h"

#include "faworld/world.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>

bool done = false;
int lr = 0, ud = 0;
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

int main(int argc, char** argv)
{
    size_t level;

    boost::program_options::options_description desc("Options");
    desc.add_options()
        ("help,h", "Print help")
        ("level,l", boost::program_options::value<size_t>(&level)->default_value(0), "Level number to load (0-4)");

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

        if(level > 4)
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

    Level::Dun dun;
    
    if(level > 0) 
    {
        FALevelGen::FAsrand(time(NULL));
        FALevelGen::generate(100, 100, dun);
    }
    else
    {
        Level::Dun sector1("levels/towndata/sector1s.dun");
        Level::Dun sector2("levels/towndata/sector2s.dun");
        Level::Dun sector3("levels/towndata/sector3s.dun");
        Level::Dun sector4("levels/towndata/sector4s.dun");

        dun = Level::Dun::getTown(sector1, sector2, sector3, sector4);
    }

    if(!renderer.setLevel(dun, level))
        return 1;

    FAWorld::World world;

    boost::posix_time::ptime last = boost::posix_time::microsec_clock::local_time();
    
    // Main game logic loop
    while(!done)
    {
        boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
        
        // loop approx 120 times per second (1sec = 1000msec, 1000/120 =~ 8)
        while(now.time_of_day().total_milliseconds() - last.time_of_day().total_milliseconds() < 8)
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
                player->mPos.mNext.first = player->mPos.mCurrent.first + lr;
                player->mPos.mNext.second = player->mPos.mCurrent.second + ud;
            }
        }

        world.update();

        FARender::RenderState* state = renderer.getFreeState();
        
        state->mPos = player->mPos;

        world.fillRenderState(state);

        renderer.setCurrentState(state);
    }

    return 0;
}
