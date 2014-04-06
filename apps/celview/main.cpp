#include <iostream>

#include <render/render.h>
#include <input/inputmanager.h>

bool done = false;
size_t celIndex = 0;
size_t max;
void keyPress(Input::Key key)
{
    switch(key)
    {
        case Input::KEY_UP:
            if(celIndex < max-1)
                celIndex++;
            break;
        case Input::KEY_DOWN:
            if(celIndex > 0)
                celIndex--;
            break;
        case Input::KEY_q:
            done = true;
            break;
        default:
            break;
    }

    std::cout << "frame " << celIndex+1 << "/" << max << std::endl;
}

int main(int, char** argv)
{
    Render::init(); 
    Input::InputManager input(&keyPress, NULL, NULL, NULL, NULL);

    Render::SpriteGroup cel(argv[1]);
    max = cel.size();

    while(!done)
    {
        input.poll();
        input.processInput();

        Render::clear();
        Render::drawAt(cel[celIndex], 0, 0);
        Render::draw();
    }
}
