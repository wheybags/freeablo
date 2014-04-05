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

        retval->level = &level;

        for(size_t x = 0; x < level.width(); x++)
        {
            for(size_t y = 0; y < level.height(); y++)
            {
                if(retval->minPillars.find(retval->level->operator[](x)[y].index()) == retval->minPillars.end())
                {
                    SDL_Surface* newPillar = createTransparentSurface(64, 256);
                    drawMinPillar(newPillar, 0, 0, level[x][y], town);

                    retval->minPillars[level[x][y].index()] = newPillar;
                }
            }
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
        for(std::map<int32_t, Sprite>::iterator it = minPillars.begin(); it != minPillars.end(); ++it)
            SDL_FreeSurface((SDL_Surface*)it->second);
    }
}
