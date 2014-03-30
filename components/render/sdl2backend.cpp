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

    SDL_Window* screen;
    SDL_Renderer* renderer;
    
    void init()
    {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &screen, &renderer);

        
        if(screen == NULL)
            printf("Could not create window: %s\n", SDL_GetError());
    }

    void quit()
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(screen);
        SDL_Quit();
    }
    

    void draw()
    {
        SDL_RenderPresent(renderer);
    }

    void drawAt(SDL_Texture* sprite, size_t x, size_t y)
    {
        int width, height;
        SDL_QueryTexture(sprite, NULL, NULL, &width, &height);

        SDL_Rect dest = { x, y, width, height };
        
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
	
    void drawMinPillar(SDL_Surface* s, int x, int y, const Level::MinPillar& pillar, Cel::CelFile& tileset);
    
    RenderLevel* setLevel(const Level::Level& level)
    {
        Cel::CelFile town(level.getTileSetPath());
        
        RenderLevel* retval = new RenderLevel();

        SDL_Surface* levelSprite = createTransparentSurface(((level.width()+level.height()))*32, ((level.width()+level.height()))*16 + 224);

        for(size_t x = 0; x < level.width(); x++)
        {
            for(size_t y = 0; y < level.height(); y++)
            {
                drawMinPillar(levelSprite, (y*(-32)) + 32*x + level.height()*32-32, (y*16) + 16*x, level[x][y], town);
            }
        }

        SDL_SaveBMP(levelSprite, "test.bmp");//TODO: should probably get rid of this at some point, useful for now

        retval->levelSprite = SDL_CreateTextureFromSurface(renderer, levelSprite);
        SDL_FreeSurface(levelSprite);
        
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
        SDL_DestroyTexture((SDL_Texture*)levelSprite);
    }
}
