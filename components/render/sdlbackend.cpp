#include "render.h"

#include <iostream>
#include <complex>

#include <SDL.h>

#include "../cel/celfile.h"
#include "../cel/celframe.h"

#include "../level/min.h"
#include "../level/tileset.h"
#include "../level/dun.h"


namespace Render
{
    #define WIDTH 1280
    #define HEIGHT 960
    #define BPP 4
    #define DEPTH 32

    SDL_Surface* screen;
    
    void init()
    {
        screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE | SDL_DOUBLEBUF);
    }

    void draw()
    {
        SDL_Flip(screen); 
    }

    void drawAt(const Sprite& sprite, size_t x, size_t y)
    {
        SDL_Rect rcDest = { x, y, 0, 0 };
        SDL_BlitSurface ((SDL_Surface*)sprite , NULL, screen, &rcDest );
    }

    SDL_Surface* createTransparentSurface(size_t width, size_t height)
    {
         SDL_Surface* s; 
        
        // SDL y u do dis
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
            s = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, DEPTH, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
        #else
            s = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, DEPTH, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
        #endif

        SDL_FillRect(s, NULL, SDL_MapRGBA(s->format, 0, 0, 0, 0)); 

        return s;
    }

    void setpixel(SDL_Surface *s, int x, int y, Cel::Colour c)
    {
        y = y*s->pitch/BPP;

        Uint32 *pixmem32;
        Uint32 colour;  
     
        colour = SDL_MapRGB( s->format, c.r, c.g, c.b );
      
        pixmem32 = (Uint32*) s->pixels  + y + x;
        *pixmem32 = colour;
    }

    void drawFrame(SDL_Surface* s, int start_x, int start_y, const Cel::CelFrame& frame)
    {
        for(int x = 0; x < frame.width; x++)
        {
            for(int y = 0; y < frame.height; y++)
            {
                if(frame[x][y].visible)
                    setpixel(s, start_x+x, start_y+y, frame[x][y]);
            }
        }
    }

    SpriteGroup::SpriteGroup(const std::string& path)
    {
        Cel::CelFile cel(path);

        for(size_t i = 0; i < cel.numFrames(); i++)
        {
            SDL_Surface* s = createTransparentSurface(cel[i].width, cel[i].height);
            drawFrame(s, 0, 0, cel[i]);

            mSprites.push_back(s);
        }
    }

    SpriteGroup::~SpriteGroup()
    {
        for(size_t i = 0; i < mSprites.size(); i++)
            SDL_FreeSurface((SDL_Surface*)mSprites[i]);
    }
    
    SDL_Surface** tileset = NULL;

    void blit(SDL_Surface* from, SDL_Surface* to, int x, int y)
    {
        SDL_Rect rcDest = { x, y, 0, 0 };
        SDL_BlitSurface (from , NULL, to, &rcDest );
    }

    void drawMinTile(SDL_Surface* s, Cel::CelFile& f, int x, int y, int16_t l, int16_t r)
    {
        if(l != -1)
        {
            drawFrame(s, x, y, f[l]);
            
            #ifdef CEL_DEBUG
                /*TTF_Font* font = TTF_OpenFont("FreeMonoBold.ttf", 8);
                SDL_Color foregroundColor = { 0, 0, 0 }; 
                SDL_Color backgroundColor = { 255, 255, 255 };
                SDL_Surface* textSurface = TTF_RenderText_Shaded(font, SSTR(l).c_str(), foregroundColor, backgroundColor);
               
                blit(textSurface, s, x, y);

                SDL_FreeSurface(textSurface);
                TTF_CloseFont(font);*/
            #endif
        }
        if(r != -1)
        {
            drawFrame(s, x+32, y, f[r]);

            #ifdef CEL_DEBUG
                /*TTF_Font* font = TTF_OpenFont("FreeMonoBold.ttf", 8);
                SDL_Color foregroundColor = { 255, 0, 0 }; 
                SDL_Color backgroundColor = { 255, 255, 255 };
                SDL_Surface* textSurface = TTF_RenderText_Shaded(font, SSTR(r).c_str(), foregroundColor, backgroundColor);
               
                blit(textSurface, s, x+32, y);

                SDL_FreeSurface(textSurface);
                TTF_CloseFont(font);*/
           #endif
        }
    }

    void drawMinPillar(SDL_Surface* s, int x, int y, const Level::MinPillar& pillar, Cel::CelFile& tileset)
    {
        // Each iteration draw one row of the min
        for(int i = 0; i < pillar.size(); i+=2)
        {
            int16_t l = (pillar[i]&0x0FFF)-1;
            int16_t r = (pillar[i+1]&0x0FFF)-1;
            
            drawMinTile(s, tileset, x, y, l, r);
        
            y += 32; // down 32 each row
        }
    }

    std::map<size_t, SDL_Surface*> tilCache;

    void drawTilBlock(SDL_Surface* to, int x, int y, const Level::TileSet& til, size_t index, const Level::Min& min, Cel::CelFile& tileset)
    {
        SDL_Surface* s;

        if(tilCache.count(index))
            s = tilCache[index];

        else
        {
            s = createTransparentSurface(128, 288);
            
            if(index < til.size())
            {
                drawMinPillar(s, 32,  0, min
                    [til
                    [index]
                    [0]],
                     tileset);
                drawMinPillar(s,  0, 16, min[til[index][2]], tileset);
                drawMinPillar(s, 64, 16, min[til[index][1]], tileset);
                drawMinPillar(s, 32, 32, min[til[index][3]], tileset);
            }
            else
            {
                std::cout << "ERR" <<std::endl;
            }

            tilCache[index] = s;
        }
        
        blit(s, to, x, y);
    }

    SDL_Surface* level = NULL;
    int32_t levelWidth, levelHeight;

    void setLevel(const std::string& tilesetPath, const Level::Dun& dun, const Level::TileSet& til, const Level::Min& min)
    {
        Cel::CelFile town(tilesetPath);

        level = SDL_CreateRGBSurface(SDL_HWSURFACE, ((dun.mWidth+dun.mHeight))*64, ((dun.mWidth+dun.mHeight))*32 + 224, screen->format->BitsPerPixel,
                                              screen->format->Rmask,
                                              screen->format->Gmask,
                                              screen->format->Bmask,
                                              screen->format->Amask);

        int x_shift = dun.mHeight*64 - 64;

        for(int x = 0; x < dun.mWidth; x++)
        {
            for(int y = 0; y < dun.mHeight; y++)
            {
                if(dun[x][y] != 0)
                    drawTilBlock(level, (y*(-64)) + 64*x + x_shift, (y*32) + 32*x, til, dun[x][y]-1, min, town);
            }
        }

        for(std::map<size_t, SDL_Surface*>::iterator it = tilCache.begin(); it != tilCache.end(); ++it)
        {
            SDL_FreeSurface((*it).second);
        }

        tilCache.clear();

        levelWidth = dun.mWidth;
        levelHeight = dun.mHeight;

        SDL_SaveBMP(level, "test.bmp");
    }
    
    int32_t levelX, levelY;

    void drawLevel(int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist)
    {
        clear();
        int16_t xPx1 = -((y1*(-32)) + 32*x1 + levelWidth*64) +WIDTH/2;
        int16_t yPx1 = -((y1*16) + (16*x1) +160) + HEIGHT/2;

        int16_t xPx2 = -((y2*(-32)) + 32*x2 + levelWidth*64) +WIDTH/2;
        int16_t yPx2 = -((y2*16) + (16*x2) +160) + HEIGHT/2;

        int16_t x = xPx1 + ((((float)(xPx2-xPx1))/100.0)*(float)dist);
        int32_t y = yPx1 + ((((float)(yPx2-yPx1))/100.0)*(float)dist);

        //TODO clean up the magic numbers here, and elsewhere in this file
        blit(level, screen, x, y);

        levelX = x;
        levelY = y;
    }
    
    void drawAt(const Sprite& sprite, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist)
    {
        int32_t xPx1 = ((y1*(-32)) + 32*x1 + levelWidth*64) + levelX -((SDL_Surface*)sprite)->w/2;
        int32_t yPx1 = ((y1*16) + (16*x1) +160) + levelY;

        int32_t xPx2 = ((y2*(-32)) + 32*x2 + levelWidth*64) + levelX -((SDL_Surface*)sprite)->w/2;
        int32_t yPx2 = ((y2*16) + (16*x2) +160) + levelY;

        int32_t x = xPx1 + ((((float)(xPx2-xPx1))/100.0)*(float)dist);
        int32_t y = yPx1 + ((((float)(yPx2-yPx1))/100.0)*(float)dist);

        drawAt(sprite, x, y);
    }

    void clear()
    {
        SDL_FillRect(screen,NULL, SDL_MapRGB( screen->format, 0, 0, 255)); 
    }
}
