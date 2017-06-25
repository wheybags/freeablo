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
#include <faio/fafileobject.h>

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

            #ifdef NDEBUG
                PyRun_SimpleString("import sys\nsys.path.append('./Release')");
            #else
                PyRun_SimpleString("import sys\nsys.path.append('./Debug')");
            #endif
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

        Rocket::Core::FontDatabase::LoadFontFace("resources/fonts/Freeablo/FreeabloWhite11.fnt");

        Rocket::Core::FontDatabase::LoadFontFace("resources/fonts/Freeablo/FreeabloGold16.fnt");
        Rocket::Core::FontDatabase::LoadFontFace("resources/fonts/Freeablo/FreeabloGold22.fnt");
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
        FAIO::FAFileObject file_handle(sourcePath);
        if (!file_handle.isValid())
            return NULL;

        size_t buffer_size = file_handle.FAsize();

        char* buffer = new char[buffer_size];
        file_handle.FAfread(buffer, 1, buffer_size);

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


    bool getImageInfo(const std::string& path, uint32_t& width, uint32_t& height, uint32_t& animLength, int32_t celIndex)
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
    void drawFrame(SDL_Surface* s, int start_x, int start_y, const Cel::CelFrame& frame);

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

    SpriteGroup* loadCelToSingleTexture(const std::string& path)
    {
        Cel::CelFile cel(path);

        size_t width = 0;
        size_t height = 0;

        for(size_t i = 0; i < cel.numFrames(); i++)
        {
            width += cel[i].mWidth;
            height = (cel[i].mHeight > height ? cel[i].mHeight : height);
        }

        assert(width > 0);
        assert(height > 0);

        SDL_Surface* surface = createTransparentSurface(width, height);

        size_t x = 0;
        for(size_t i = 0; i < cel.numFrames(); i++)
        {
            drawFrame(surface, x, 0, cel[i]);
            x += cel[i].mWidth;
        }

        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        std::vector<Sprite> vec(1);
        vec[0] = (Sprite)tex;

        return new SpriteGroup(vec);
    }

    SpriteGroup* loadTiledTexture(const std::string& sourcePath, size_t width, size_t height, bool hasTrans, size_t transR, size_t transG, size_t transB)
    {
        std::string extension = getImageExtension(sourcePath);
        SDL_Surface* tile = loadNonCelImageTrans(sourcePath, extension, hasTrans, transR, transG, transB);
        SDL_Surface* texture = createTransparentSurface(width, height);

        int dx = tile->w;
        int dy = tile->h;

        for(size_t y = 0 ; y < height ; y += dy )
        {
            for(size_t x = 0 ; x < width ; x += dx )
            {
                for(size_t sy = 0 ; sy < (size_t)tile->h && (y + sy) < height; sy++)
                {
                    for(size_t sx = 0 ; sx < (size_t)tile->w && (x + sx) < width ; sx++)
                    {
                        Cel::Colour px = getPixel(tile, sx, sy);
                        setpixel(texture, x + sx, y + sy, px);
                    }
                }
            }
        }

        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, texture);
        SDL_FreeSurface(texture);
        SDL_FreeSurface(tile);

        std::vector<Sprite> vec(1);
        vec[0] = (Sprite)tex;

        return new SpriteGroup(vec);
    }

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
            drawSprite (s, x-w/2, y - h/2);
        }
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

    void drawSprite(const Sprite &sprite, size_t x, size_t y)
    {
        int width, height;
        auto texture = static_cast<SDL_Texture *> (sprite);
        SDL_QueryTexture(texture, NULL, NULL, &width, &height);

        SDL_Rect dest = { int(x), int(y), width, height };

        SDL_RenderCopy(renderer, texture, NULL, &dest);
    }

    constexpr auto tileHeight = 32;
    constexpr auto tileWidth = tileHeight * 2;

    void drawAtTile(const Sprite& sprite, const Misc::Point& tileTop, int spriteW, int spriteH)
    {
        // centering spright at the center of tile by width and at the bottom of tile by height
        drawSprite(sprite, tileTop.x - spriteW / 2, tileTop.y - spriteH + tileHeight);
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

    void spriteSize(const Sprite& sprite, int32_t& w, int32_t& h)
    {
        SDL_QueryTexture((SDL_Texture*)sprite, NULL, NULL, &w, &h);
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

    // basic transform of isometric grid to normal, (0, 0) tile coordinate maps to (0, 0) pixel coordinates
    // since eventually we're gonna shift coordinates to viewport center, it's better to keep transform itself
    // as simple as possible
    static Misc::Point tileTopPoint (const Tile &tile) {
      return {(tileWidth / 2) * (tile.x - tile.y), (tile.y + tile.x) * (tileHeight / 2)};
    }

    // this function simply does the reverse of the above function, could be found by solving linear equation system
    // it obviously uses the fact that ttileWidth = tileHeight * 2
    Tile getTileFromScreenCoords(const Misc::Point& screenPos, const Misc::Point& toScreen)
    {
        auto point = screenPos - toScreen;
        return {(2 * point.y + point.x) / tileWidth, (2 * point.y - point.x) / tileWidth}; // divisin by 64 is pretty fast btw
    }

    static Misc::Point pointBetween (const Tile &start, const Tile &finish, const size_t &percent) {
      auto pointA = tileTopPoint (start);
      auto pointB = tileTopPoint (finish);
      return pointA + (pointB - pointA) * (percent * 0.01);
    }

    void drawMovingSprite(const Sprite& sprite, const Tile& start, const Tile& finish, size_t dist, const Misc::Point& toScreen)
    {
        int32_t w, h;
        spriteSize(sprite, w, h);
        auto point = pointBetween (start, finish, dist);
        auto res = point + toScreen;
        drawAtTile(sprite, res, w, h);
    }

    constexpr auto bottomMenuSize = 144; // TODO: pass it as a variable
    Misc::Point worldToScreenVector(const Tile& start, const Tile& finish, size_t dist)
    {
        // centering takes in accord bottom menu size to be consistent with original game centering
        return Misc::Point{WIDTH / 2, (HEIGHT - bottomMenuSize) / 2} - pointBetween(start, finish, dist);
    }

    Tile getClickedTile(size_t x, size_t y, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist)
    {
        auto toScreen = worldToScreenVector({x1, y1}, {x2, y2}, dist);
        return getTileFromScreenCoords({static_cast<int32_t> (x), static_cast<int32_t> (y)}, toScreen);
    }

    constexpr auto staticObjectHeight = 256;

    template <typename ProcessTileFunc>
    void drawObjectsByTiles (const Misc::Point &toScreen, ProcessTileFunc processTile)
    {
      Misc::Point start {-tileWidth, -tileHeight};
      auto startingTile = getTileFromScreenCoords(start, toScreen);

      auto startingPoint = tileTopPoint(startingTile) + toScreen;
      auto processLine = [&]()
        {
           auto point = startingPoint;
           auto tile = startingTile;

           while (point.x < WIDTH + tileWidth / 2)
             {
               point.x += tileWidth;
               ++tile.x;
               --tile.y;
               processTile (tile, point);
             }
        };

      // then from top left to top-bottom
      while (startingPoint.y < HEIGHT + staticObjectHeight - tileHeight)
        {
          ++startingTile.y;
          startingPoint.x -= tileWidth / 2; startingPoint.y += tileHeight / 2;
          processLine ();
          ++startingTile.x;
          startingPoint.x += tileWidth / 2; startingPoint.y += tileHeight / 2;
          processLine ();
        }
    }

    void drawLevel(const Level::Level& level, size_t minTopsHandle, size_t minBottomsHandle, SpriteCacheBase* cache, LevelObjects& objs, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist) {
      auto toScreen = worldToScreenVector({x1, y1}, {x2, y2}, dist);
      SpriteGroup* minBottoms = cache->get(minBottomsHandle);
      auto isInvalidTile = [&](const Tile &tile){ return tile.x < 0 || tile.y < 0 || tile.x >= static_cast<int32_t> (level.width()) || tile.y >= static_cast<int32_t> (level.height());};

      // drawing on the ground objects
      drawObjectsByTiles (toScreen, [&](const Tile &tile, const Misc::Point &topLeft){
          // Fill invalid tiles with ground, it looks ok but it's probably better to have something else than zero-eth sprite here
          if (isInvalidTile (tile))
            return drawAtTile ((*minBottoms)[0], topLeft, tileWidth, staticObjectHeight);

        size_t index = level[tile.x][tile.y].index();
        if(index < minBottoms->size())
           drawAtTile ((*minBottoms)[index], topLeft, tileWidth, staticObjectHeight); // all static objects have the same sprite size
      });

      SpriteGroup* minTops = cache->get(minTopsHandle);
      cache->setImmortal(minTopsHandle, true);

      // drawing above the ground and moving object
      drawObjectsByTiles (toScreen, [&](const Tile &tile, const Misc::Point &topLeft){
        if (isInvalidTile (tile))
          return;

        size_t index = level[tile.x][tile.y].index();
        if(index < minTops->size())
           drawAtTile ((*minTops)[index], topLeft, tileWidth, staticObjectHeight);

        auto &objsForTile = objs[tile.x][tile.y];
        for (auto obj : objsForTile) {
            if (obj.valid)
                drawMovingSprite((*cache->get(obj.spriteCacheIndex))[obj.spriteFrame], tile, {obj.x2, obj.y2}, obj.dist, toScreen);
        }
      });

      cache->setImmortal(minTopsHandle, false);
    }
}
