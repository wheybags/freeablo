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

        retval->level = &level;

        SDL_Surface* newPillar = createTransparentSurface(64, 256);
        
        for(size_t i = 0; i < level.minSize(); i++)
        {
            clearTransparentSurface(newPillar);
            drawMinPillar(newPillar, 0, 0, level.minPillar(i), town);

            retval->minPillars[i] = SDL_CreateTextureFromSurface(renderer, newPillar);
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
        for(std::map<int32_t, Sprite>::iterator it = minPillars.begin(); it != minPillars.end(); ++it)
            SDL_DestroyTexture((SDL_Texture*)it->second);
    }
}
