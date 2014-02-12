#include <render/render.h>
#include <input/inputmanager.h>
#include <level/dunfile.h>

#include "falevelgen/levelgen.h"
#include "falevelgen/random.h"

#include "farender/renderer.h"

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

    Level::DunFile dun;
    
    if(level > 0) 
    {
        FALevelGen::FAsrand(time(NULL));
        FALevelGen::generate(100, 100, dun);
    }
    else
    {
        Level::DunFile sector1("levels/towndata/sector1s.dun");
        Level::DunFile sector2("levels/towndata/sector2s.dun");
        Level::DunFile sector3("levels/towndata/sector3s.dun");
        Level::DunFile sector4("levels/towndata/sector4s.dun");

        dun = Level::DunFile::getTown(sector1, sector2, sector3, sector4);
    }

    if(!renderer.setLevel(dun, level))
        return 1;

    boost::posix_time::ptime last = boost::posix_time::microsec_clock::local_time();
    
    int32_t x = 0, y = 0;
    int32_t dirX = 0, dirY = 0;
    bool moving = false;
    size_t dist = 0;
     
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

        if(!moving)
        {
            dirX = lr;
            dirY = ud;

            if(dirX || dirY)
            {
                moving = true;
                dist = 0;
            }
        }
        // Smooth movement
        else
        {
            dist += 2;
            if(dist >= 100)
            {
                x = x + dirX;
                y = y + dirY;
                moving = false;
                dirX = 0;
                dirY = 0;
                dist = 0;
            }
        }

        FARender::RenderState* state = renderer.getFreeState();
        state->mX1 = x;
        state->mY1 = y;

        state->mX2 = x + dirX;
        state->mY2 = y + dirY;

        state->mDist = dist;

        renderer.setCurrentState(state);
    }

    return 0;
}
