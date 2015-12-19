#include "render.h"

#include <iostream>
#include <complex>

#include <SDL.h>
#include <SDL_image.h>

#include "../cel/celfile.h"
#include "../cel/celframe.h"



#include "../level/level.h"
#include <misc/stringops.h>
#include <misc/savePNG.h>
#include <faio/faio.h>

#include <Rocket/Core.h>
#include <Rocket/Core/Input.h>

#include "rocketglue/FAIOFileInterface.h"
#include "rocketglue/SystemInterfaceSDL2.h"
#include "rocketglue/RenderInterfaceSDL2.h"

#include <misc/boost_python.h>
#include <Rocket/Core/Python/Python.h>

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
        int flags = SDL_WINDOW_OPENGL;

        if (settings.fullscreen)
        {
            flags |= SDL_WINDOW_FULLSCREEN;
        }
        else
        {
            flags |= SDL_WINDOW_RESIZABLE;
        }

        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
        screen = SDL_CreateWindow("LibRocket SDL2 test", 20, 20, WIDTH, HEIGHT, flags);
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

    bool import(const std::string& name)
    {
        PyObject* module = PyImport_ImportModule(name.c_str());
        if (!module)
        {
            PyErr_Print();
            return false;
        }

        Py_DECREF(module);
        return true;
    }

    Rocket::Core::Context* initGui(std::function<bool(Rocket::Core::TextureHandle&, Rocket::Core::Vector2i&, const Rocket::Core::String&)> loadTextureFunc,
                                   std::function<bool(Rocket::Core::TextureHandle&, const Rocket::Core::byte*, const Rocket::Core::Vector2i&)> generateTextureFunc,
                                   std::function<void(Rocket::Core::TextureHandle)> releaseTextureFunc)
    {
        #ifdef WIN32
            Py_SetPythonHome("Python27");
        #endif
        Py_Initialize();

        #ifdef WIN32
            PyRun_SimpleString("import sys\nsys.path.append('.')");
        #endif
        
        // add our python libs to path
        PyRun_SimpleString("import sys\nsys.path.append('./resources/python')");

        // Pull in the Rocket Python module.
        import("rocket");

        Renderer = new RocketSDL2Renderer(renderer, screen, loadTextureFunc, generateTextureFunc, releaseTextureFunc);
        SystemInterface = new RocketSDL2SystemInterface();
        FileInterface = new FAIOFileInterface();

        Rocket::Core::SetFileInterface(FileInterface);
        Rocket::Core::SetRenderInterface(Renderer);
        Rocket::Core::SetSystemInterface(SystemInterface);

        if(!Rocket::Core::Initialise())
            fprintf(stderr, "couldn't initialise rocket!");

        Rocket::Core::FontDatabase::LoadFontFace("resources/fonts/Freeablo/FreeabloGold16.fnt");
        Rocket::Core::FontDatabase::LoadFontFace("resources/fonts/Freeablo/FreeabloGold24.fnt");
        Rocket::Core::FontDatabase::LoadFontFace("resources/fonts/Freeablo/FreeabloGold30.fnt");
        Rocket::Core::FontDatabase::LoadFontFace("resources/fonts/Freeablo/FreeabloGold42.fnt");

        Rocket::Core::FontDatabase::LoadFontFace("resources/fonts/Freeablo/FreeabloSilver16.fnt");
        Rocket::Core::FontDatabase::LoadFontFace("resources/fonts/Freeablo/FreeabloSilver24.fnt");
        Rocket::Core::FontDatabase::LoadFontFace("resources/fonts/Freeablo/FreeabloSilver30.fnt");
        Rocket::Core::FontDatabase::LoadFontFace("resources/fonts/Freeablo/FreeabloSilver42.fnt");

        Rocket::Core::FontDatabase::LoadFontFace("resources/fonts/FreeMono/FreeMonoBoldOblique.ttf");
        Rocket::Core::FontDatabase::LoadFontFace("resources/fonts/FreeMono/FreeMonoBold.ttf");
        Rocket::Core::FontDatabase::LoadFontFace("resources/fonts/FreeMono/FreeMonoOblique.ttf");
        Rocket::Core::FontDatabase::LoadFontFace("resources/fonts/FreeMono/FreeMono.ttf");

        Context = Rocket::Core::CreateContext("default",
            Rocket::Core::Vector2i(WIDTH, HEIGHT));

        return Context;
    }

    void quitGui()
    {
        Context->UnloadAllDocuments();

        Context->RemoveReference();

        Rocket::Core::Shutdown();
        Py_Finalize();

        delete Renderer;
        delete FileInterface;
        delete SystemInterface;
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

    RenderSettings getWindowSize()
    {
        RenderSettings settings;
        SDL_GetWindowSize(screen, &settings.windowWidth, &settings.windowHeight);
        return settings;
    }

    void updateGuiBuffer(std::vector<DrawCommand>* buffer)
    {
        if(resized)
        {
            Context->SetDimensions(Rocket::Core::Vector2i(WIDTH, HEIGHT));
            resized = false;
        }

        if(buffer)
            buffer->clear();

        Renderer->mDrawBuffer = buffer;
        Context->Render();
    }

    void drawGui(std::vector<DrawCommand>& buffer, SpriteCacheBase* cache)
    {
        Renderer->drawBuffer(buffer, cache);
    }

    SDL_Surface* loadNonCelImage(const std::string& sourcePath, const std::string& extension)
    {
        FAIO::FAFile* file_handle = FAIO::FAfopen(sourcePath);
        if (!file_handle)
            return NULL;

        size_t buffer_size = FAIO::FAsize(file_handle);

        char* buffer = new char[buffer_size];
        FAIO::FAfread(buffer, 1, buffer_size, file_handle);
        FAIO::FAfclose(file_handle);

        SDL_Surface* s = IMG_LoadTyped_RW(SDL_RWFromMem(buffer, buffer_size), 1, extension.c_str());
        delete[] buffer;

        return s;
    }

    std::string getImageExtension(const std::string& path)
    {
        size_t i;
        for(i = path.length() - 1; i > 0; i--)
        {
            if(path[i] == '.')
                break;
        }

        return path.substr(i+1, path.length()-i);
    }


    bool getImageInfo(const std::string& path, size_t& width, size_t& height, size_t& animLength, int32_t celIndex)
    {
        //TODO: get better image decoders that allow you to peek image dimensions without loading full image

        std::string extension = getImageExtension(path);

        if(Misc::StringUtils::ciEqual(extension, "cel") || Misc::StringUtils::ciEqual(extension, "cl2"))
        {
            Cel::CelFile cel(path);
            width = cel[celIndex].mWidth;
            height = cel[celIndex].mHeight;
            animLength = cel.animLength();
        }
        else
        {
            if(celIndex != 0)   // no indices on normal files
                return false; 

            SDL_Surface* surface = loadNonCelImage(path, extension);

            if(surface)
            {
                width = surface->w;
                height = surface->h;
                animLength = 1;

                SDL_FreeSurface(surface);
            }
            else
            {
                return false;
            }
        }

        return true;
    }

    Cel::Colour getPixel(SDL_Surface* s, int x, int y);
    void setpixel(SDL_Surface *s, int x, int y, Cel::Colour c);
    SDL_Surface* createTransparentSurface(size_t width, size_t height);

    SDL_Surface* loadNonCelImageTrans(const std::string& path, const std::string& extension, bool hasTrans, size_t transR, size_t transG, size_t transB)
    {
        SDL_Surface* tmp = loadNonCelImage(path, extension);

        if(hasTrans)
        {
            SDL_Surface* src = tmp;
            tmp = createTransparentSurface(src->w, src->h);

            for(int x = 0; x < src->w; x++)
            {
                for(int y = 0; y < src->h; y++)
                {
                    Cel::Colour px = getPixel(src, x, y);
                    if(!(px.r == transR && px.g == transG && px.b == transB))
                    {
                        setpixel(tmp, x, y, px);
                    }
                }
            }
            SDL_FreeSurface(src);
        }

        return tmp;
    }

    SpriteGroup* loadSprite(const std::string& path, bool hasTrans, size_t transR, size_t transG, size_t transB)
    {
        std::string extension = getImageExtension(path);

        if(Misc::StringUtils::ciEqual(extension, "cel") || Misc::StringUtils::ciEqual(extension, "cl2"))
        {
            return new SpriteGroup(path);
        }
        else
        {
            SDL_Surface* tmp = loadNonCelImageTrans(path, extension, hasTrans, transR, transG, transB);
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tmp);
            SDL_FreeSurface(tmp);

            std::vector<Sprite> vec(1);
            vec[0] = (Sprite)tex;

            return new SpriteGroup(vec);
        }
    }

    void clearTransparentSurface(SDL_Surface* s);

    SpriteGroup* loadVanimSprite(const std::string& path, size_t vAnim, bool hasTrans, size_t transR, size_t transG, size_t transB)
    {
        std::string extension = getImageExtension(path);
        SDL_Surface* original = loadNonCelImageTrans(path, extension, hasTrans, transR, transG, transB);

        SDL_Surface* tmp = createTransparentSurface(original->w, vAnim);

        std::cout << original->w;

        std::vector<Sprite> vec;

        for(size_t srcY = 0; srcY < (size_t)original->h-1; srcY += vAnim)
        {
            for(size_t x = 0; x < (size_t)original->w; x++)
            {
                for(size_t y = 0; y < vAnim; y++)
                {
                    if(srcY + y < (size_t)original->h)
                    {
                        Cel::Colour px = getPixel(original, x, srcY + y);
                        setpixel(tmp, x, y, px);
                    }
                }
            }

            vec.push_back((Sprite)SDL_CreateTextureFromSurface(renderer, tmp));

            clearTransparentSurface(tmp);
        }

        SDL_FreeSurface(original);
        SDL_FreeSurface(tmp);

        return new SpriteGroup(vec);
    }

    SpriteGroup* loadResizedSprite(const std::string& path, size_t width, size_t height, size_t tileWidth, size_t tileHeight, bool hasTrans, size_t transR, size_t transG, size_t transB)
    {
        std::string extension = getImageExtension(path);
        SDL_Surface* original = loadNonCelImageTrans(path, extension, hasTrans, transR, transG, transB);
        SDL_Surface* tmp = createTransparentSurface(width, height);

        size_t srcX = 0;
        size_t srcY = 0;
        size_t dstX = 0;
        size_t dstY = 0;

        while(true)
        {
            for(size_t y = 0; y < tileHeight ; y += 1)
            {
                for(size_t x = 0; x < tileWidth ; x += 1)
                {
                    Cel::Colour px = getPixel(original, srcX + x, srcY + y);
                    setpixel(tmp, dstX + x, dstY + y, px);
                }
            }

            srcX += tileWidth;
            if(srcX >= (size_t)original->w)
            {
                srcX = 0;
                srcY += tileHeight;
            }

            if(srcY >= (size_t)original->h)
                break;

            dstX += tileWidth;
            if(dstX >= width)
            {
                dstX = 0;
                dstY += tileHeight;
            }

            if(dstY >= height)
                break;
        }

        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tmp);
        SDL_FreeSurface(original);
        SDL_FreeSurface(tmp);

        std::vector<Sprite> vec(1);
        vec[0] = (Sprite)tex;

        return new SpriteGroup(vec);
    }
    void drawFrame(SDL_Surface* s, int start_x, int start_y, const Cel::CelFrame& frame);

    void drawCursor(Sprite s, size_t w, size_t h)
    {

        if(s == NULL)
        {

            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
            SDL_ShowCursor(1);

        }

        else
        {
            SDL_ShowCursor(0);
            int x,y;
            SDL_GetMouseState(&x,&y);
            drawAt(s, x-w/2, y-h/2);

        }
        return;



    }

    SpriteGroup* loadSprite(const uint8_t* source, size_t width, size_t height)
    {
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
            Uint32 rmask = 0xff000000;
            Uint32 gmask = 0x00ff0000;
            Uint32 bmask = 0x0000ff00;
            Uint32 amask = 0x000000ff;
        #else
            Uint32 rmask = 0x000000ff;
            Uint32 gmask = 0x0000ff00;
            Uint32 bmask = 0x00ff0000;
            Uint32 amask = 0xff000000;
        #endif

        SDL_Surface *surface = SDL_CreateRGBSurfaceFrom ((void*) source, width, height, 32, width*4, rmask, gmask, bmask, amask);
        SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
        SDL_FreeSurface(surface);

        std::vector<Sprite> vec(1);
        vec[0] = (Sprite)tex;
        return new SpriteGroup(vec);
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


    SpriteGroup::SpriteGroup(const std::string& path)
    {
        Cel::CelFile cel(path);
        
        for(size_t i = 0; i < cel.numFrames(); i++)
        {
            SDL_Surface* s = createTransparentSurface(cel[i].mWidth, cel[i].mHeight);
            drawFrame(s, 0, 0, cel[i]);

            mSprites.push_back(SDL_CreateTextureFromSurface(renderer, s));

            SDL_FreeSurface(s);
        }


        mAnimLength = cel.animLength();
    }

    void SpriteGroup::toPng(const std::string& celPath, const std::string& pngPath)
    {
        Cel::CelFile cel(celPath);

        size_t numFrames = cel.animLength();
        if(numFrames == 0)
            return;

        size_t sumWidth = 0;
        size_t maxHeight = 0;
        for(size_t i = 0; i < numFrames; i++)
        {
            sumWidth += cel[i].mWidth;
            if(cel[i].mHeight > maxHeight) maxHeight = cel[i].mHeight;
        }

        if(sumWidth == 0)
            return;

        SDL_Surface* s = createTransparentSurface(sumWidth, maxHeight);
        unsigned int x = 0;
        unsigned int dx = 0;
        for(size_t i = 0; i < numFrames; i++)
        {
            drawFrame(s, x, 0, cel[i]);
            dx = cel[i].mWidth;
            x += dx;
        }

        SDL_SavePNG(s,pngPath.c_str());

        SDL_FreeSurface(s);
    }

    void SpriteGroup::destroy()
    {
        for(size_t i = 0; i < mSprites.size(); i++)
            SDL_DestroyTexture((SDL_Texture*)mSprites[i]);
    }
    
    void drawMinPillarTop(SDL_Surface* s, int x, int y, const std::vector<int16_t>& pillar, Cel::CelFile& tileset);
    void drawMinPillarBase(SDL_Surface* s, int x, int y, const std::vector<int16_t>& pillar, Cel::CelFile& tileset);
    
    SpriteGroup* loadTilesetSprite(const std::string& celPath, const std::string& minPath, bool top)
    {
        Cel::CelFile cel(celPath);
        Level::Min min(minPath);

        SDL_Surface* newPillar = createTransparentSurface(64, 256);

        std::vector<Sprite> newMin(min.size()-1);

        for(size_t i = 0; i < min.size()-1; i++)
        {
            clearTransparentSurface(newPillar);

            if(top)
                drawMinPillarTop(newPillar, 0, 0, min[i], cel);
            else
                drawMinPillarBase(newPillar, 0, 0, min[i], cel);

            newMin[i] = SDL_CreateTextureFromSurface(renderer, newPillar);
        }

        SDL_FreeSurface(newPillar);

        return new SpriteGroup(newMin);
    }
    
    void spriteSize(const Sprite& sprite, size_t& w, size_t& h)
    {
        int tmpW, tmpH;
        SDL_QueryTexture((SDL_Texture*)sprite, NULL, NULL, &tmpW, &tmpH);
        w = tmpW;
        h = tmpH;
    }

    void clear(int r, int g, int b)
    {
         SDL_SetRenderDrawColor(renderer, r, g, b, 255);
         SDL_RenderClear(renderer);
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

    void setpixel(SDL_Surface *surface, int x, int y, Cel::Colour c)
    {
        Uint32 pixel = SDL_MapRGBA(surface->format, c.r, c.g, c.b, ((int)c.visible)*255);

        int bpp = surface->format->BytesPerPixel;
        // Here p is the address to the pixel we want to set
        Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

        switch(bpp) {
        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *)p = pixel;
            break;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4:
            *(Uint32 *)p = pixel;
            break;
        }
    }

    Cel::Colour getPixel(SDL_Surface* s, int x, int y)
    {
        Uint32 pix;

        int bpp = s->format->BytesPerPixel;
        // Here p is the address to the pixel we want to retrieve
        Uint8 *p = (Uint8 *)s->pixels + y * s->pitch + x * bpp;

        switch(bpp) {
            case 1:
                pix = *p;
                break;

            case 2:
                pix = *(Uint16 *)p;
                break;

            case 3:
                if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                    pix = p[0] << 16 | p[1] << 8 | p[2];
                else
                    pix = p[0] | p[1] << 8 | p[2] << 16;
                break;

            case 4:
                pix = *(Uint32 *)p;
                break;

            default:
                pix = 0;
        }

        Uint8 r, g, b, a;
        SDL_GetRGBA(pix, s->format, &r, &g, &b, &a);

        return Cel::Colour(r, g, b, a == 255);
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

    void drawMinPillar(SDL_Surface* s, int x, int y, const std::vector<int16_t>& pillar, Cel::CelFile& tileset, bool top)
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

    void drawMinPillarTop(SDL_Surface* s, int x, int y, const std::vector<int16_t>& pillar, Cel::CelFile& tileset)
    {
        drawMinPillar(s, x, y, pillar, tileset, true);
    }

    void drawMinPillarBase(SDL_Surface* s, int x, int y, const std::vector<int16_t>& pillar, Cel::CelFile& tileset)
    {
        drawMinPillar(s, x, y, pillar, tileset, false);
    }

    void drawAt(const Level::Level& level, const Sprite& sprite, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist, int32_t levelX, int32_t levelY)
    {
        size_t w, h;
        spriteSize(sprite, w, h);

        int32_t xPx1 = ((y1*(-32)) + 32*x1 + level.width()*32) + levelX - w/2;
        int32_t yPx1 = ((y1*16) + (16*x1) +160) + levelY;

        int32_t xPx2 = ((y2*(-32)) + 32*x2 + level.width()*32) + levelX - w/2;
        int32_t yPx2 = ((y2*16) + (16*x2) +160) + levelY;

        int32_t x = xPx1 + ((((float)(xPx2-xPx1))/100.0)*(float)dist);
        int32_t y = yPx1 + ((((float)(yPx2-yPx1))/100.0)*(float)dist);

        drawAt(sprite, x, y);
    }

    void getMapScreenCoords(const Level::Level& level, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist, int32_t& levelX, int32_t& levelY)
    {
        int16_t xPx1 = -((y1*(-32)) + 32*x1 + level.width()*32) +WIDTH/2;
        int16_t yPx1 = -((y1*16) + (16*x1) +160) + HEIGHT/2;

        int16_t xPx2 = -((y2*(-32)) + 32*x2 + level.width()*32) +WIDTH/2;
        int16_t yPx2 = -((y2*16) + (16*x2) +160) + HEIGHT/2;

        levelX = xPx1 + ((((float)(xPx2-xPx1))/100.0)*(float)dist);
        levelY = yPx1 + ((((float)(yPx2-yPx1))/100.0)*(float)dist);
    }

    std::pair<int32_t, int32_t> getTileFromScreenCoords(const Level::Level& level, size_t levelX, size_t levelY, size_t x, size_t y)
    {
        // Position on the map in pixels
        int32_t flatX = x - levelX;
        int32_t flatY = y - levelY;

        // position on the map divided into 32x16 flat blocks
        // every second one of these blocks is centred on an isometric
        // block centre, the others are centred on isometric block corners
        int32_t flatGridX = (flatX+16) / 32;
        int32_t flatGridY = (flatY+8) / 16;
        
        // origin position (in flat grid coords) for the first line (isometric y = 0)
        int32_t flatOriginPosX = level.height();
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
    
    std::pair<int32_t, int32_t> getClickedTile(const Level::Level& level, size_t x, size_t y, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist)
    {
        int32_t levelX, levelY;
        getMapScreenCoords(level, x1, y1, x2, y2, dist, levelX, levelY);
        
        return getTileFromScreenCoords(level, levelX, levelY, x, y);
    }
    
    // returns: whether or not to keep drawing the current line (see where it's used for what I mean by "line"
    bool drawLevelHelper(const Level::Level& level, SpriteGroup& minSprites, int32_t x, int32_t y, int32_t levelX, int32_t levelY)
    {
        // if the top left of our screen is on a negative coord (we're at the edge of the map)
        // we want to continue the line, but we obviously don't want to draw tiles outside the map
        if(x < 0)
            return true;
        
        if((size_t)x < level.width() && (size_t)y < level.height())
        {
            size_t index = level[x][y].index();
            int32_t xCoord = (y*(-32)) + 32*x + level.height()*32-32 + levelX;
            int32_t yCoord = (y*16) + 16*x + levelY;

            if(xCoord <=  WIDTH && yCoord <= HEIGHT)
            {
                if(index < minSprites.size())
                    drawAt(minSprites[index], xCoord, yCoord);
                
                return true;
            }
            
            return false;
        }
        
        return false;
    }

    void drawLevel(const Level::Level& level, size_t minTopsHandle, size_t minBottomsHandle, SpriteCacheBase* cache, LevelObjects& objs, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist)
    {
        int32_t levelX, levelY;
        getMapScreenCoords(level, x1, y1, x2, y2, dist, levelX, levelY);

        //TODO clean up the magic numbers here, and elsewhere in this file

        SpriteGroup* minBottoms = cache->get(minBottomsHandle);
        
        
        int32_t startX = WIDTH;
        int32_t startY = -32;
        
        // draw the squares that are in view, starting at the top right of the screen, running
        // in lines along the isometric x axis until they go off screen, with each line starting one
        // to the left in screen space (x-1, y+1 in iso space)
        // this loop draws the bottoms of the tiles, ie the ground, not including walls
        while(startY < HEIGHT)
        {
            std::pair<int32_t, int32_t> tilePos = getTileFromScreenCoords(level, levelX, levelY, startX, startY);
            
            while(drawLevelHelper(level, *minBottoms, tilePos.first, tilePos.second, levelX, levelY))
                tilePos.first++;
            
            if(startX > -64)
            {
                startX -= 64;
                
                if(startX < -64)
                    startX = -64;
            }
            else
            {
                startY += 32;
            }
        }
        
        
        startX = WIDTH;
        startY = -256;
        
        SpriteGroup* minTops = cache->get(minTopsHandle);
        cache->setImmortal(minTopsHandle, true);
        
        // same as above, but for the above-gound parts (walls + actors)
        // starts at -256 and finishes at +256 because the minTops are 256 px tall
        // Note to people trying to improve perf: I tried rolling both loops into one,
        // the stuff from this loop has to be drawn at y-1 of the stuff from the first loop (or maybe >1)
        // but it's actually slower. My guess would be that the offset ruins the cache,
        // so yeah, it's a copy pasted loop DRY blah blah blah
        while(startY < HEIGHT + 256)
        {
            std::pair<int32_t, int32_t> tilePos = getTileFromScreenCoords(level, levelX, levelY, startX, startY);
            
            while(drawLevelHelper(level, *minTops, tilePos.first, tilePos.second, levelX, levelY))
            {
                if(tilePos.first >= 0 && tilePos.second >= 0 && objs[tilePos.first][tilePos.second].valid)
                {
                    LevelObject o = objs[tilePos.first][tilePos.second];
                    drawAt(level, cache->get(objs[tilePos.first][tilePos.second].spriteCacheIndex)->operator[](o.spriteFrame), tilePos.first, tilePos.second, objs[tilePos.first][tilePos.second].x2, objs[tilePos.first][tilePos.second].y2, objs[tilePos.first][tilePos.second].dist, levelX, levelY);
                }
                
                tilePos.first++;
            }
            
            if(startX > -64)
            {
                startX -= 64;
                
                if(startX < -64)
                    startX = -64;
            }
            else
            {
                startY += 32;
            }
        }
        cache->setImmortal(minTopsHandle, false);
    }
}
