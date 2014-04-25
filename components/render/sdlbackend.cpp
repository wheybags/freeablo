#include "render.h"

#include <iostream>
#include <complex>

#include <SDL.h>

#include "../cel/celfile.h"
#include "../cel/celframe.h"

#include "../level/level.h"


namespace Render
{
    int32_t WIDTH = 1280;
    int32_t HEIGHT = 960;
    #define DEPTH 32

    SDL_Surface* screen;
    
    void init()
    {
        SDL_Init(SDL_INIT_VIDEO);
        screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);
    }

    void quit()
    {
        SDL_Quit();
    }

    void resize(size_t w, size_t h)
    {
        WIDTH = w;
        HEIGHT = h;
        screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);
    }

    void draw()
    {
        SDL_Flip(screen); 
    }

    void drawAt(SDL_Surface* sprite, size_t x, size_t y)
    {
        SDL_Rect rcDest = { x, y, 0, 0 };
        SDL_BlitSurface (sprite , NULL, screen, &rcDest );
    }
    
    void drawAt(const Sprite& sprite, size_t x, size_t y)
    {
        drawAt((SDL_Surface*)sprite, x, y);
    }

    void drawFrame(SDL_Surface* s, int start_x, int start_y, const Cel::CelFrame& frame);
    SDL_Surface* createTransparentSurface(size_t width, size_t height);

    SpriteGroup::SpriteGroup(const std::string& path)
    {
        Cel::CelFile cel(path);

        for(size_t i = 0; i < cel.numFrames(); i++)
        {
            SDL_Surface* s = createTransparentSurface(cel[i].mWidth, cel[i].mHeight);
            drawFrame(s, 0, 0, cel[i]);

            mSprites.push_back(s);
        }

        mAnimLength = cel.animLength();
    }

    void SpriteGroup::destroy()
    {
        for(size_t i = 0; i < mSprites.size(); i++)
            SDL_FreeSurface((SDL_Surface*)mSprites[i]);
    }
	
    void drawMinPillarTop(SDL_Surface* s, int x, int y, const Level::MinPillar& pillar, Cel::CelFile& tileset);
    void drawMinPillarBase(SDL_Surface* s, int x, int y, const Level::MinPillar& pillar, Cel::CelFile& tileset);
    
    RenderLevel* setLevel(const Level::Level& level)
    {
        Cel::CelFile town(level.getTileSetPath());
        
        RenderLevel* retval = new RenderLevel();

        retval->level = &level;

        for(size_t i = 0; i < level.minSize()-1; i++)
        {
                    SDL_Surface* newPillar = createTransparentSurface(64, 256);
                    drawMinPillarTop(newPillar, 0, 0, level.minPillar(i), town);
                    retval->minTops[i] = newPillar;

                    newPillar = createTransparentSurface(64, 256);
                    drawMinPillarBase(newPillar, 0, 0, level.minPillar(i), town);
                    retval->minBottoms[i] = newPillar;
        }

        retval->levelWidth = level.width();
        retval->levelHeight = level.height();

        return retval;
    }
    
    void spriteSize(const Sprite& sprite, size_t& w, size_t& h)
    {
        w = ((SDL_Surface*)sprite)->w;
        h = ((SDL_Surface*)sprite)->h;
    }

    void clear()
    {
        SDL_FillRect(screen,NULL, SDL_MapRGB( screen->format, 0, 0, 255)); 
    }

    RenderLevel::~RenderLevel()
    {
        for(std::map<int32_t, Sprite>::iterator it = minTops.begin(); it != minTops.end(); ++it)
            SDL_FreeSurface((SDL_Surface*)it->second);

        for(std::map<int32_t, Sprite>::iterator it = minBottoms.begin(); it != minBottoms.end(); ++it)
            SDL_FreeSurface((SDL_Surface*)it->second);
    }
}
