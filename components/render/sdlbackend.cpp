#include "render.h"

#include <iostream>

#include <SDL.h>

#include "../cel/celfile.h"
#include "../cel/celframe.h"

#include "../level/minfile.h"
#include "../level/tilfile.h"
#include "../level/dunfile.h"


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

    std::vector<Sprite> loadGroup(const std::string& path)
    {
        Cel::CelFile cel(path);
        
        std::vector<Sprite> sprites;

        for(size_t i = 0; i < cel.numFrames(); i++)
        {
            SDL_Surface* s = createTransparentSurface(cel[i].width, cel[i].height);
            drawFrame(s, 0, 0, cel[i]);

            sprites.push_back(s);
        }

        return sprites;
    }


    SDL_Surface** tileset = NULL;

    SDL_Surface* getSprite(Cel::CelFile& f, size_t index)
    {
        if(tileset == NULL)
        {
            size_t size = f.numFrames();

            tileset = new SDL_Surface*[size];
            
            for(size_t i = 0; i < size; i++)
                tileset[i] = NULL;
        }
        
        if(tileset[index] != NULL)
            return tileset[index];
        
        Cel::CelFrame frame = f[index];

        SDL_Surface* s = createTransparentSurface(frame.width, frame.height);

        drawFrame(s, 0, 0, frame);

        
        tileset[index] = s;

        return s;
    }

    void blit(SDL_Surface* from, SDL_Surface* to, int x, int y)
    {
        SDL_Rect rcDest = { x, y, 0, 0 };
        SDL_BlitSurface (from , NULL, to, &rcDest );
    }

    int x_base = WIDTH/2, y_base = 0;

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

    void drawTilBlock(SDL_Surface* to, int x, int y, const Level::TilFile& til, size_t index, const Level::MinFile& min, Cel::CelFile& tileset)
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

    void setLevel(const std::string& tilesetPath, const Level::DunFile& dun, const Level::TilFile& til, const Level::MinFile& min)
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

        SDL_SaveBMP(level, "test.bmp");
    }

    void drawLevel(int32_t x, int32_t y)
    {
        blit(level, screen, x, y);
    }

    void clear()
    {
        SDL_FillRect(screen,NULL, SDL_MapRGB( screen->format, 0, 0, 255)); 
    }
}
