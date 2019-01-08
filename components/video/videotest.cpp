#include "video.h"
// Stop SDL from messing with MSVC main()
// https://wiki.libsdl.org/SDL_SetMainReady
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return EXIT_FAILURE;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow(
        "VideoTest", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 960, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN /*|SDL_WINDOW_FULLSCREEN*/);
    if (!window)
    {
        return EXIT_FAILURE;
    }
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    Video::init();

    Video::playVideo(argv[1]);

    while (Video::isVideoPlaying())
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    Video::stopVideo();
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                        case SDLK_q:
                            Video::stopVideo();
                            break;
                        default:
                            break;
                    }
                    break;
            }
        }
        Video::update();
    }

    Video::quit();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
