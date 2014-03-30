#include "render.h"

#include <iostream>
#include <complex>

#include <SDL.h>

#include "../cel/celfile.h"
#include "../cel/celframe.h"

#include "../level/level.h"


namespace Render
{
    #define WIDTH 1280
    #define HEIGHT 960
    #define DEPTH 32

    SDL_Surface* screen;
    
    void init()
    {
        SDL_Init(SDL_INIT_VIDEO);
        screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE | SDL_DOUBLEBUF);
    }

    void quit()
    {
        SDL_Quit();
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
	
    void drawMinPillar(SDL_Surface* s, int x, int y, const Level::MinPillar& pillar, Cel::CelFile& tileset);
    
    RenderLevel* setLevel(const Level::Level& level)
    {
        Cel::CelFile town(level.getTileSetPath());
        
        RenderLevel* retval = new RenderLevel();

        retval->levelSprite = SDL_CreateRGBSurface(SDL_HWSURFACE, ((level.width()+level.height()))*32, ((level.width()+level.height()))*16 + 224, screen->format->BitsPerPixel,
                                              screen->format->Rmask,
                                              screen->format->Gmask,
                                              screen->format->Bmask,
                                              screen->format->Amask);

        for(size_t x = 0; x < level.width(); x++)
        {
            for(size_t y = 0; y < level.height(); y++)
            {
                drawMinPillar((SDL_Surface*)retval->levelSprite, (y*(-32)) + 32*x + level.height()*32-32, (y*16) + 16*x, level[x][y], town);
            }
        }

        SDL_SaveBMP((SDL_Surface*)retval->levelSprite, "test.bmp");//TODO: should probably get rid of this at some point, useful for now

        retval->levelWidth = level.width();
        retval->levelHeight = level.height();

        return retval;
    }
    
    void drawLevel(RenderLevel* level, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist)
    {
        int16_t xPx1 = -((y1*(-32)) + 32*x1 + level->levelWidth*32) +WIDTH/2;
        int16_t yPx1 = -((y1*16) + (16*x1) +160) + HEIGHT/2;

        int16_t xPx2 = -((y2*(-32)) + 32*x2 + level->levelWidth*32) +WIDTH/2;
        int16_t yPx2 = -((y2*16) + (16*x2) +160) + HEIGHT/2;

        level->levelX = xPx1 + ((((float)(xPx2-xPx1))/100.0)*(float)dist);
        level->levelY = yPx1 + ((((float)(yPx2-yPx1))/100.0)*(float)dist);

        //TODO clean up the magic numbers here, and elsewhere in this file
        drawAt(level->levelSprite, level->levelX, level->levelY);
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
        SDL_FreeSurface((SDL_Surface*)levelSprite);
    }
}
