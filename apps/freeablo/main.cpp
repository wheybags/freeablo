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
}
