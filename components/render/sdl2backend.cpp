#include "render.h"

#include <iostream>
#include <complex>

#include <SDL.h>

#include "../cel/celfile.h"
#include "../cel/celframe.h"

#include "../level/level.h"

#include <Rocket/Core.h>
#include <Rocket/Core/Input.h>

#include "rocketglue/FAIOFileInterface.h"
#include "rocketglue/SystemInterfaceSDL2.h"
#include "rocketglue/RenderInterfaceSDL2.h"

namespace Render
{
    int32_t WIDTH = 1280;
    int32_t HEIGHT = 960;

    SDL_Window* screen;
    SDL_Renderer* renderer;

    RocketSDL2Renderer* Renderer;
    RocketSDL2SystemInterface* SystemInterface;
    FAIOFileInterface* FileInterface;
    Rocket::Core::Context* Context;
    
    void init(const RenderSettings& settings)
    {
        WIDTH = settings.windowWidth;
        HEIGHT = settings.windowHeight;

        SDL_Init( SDL_INIT_VIDEO );
        screen = SDL_CreateWindow("LibRocket SDL2 test", 20, 20, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        if(screen == NULL)
            printf("Could not create window: %s\n", SDL_GetError());


        SDL_GLContext glcontext = SDL_GL_CreateContext(screen);
        int oglIdx = -1;
        int nRD = SDL_GetNumRenderDrivers();
        for(int i=0; i<nRD; i++)
        {
            SDL_RendererInfo info;
            if(!SDL_GetRenderDriverInfo(i, &info))
            {
                if(!strcmp(info.name, "opengl"))
                {
                    oglIdx = i;
                }
            }
        }

        renderer = SDL_CreateRenderer(screen, oglIdx, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        glMatrixMode(GL_PROJECTION|GL_MODELVIEW);
        glLoadIdentity();
        glOrtho(0, WIDTH, HEIGHT, 0, 0, 1);
    }

    Rocket::Core::Context* initGui()
    {
        Renderer = new RocketSDL2Renderer(renderer, screen);
        SystemInterface = new RocketSDL2SystemInterface();
        FileInterface = new FAIOFileInterface();

        Rocket::Core::SetFileInterface(FileInterface);
        Rocket::Core::SetRenderInterface(Renderer);
        Rocket::Core::SetSystemInterface(SystemInterface);

        if(!Rocket::Core::Initialise())
            fprintf(stderr, "couldn't initialise rocket!");

        Rocket::Core::FontDatabase::LoadFontFace("Delicious-Bold.otf");
        Rocket::Core::FontDatabase::LoadFontFace("Delicious-BoldItalic.otf");
        Rocket::Core::FontDatabase::LoadFontFace("Delicious-Italic.otf");
        Rocket::Core::FontDatabase::LoadFontFace("Delicious-Roman.otf");

        Context = Rocket::Core::CreateContext("default",
            Rocket::Core::Vector2i(640, 480));

        return Context;
    }
	
    void quit()
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(screen);
        SDL_Quit();
    }

    void resize(size_t w, size_t h)
    {
        WIDTH = w;
        HEIGHT = h;
    }

    void updateGuiBuffer()
    {
        Renderer->clearDrawBuffer();
        Context->Render();
    }

    void drawGui()
    {
        Renderer->drawBuffer();
    }

    void draw()
    {
        SDL_RenderPresent(renderer);
    }

    void drawAt(SDL_Texture* sprite, size_t x, size_t y)
    {
        int width, height;
        SDL_QueryTexture(sprite, NULL, NULL, &width, &height);

        SDL_Rect dest = { int(x), int(y), width, height };
        
        SDL_RenderCopy(renderer, sprite, NULL, &dest);
    }
    
    void drawAt(const Sprite& sprite, size_t x, size_t y)
    {
        drawAt((SDL_Texture*)sprite, x, y);
    }

    void drawFrame(SDL_Surface* s, int start_x, int start_y, const Cel::CelFrame& frame);
    SDL_Surface* createTransparentSurface(size_t width, size_t height);
    void clearTransparentSurface(SDL_Surface* s);

    SpriteGroup::SpriteGroup(const std::string& path)
    {
        Cel::CelFile cel(path);
        
        SDL_Surface* s = createTransparentSurface(cel[0].mWidth, cel[0].mHeight);

        for(size_t i = 0; i < cel.numFrames(); i++)
        {
            drawFrame(s, 0, 0, cel[i]);

            mSprites.push_back(SDL_CreateTextureFromSurface(renderer, s));

            clearTransparentSurface(s);
        }

        SDL_FreeSurface(s);

        mAnimLength = cel.animLength();
    }

    void SpriteGroup::destroy()
    {
        for(size_t i = 0; i < mSprites.size(); i++)
            SDL_DestroyTexture((SDL_Texture*)mSprites[i]);
    }
	
    void drawMinPillarTop(SDL_Surface* s, int x, int y, const Level::MinPillar& pillar, Cel::CelFile& tileset);
    void drawMinPillarBase(SDL_Surface* s, int x, int y, const Level::MinPillar& pillar, Cel::CelFile& tileset);
    
    RenderLevel* setLevel(const Level::Level& level)
    {
        Cel::CelFile town(level.getTileSetPath());
        
        RenderLevel* retval = new RenderLevel();

        retval->level = &level;

        SDL_Surface* newPillar = createTransparentSurface(64, 256);
        
        for(size_t i = 0; i < level.minSize()-1; i++)
        {
            clearTransparentSurface(newPillar);
            drawMinPillarTop(newPillar, 0, 0, level.minPillar(i), town);
            retval->minTops[i] = SDL_CreateTextureFromSurface(renderer, newPillar);

            clearTransparentSurface(newPillar);
            drawMinPillarBase(newPillar, 0, 0, level.minPillar(i), town);
            retval->minBottoms[i] = SDL_CreateTextureFromSurface(renderer, newPillar);
        }

        SDL_FreeSurface(newPillar);

        retval->levelWidth = level.width();
        retval->levelHeight = level.height();

        return retval;
    }
    
    void spriteSize(const Sprite& sprite, size_t& w, size_t& h)
    {
        int tmpW, tmpH;
        SDL_QueryTexture((SDL_Texture*)sprite, NULL, NULL, &tmpW, &tmpH);
        w = tmpW;
        h = tmpH;
    }

    void clear()
    {
         SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
         SDL_RenderClear(renderer);
    }

    RenderLevel::~RenderLevel()
    {
        for(std::map<int32_t, Sprite>::iterator it = minTops.begin(); it != minTops.end(); ++it)
            SDL_DestroyTexture((SDL_Texture*)it->second);

        for(std::map<int32_t, Sprite>::iterator it = minBottoms.begin(); it != minBottoms.end(); ++it)
            SDL_DestroyTexture((SDL_Texture*)it->second);
    }
}
