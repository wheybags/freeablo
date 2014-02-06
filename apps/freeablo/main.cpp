#include <level/minfile.h>
#include <level/tilfile.h>
#include <level/dunfile.h>

#include <render/render.h>
#include <input/inputmanager.h>

#include "falevelgen/levelgen.h"
#include "falevelgen/random.h"

#include "farender/renderer.h"

#include <boost/date_time/posix_time/posix_time.hpp>

bool done = false;
int lr = 0, ud = 0;
    int32_t x_base = 0, y_base = 0;
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
    // Starts rendering thread
    FARender::Renderer renderer;

    // Starts input thread
    Input::InputManager input(&keyPress, &keyRelease);

    Level::MinFile min("levels/l1data/l1.min");
    Level::TilFile til("levels/l1data/l1.til");
    Level::DunFile dun;

    FALevelGen::FAsrand(time(NULL));
    FALevelGen::generate(100, 100, dun);

    renderer.setLevel(dun, 1);

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

        x_base += lr;
        y_base += ud; 
        
        FARender::RenderState* state = renderer.getFreeState();
        state->mX = x_base;
        state->mY = y_base;

        renderer.setCurrentState(state);
    }
}
