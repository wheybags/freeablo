#include "render.h"
#include <SDL.h>
#include <fa_nuklear.h>
#include <faio/fafileobject.h>
#include <misc/assert.h>
#include <misc/savePNG.h>
#include <render/buffer.h>
#include <render/commandqueue.h>
#include <render/pipeline.h>
#include <render/renderinstance.h>
#include <render/texture.h>
#include <render/vertexarrayobject.h>

#if defined(WIN32) || defined(_WIN32)
extern "C" {
__declspec(dllexport) unsigned NvOptimusEnablement = 0x00000001;
}
extern "C" {
__declspec(dllexport) unsigned AmdPowerXpressRequestHighPerformance = 0x00000001;
}
#else
extern "C" {
int NvOptimusEnablement = 1;
}
extern "C" {
int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

namespace Render
{
    int32_t WIDTH = 1280;
    int32_t HEIGHT = 960;

    SDL_Window* screen;
    RenderInstance* renderInstance = nullptr;
    RenderInstance* mainRenderInstance = nullptr;
    CommandQueue* mainCommandQueue = nullptr;

    std::string windowTitle;

    void init(const std::string& title, const RenderSettings& settings)
    {
        WIDTH = settings.windowWidth;
        HEIGHT = settings.windowHeight;
        int flags = SDL_WINDOW_OPENGL;
        windowTitle = title;

        if (settings.fullscreen)
        {
            flags |= SDL_WINDOW_FULLSCREEN;
        }
        else
        {
            flags |= SDL_WINDOW_RESIZABLE;
        }

        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
        screen = SDL_CreateWindow(title.c_str(), 20, 20, WIDTH, HEIGHT, flags);
        if (screen == nullptr)
            printf("Could not create window: %s\n", SDL_GetError());

        renderInstance = RenderInstance::createRenderInstance(RenderInstance::Type::OpenGL, *screen);
        mainCommandQueue = renderInstance->createCommandQueue().release();
        mainCommandQueue->begin();

        mainRenderInstance = renderInstance;

        // Update screen with/height, as starting full screen window in
        // Windows does not trigger a SDL_WINDOWEVENT_RESIZED event.
        SDL_GetWindowSize(screen, &WIDTH, &HEIGHT);

        clear(0, 0, 0);
        draw();
    }

    const std::string& getWindowTitle() { return windowTitle; }
    void setWindowTitle(const std::string& title) { SDL_SetWindowTitle(screen, title.c_str()); }

    void quit()
    {
        delete mainCommandQueue;
        delete renderInstance;
        SDL_DestroyWindow(screen);
        SDL_Quit();
    }

    void resize(size_t w, size_t h)
    {
        WIDTH = w;
        HEIGHT = h;

        renderInstance->onWindowResized(WIDTH, HEIGHT);
    }

    RenderSettings getWindowSize()
    {
        RenderSettings settings;
        settings.windowWidth = WIDTH;
        settings.windowHeight = HEIGHT;
        return settings;
    }

    void draw()
    {
        mainCommandQueue->cmdPresent();
        mainCommandQueue->end();
        mainCommandQueue->submit();
        mainCommandQueue->begin();
    }

    void clear(int r, int g, int b) { mainCommandQueue->cmdClearFramebuffer(Color(float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f, 1.0f), false); }
}
