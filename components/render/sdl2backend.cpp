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


        SDL_GL_CreateContext(screen);
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
            Rocket::Core::Vector2i(WIDTH, HEIGHT));

        return Context;
    }
	
    void quit()
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(screen);
        SDL_Quit();
    }

    bool resized = false;

    void resize(size_t w, size_t h)
    {
        WIDTH = w;
        HEIGHT = h;
        resized = true;
    }

    void updateGuiBuffer()
    {
        if(resized)
        {
            Context->SetDimensions(Rocket::Core::Vector2i(WIDTH, HEIGHT));
            resized = false;
        }
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

    #define BPP 4
    #define DEPTH 32

    void clearTransparentSurface(SDL_Surface* s)
    {
        SDL_FillRect(s, NULL, SDL_MapRGBA(s->format, 0, 0, 0, 0)); 
    }

    SDL_Surface* createTransparentSurface(size_t width, size_t height)
    {
         SDL_Surface* s; 
        
        // SDL y u do dis
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
            s = SDL_CreateRGBSurface(0, width, height, DEPTH, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
        #else
            s = SDL_CreateRGBSurface(0, width, height, DEPTH, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
        #endif

        clearTransparentSurface(s);

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
        for(size_t x = 0; x < frame.mWidth; x++)
        {
            for(size_t y = 0; y < frame.mHeight; y++)
            {
                if(frame[x][y].visible)
                    setpixel(s, start_x+x, start_y+y, frame[x][y]);
            }
        }
    }

    void drawMinTile(SDL_Surface* s, Cel::CelFile& f, int x, int y, int16_t l, int16_t r)
    {
        if(l != -1)
            drawFrame(s, x, y, f[l]);

        if(r != -1)
            drawFrame(s, x+32, y, f[r]);
    }

    void drawMinPillar(SDL_Surface* s, int x, int y, const Level::MinPillar& pillar, Cel::CelFile& tileset, bool top)
    {
        // compensate for maps using 5-row min files
        if(pillar.size() == 10)
            y += 3*32;

        size_t i, lim;

        if(top)
        {
            i = 0;
            lim = pillar.size() - 2;
        }
        else
        {
            i = pillar.size() - 2;
            lim = pillar.size();
            y += i*16;
        }

        // Each iteration draw one row of the min
        for(; i < lim; i+=2)
        {
            int16_t l = (pillar[i]&0x0FFF)-1;
            int16_t r = (pillar[i+1]&0x0FFF)-1;
            
            drawMinTile(s, tileset, x, y, l, r);
        
            y += 32; // down 32 each row
        }
    }

    void drawMinPillarTop(SDL_Surface* s, int x, int y, const Level::MinPillar& pillar, Cel::CelFile& tileset)
    {
        drawMinPillar(s, x, y, pillar, tileset, true);
    }

    void drawMinPillarBase(SDL_Surface* s, int x, int y, const Level::MinPillar& pillar, Cel::CelFile& tileset)
    {
        drawMinPillar(s, x, y, pillar, tileset, false);
    }

    void drawAt(RenderLevel* level, const Sprite& sprite, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist)
    {
        size_t w, h;
        spriteSize(sprite, w, h);

        int32_t xPx1 = ((y1*(-32)) + 32*x1 + level->levelWidth*32) + level->levelX - w/2;
        int32_t yPx1 = ((y1*16) + (16*x1) +160) + level->levelY;

        int32_t xPx2 = ((y2*(-32)) + 32*x2 + level->levelWidth*32) + level->levelX - w/2;
        int32_t yPx2 = ((y2*16) + (16*x2) +160) + level->levelY;

        int32_t x = xPx1 + ((((float)(xPx2-xPx1))/100.0)*(float)dist);
        int32_t y = yPx1 + ((((float)(yPx2-yPx1))/100.0)*(float)dist);

        drawAt(sprite, x, y);
    }

    std::pair<size_t, size_t> getClickedTile(RenderLevel* level, size_t x, size_t y)
    {
        // Position on the map in pixels
        int32_t flatX = x - level->levelX;
        int32_t flatY = y - level->levelY;

        // position on the map divided into 32x16 flat blocks
        // every second one of these blocks is centred on an isometric
        // block centre, the others are centred on isometric block corners
        int32_t flatGridX = (flatX+16) / 32;
        int32_t flatGridY = (flatY+8) / 16;
        
        // origin position (in flat grid coords) for the first line (isometric y = 0)
        int32_t flatOriginPosX = level->levelHeight;
        int32_t flatOriginPosY = 15;

        // when a flat grid box is clicked that does not centre on an isometric block, work out which
        // isometric quadrant of that box was clicked, then adjust flatGridPos accordingly
        if((flatGridX % 2 == 1 && flatGridY % 2 == 1) || (flatGridX % 2 == 0 && flatGridY % 2 == 0))
        {
            
            // origin of current flat grid box
            int32_t baseX = 32*flatGridX - 16;
            int32_t baseY = 16*flatGridY - 8;
            
            // position within grid box
            int32_t blockPosX = flatX - baseX;
            int32_t blockPosY = flatY - baseY;

            if(blockPosY*2 > blockPosX)
            {
                if(blockPosX < (15-blockPosY)*2)
                    flatGridX--;
                else
                    flatGridY++;
            }
            else
            {
                if(blockPosX < (15-blockPosY)*2)
                    flatGridY--;
                else
                    flatGridX++;
            }
        }
        
        // flatOrigin adjusted for the current y value
        int32_t lineOriginPosX = flatOriginPosX + ((flatGridX - flatOriginPosX) - (flatGridY - flatOriginPosY))/2;
        int32_t lineOriginPosY = flatOriginPosY - (-(flatGridX - flatOriginPosX) -( flatGridY - flatOriginPosY))/2;

        int32_t isoPosX = flatGridX - lineOriginPosX;
        int32_t isoPosY = flatGridY - lineOriginPosY;

        return std::make_pair(isoPosX, isoPosY);
    }

    void drawLevelHelper(RenderLevel* level, std::map<int32_t, Sprite>& minMap, int32_t x, int32_t y)
    {
        if((size_t)x < level->level->width() && (size_t)y < level->level->height())
        {
            size_t index = level->level->operator[](x)[y].index();
            int32_t xCoord = (y*(-32)) + 32*x + level->level->height()*32-32 +level->levelX;
            int32_t yCoord = (y*16) + 16*x + level->levelY;

            if(xCoord >= -64 && xCoord <=  WIDTH  && yCoord >= -256 && yCoord <= HEIGHT && minMap.find(index) != minMap.end())
                drawAt(minMap[index], xCoord, yCoord);
        }
    }

    void drawLevel(RenderLevel* level, LevelObjects& objs, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist)
    {
        int16_t xPx1 = -((y1*(-32)) + 32*x1 + level->levelWidth*32) +WIDTH/2;
        int16_t yPx1 = -((y1*16) + (16*x1) +160) + HEIGHT/2;

        int16_t xPx2 = -((y2*(-32)) + 32*x2 + level->levelWidth*32) +WIDTH/2;
        int16_t yPx2 = -((y2*16) + (16*x2) +160) + HEIGHT/2;

        level->levelX = xPx1 + ((((float)(xPx2-xPx1))/100.0)*(float)dist);
        level->levelY = yPx1 + ((((float)(yPx2-yPx1))/100.0)*(float)dist);

        //TODO clean up the magic numbers here, and elsewhere in this file
        
        for(size_t x = 0; x < level->level->width(); x++)
        {
            for(size_t y = 0; y < level->level->height(); y++)
            {
                drawLevelHelper(level, level->minBottoms, x+1, y+1);
            }
        }

        for(size_t x = 0; x < level->level->width(); x++)
        {
            for(size_t y = 0; y < level->level->height(); y++)
            {
                if(objs[x][y].sprite)
                    drawAt(level, objs[x][y].sprite, x, y, objs[x][y].x2, objs[x][y].y2, objs[x][y].dist);

                drawLevelHelper(level, level->minTops, x, y);
            }
        }
    }
}
