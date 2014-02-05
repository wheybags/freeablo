#include <level/minfile.h>
#include <level/tilfile.h>
#include <level/dunfile.h>

#include <render/render.h>
#include <input/inputmanager.h>

#include "falevelgen/levelgen.h"
#include "falevelgen/random.h"

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
    Render::init(); 

    Input::InputManager input(&keyPress, &keyRelease);

    Level::MinFile min("levels/l1data/l1.min");
    Level::TilFile til("levels/l1data/l1.til");
    Level::DunFile dun;

    FALevelGen::FAsrand(time(NULL));
    FALevelGen::generate(100, 100, dun);

    Render::setLevel("levels/l1data/l1.cel", dun, til, min);
    
    int32_t x_base = 100, y_base = 100;

    while(!done)
    {
        x_base += lr;
        y_base += ud; 

        Render::drawLevel(x_base,y_base);
        Render::draw();
    }
}
