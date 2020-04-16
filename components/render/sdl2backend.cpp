#include "nuklear_sdl_gl3.h"
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

    void init(const std::string& title, const RenderSettings& settings, NuklearGraphicsContext& nuklearGraphics, nk_context* nk_ctx)
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

        if (nk_ctx)
        {
            nuklearGraphics = {};
            nk_sdl_device_create(nuklearGraphics.dev, *renderInstance);
        }
    }

    const std::string& getWindowTitle() { return windowTitle; }
    void setWindowTitle(const std::string& title) { SDL_SetWindowTitle(screen, title.c_str()); }

    void destroyNuklearGraphicsContext(NuklearGraphicsContext& nuklearGraphics)
    {
        nk_font_atlas_clear(&nuklearGraphics.atlas);
        nk_sdl_device_destroy(nuklearGraphics.dev);
    }

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

    void drawGui(NuklearFrameDump& dump) { nk_sdl_render_dump(dump, screen, *mainCommandQueue); }

    void draw()
    {
        mainCommandQueue->cmdPresent();
        mainCommandQueue->end();
        mainCommandQueue->submit();
        mainCommandQueue->begin();
    }

    void clear(int r, int g, int b)
    {
        mainCommandQueue->cmdClearCurrentFramebuffer(Color(float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f, 1.0f), true);
    }
}
