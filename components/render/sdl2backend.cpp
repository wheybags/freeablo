#include "render.h"

#include <complex>
#include <iostream>

#include <SDL.h>
//#include <SDL_opengl.h>
#include <SDL_image.h>

#include "sdl_gl_funcs.h"

#include "../cel/celfile.h"
#include "../cel/celframe.h"

#include "../level/level.h"
#include <faio/fafileobject.h>
#include <misc/assert.h>
#include <misc/savePNG.h>
#include <misc/stringops.h>

// clang-format off
#include <misc/disablewarn.h>
#include "../../extern/jo_gif/jo_gif.cpp"
#include "../../extern/RectangleBinPack/MaxRectsBinPack.h"
#include <misc/enablewarn.h>
// clang-format on

/*#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION*/
#include <fa_nuklear.h>

#define NK_SDL_GL3_IMPLEMENTATION
#include "nuklear_sdl_gl3.h"

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

#if defined(WIN32) || defined(_WIN32)
extern "C" {
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
extern "C" {
__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001;
}
#else
extern "C" {
int NvOptimusEnablement = 1;
}
extern "C" {
int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

namespace Render
{
#define GL_CHECK_ERROR()                                                                                                                                       \
    {                                                                                                                                                          \
        GLenum err;                                                                                                                                            \
        while ((err = glGetError()) != GL_NO_ERROR)                                                                                                            \
        {                                                                                                                                                      \
            fprintf(stderr, "glError %s:%d, 0x%04X\n", __FILE__, __LINE__, err);                                                                               \
        }                                                                                                                                                      \
    }

    /* Stores many small textures into a large texture (or array of textures)
     * to allow batched drawing commands that increase performance. */
    class TextureAtlas
    {
    public:
        class TextureAtlasEntry
        {
        public:
            TextureAtlasEntry() = default;
            TextureAtlasEntry(int32_t x, int32_t y, int32_t layer, int32_t width, int32_t height) : mX(x), mY(y), mLayer(layer), mWidth(width), mHeight(height)
            {
            }

            int32_t mX = 0, mY = 0, mLayer = 0, mWidth = 0, mHeight = 0;
        };

        TextureAtlas()
        {
            GLint maxTextureSize;
            GLint maxArrayTextureLayers;
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
            glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxArrayTextureLayers);

            mTextureWidth = maxTextureSize;
            mTextureHeight = maxTextureSize;

            // Limit array texture depth to a reasonable level (measured from testing).
            // Note: Increasing this has a severe impact on performance.
            // TODO: Something is falling apart when texture has more than one layer.
            //       Alpha seems to sometimes disappear, maybe blending on the z-axis?
            GLint estimatedRequiredTextures = 1;
            // GLint estimatedRequiredTextures = (1uLL << 29) / (mTextureWidth * mTextureHeight);
            mTextureLayers = std::min(estimatedRequiredTextures, maxArrayTextureLayers);

            for (int32_t layer = 0; layer < mTextureLayers; layer++)
                mBinPacker.push_back(rbp::MaxRectsBinPack(mTextureWidth, mTextureHeight, false));

            glGenTextures(1, &mTextureArrayId);
            glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureArrayId);

            // Allocate memory for texture array (by passing NULL).
            // TODO: Use compressed format as this texture is very large.
            // NOTE: For GL_COMPRESSED_RGBA image dimensions need to be padded to
            // multiples of 4: https://forums.khronos.org/showthread.php/77554
            glTexImage3D(GL_TEXTURE_2D_ARRAY,
                         0,
                         /*GL_RGBA8*/ GL_RGB5_A1 /*GL_COMPRESSED_RGBA*/,
                         mTextureWidth,
                         mTextureHeight,
                         mTextureLayers,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         NULL);

            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Diagnostic only.
            GLint internalFormat = 0;
            glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
            printf("MaxTextureSize %d, MaxArrayTextureLayers %d, used (%d, %d, %d), 0x%04X\n",
                   maxTextureSize,
                   maxArrayTextureLayers,
                   mTextureWidth,
                   mTextureHeight,
                   mTextureLayers,
                   internalFormat);
        }

        size_t addTexture(int32_t width, int32_t height, const void* data)
        {
            release_assert(width <= mTextureWidth && height <= mTextureHeight); // Texture size too small...

            rbp::Rect packedPos;
            int32_t layer;
            for (layer = 0; layer < mTextureLayers; layer++)
            {
                packedPos = mBinPacker[layer].Insert(width, height, rbp::MaxRectsBinPack::RectBestAreaFit);
                if (packedPos.height != 0)
                    break;
            }
            release_assert(layer < mTextureLayers); // Run out of memory...

            glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureArrayId);
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, packedPos.x, packedPos.y, layer, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

            // TODO: Sometimes adding glFlush/glFinish fixes tiles a bit
            // when texture is compressed but not really sure why...
            // glFlush();
            // glFinish();

            auto id = mNextTextureId++;
            mLookupMap[id] = TextureAtlasEntry(packedPos.x, packedPos.y, layer, width, height);

            return id;
        }

        void bind() { glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureArrayId); }
        void free() { glDeleteTextures(1, &mTextureArrayId); }
        GLint getTextureWidth() const { return mTextureWidth; }
        GLint getTextureHeight() const { return mTextureHeight; }
        const std::map<size_t, TextureAtlasEntry>& getLookupMap() const { return mLookupMap; }

        float getOccupancy() const
        {
            float summedOccupancy = 0;
            for (auto& bp : mBinPacker)
                summedOccupancy += bp.Occupancy();
            return summedOccupancy / mBinPacker.size() * 100;
        }

    private:
        GLuint mTextureArrayId;
        GLint mTextureWidth;
        GLint mTextureHeight;
        GLint mTextureLayers;
        std::map<size_t, TextureAtlasEntry> mLookupMap;
        size_t mNextTextureId = 1;
        std::vector<rbp::MaxRectsBinPack> mBinPacker;
    };

    /* Caches level sprites/positions etc in a format that can be directly injected into GL VBOs. */
    class DrawLevelCache
    {
    public:
        DrawLevelCache(size_t initSize)
        {
            mSprite.reserve(initSize);
            mImageOffset.reserve(2 * initSize);
            mHoverColor.reserve(4 * initSize);
        }

        void addSprite(GLuint sprite, int32_t x, int32_t y, boost::optional<Cel::Colour> highlightColor)
        {
            mSprite.push_back(sprite);

            mImageOffset.push_back(x);
            mImageOffset.push_back(y);

            if (auto c = highlightColor)
            {
                mHoverColor.push_back(c->r / 255.f);
                mHoverColor.push_back(c->g / 255.f);
                mHoverColor.push_back(c->b / 255.f);
                mHoverColor.push_back(1.0f);
            }
            else
            {
                mHoverColor.push_back(0.0f);
                mHoverColor.push_back(0.0f);
                mHoverColor.push_back(0.0f);
                mHoverColor.push_back(0.0f);
            }
        }

        size_t size() const { return mSprite.size(); }

        void clear()
        {
            mSprite.clear();
            mImageOffset.clear();
            mHoverColor.clear();
        }

        std::vector<GLuint> mSprite;
        std::vector<GLfloat> mImageOffset;
        std::vector<GLfloat> mHoverColor;
    };

    int32_t WIDTH = 1280;
    int32_t HEIGHT = 960;

    SDL_Window* screen;
    // SDL_Renderer* renderer;
    SDL_GLContext glContext;

    // textureAtlas is optional as to not instantiate until opengl is setup.
    boost::optional<TextureAtlas> textureAtlas = boost::none;
    DrawLevelCache drawLevelCache = DrawLevelCache(2000);

    void init(const std::string& title, const RenderSettings& settings, NuklearGraphicsContext& nuklearGraphics, nk_context* nk_ctx)
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
        screen = SDL_CreateWindow(title.c_str(), 20, 20, WIDTH, HEIGHT, flags);
        if (screen == NULL)
            printf("Could not create window: %s\n", SDL_GetError());

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        glContext = SDL_GL_CreateContext(screen);

        // Check opengl version is at least 3.3.
        const GLubyte* glVersion(glGetString(GL_VERSION));
        int major = glVersion[0] - '0';
        int minor = glVersion[2] - '0';
        if (major < 3 || (major == 3 && minor < 3))
            std::cerr << "ERROR: Minimum OpenGL version is 3.3. Your current version is " << major << "." << minor << std::endl;

        /*int oglIdx = -1;
        int nRD = SDL_GetNumRenderDrivers();
        for (int i = 0; i < nRD; i++)
        {
            SDL_RendererInfo info;
            if (!SDL_GetRenderDriverInfo(i, &info))
            {
                if (!strcmp(info.name, "opengl"))
                {
                    oglIdx = i;
                }
            }
        }*/

        // renderer = SDL_CreateRenderer(screen, oglIdx, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        initGlFuncs();

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        textureAtlas = TextureAtlas();

        if (nk_ctx)
        {
            memset(&nuklearGraphics, 0, sizeof(nuklearGraphics));
            nk_sdl_device_create(nuklearGraphics.dev);
        }
    }

    void setWindowSize(const RenderSettings& settings) { SDL_SetWindowSize(screen, settings.windowWidth, settings.windowHeight); }

    void destroyNuklearGraphicsContext(NuklearGraphicsContext& nuklearGraphics)
    {
        nk_font_atlas_clear(&nuklearGraphics.atlas);
        nk_sdl_device_destroy(nuklearGraphics.dev);
    }

    void quit()
    {
        textureAtlas->free();
        SDL_GL_DeleteContext(glContext);
        // SDL_DestroyRenderer(renderer);
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
        settings.windowWidth = WIDTH;
        settings.windowHeight = HEIGHT;
        // SDL_GetRendererOutputSize(renderer, &settings.windowWidth, &settings.windowHeight);
        // SDL_GetWindowSize(screen, &settings.windowWidth, &settings.windowHeight);
        return settings;
    }

    // TODO: bindSprite needs to be tidied up, was just a quick throw in for testing.
    void bindSprite(const Sprite& sprite,
                    int32_t& width,
                    int32_t& height,
                    int32_t& atlasOffsetX,
                    int32_t& atlasOffsetY,
                    int32_t& atlasOffsetZ,
                    int32_t& atlasWidth,
                    int32_t& atlasHeight)
    {
        auto& atlasEntry = textureAtlas->getLookupMap().at((GLuint)(intptr_t)sprite);
        width = atlasEntry.mWidth;
        height = atlasEntry.mHeight;
        atlasOffsetX = atlasEntry.mX;
        atlasOffsetY = atlasEntry.mY;
        atlasOffsetZ = atlasEntry.mLayer;
        atlasWidth = textureAtlas->getTextureWidth();
        atlasHeight = textureAtlas->getTextureHeight();
        textureAtlas->bind();
    }

    GLuint getGLTexFromSurface(SDL_Surface* surf)
    {
        bool validFormat = true;
        if (surf->format->BitsPerPixel != 24 && surf->format->BitsPerPixel != 32)
            validFormat = false;
        if (surf->format->Rmask != 0x000000FF || surf->format->Gmask != 0x0000FF00 || surf->format->Bmask != 0x00FF0000)
            validFormat = false;
        if (surf->format->BitsPerPixel != 32 || surf->format->Amask != 0xFF000000)
            validFormat = false;
        if (surf->pitch != 4 * surf->w)
            validFormat = false;

        if (!validFormat)
            surf = SDL_ConvertSurfaceFormat(
                surf, SDL_PIXELFORMAT_ABGR8888, 0); // SDL is stupid and interprets pixel formats by endianness, so on LE, it calls RGBA ABGR...

        debug_assert(surf->pitch == 4 * surf->w);

        GLuint id = textureAtlas->addTexture(surf->w, surf->h, surf->pixels);

        if (!validFormat)
            SDL_FreeSurface(surf);

        return id;
    }

    void drawGui(NuklearFrameDump& dump, SpriteCacheBase* cache)
    {
        // IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
        // with blending, scissor, face culling, depth test and viewport and
        // defaults everything back into a default state.
        // Make sure to either a.) save and restore or b.) reset your own state after
        // rendering the UI.
        nk_sdl_render_dump(cache, dump, screen);

        glEnable(GL_BLEND); // see above comment
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
        for (i = path.length() - 1; i > 0; i--)
        {
            if (path[i] == '.')
                break;
        }

        return path.substr(i + 1, path.length() - i);
    }

    bool getImageInfo(const std::string& path, std::vector<int32_t>& widths, std::vector<int32_t>& heights, int32_t& animLength)
    {
        // TODO: get better image decoders that allow you to peek image dimensions without loading full image

        std::string extension = getImageExtension(path);

        if (Misc::StringUtils::ciEqual(extension, "cel") || Misc::StringUtils::ciEqual(extension, "cl2"))
        {
            Cel::CelFile cel(path);
            widths.resize(cel.animLength());
            heights.resize(cel.animLength());
            for (int i = 0; i < cel.animLength(); ++i)
            {
                widths[i] = cel[i].width();
                heights[i] = cel[i].height();
            }
            animLength = cel.animLength();
        }
        else
        {
            SDL_Surface* surface = loadNonCelImage(path, extension);

            if (surface)
            {
                widths = {surface->w};
                heights = {surface->h};
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

    Cel::Colour getPixel(const SDL_Surface* s, int x, int y);
    void setpixel(SDL_Surface* s, int x, int y, Cel::Colour c);
    SDL_Surface* createTransparentSurface(size_t width, size_t height);
    void drawFrame(SDL_Surface* s, int start_x, int start_y, const Cel::CelFrame& frame);

    SDL_Surface* loadNonCelImageTrans(const std::string& path, const std::string& extension, bool hasTrans, size_t transR, size_t transG, size_t transB)
    {
        SDL_Surface* tmp = loadNonCelImage(path, extension);

        if (hasTrans)
        {
            SDL_Surface* src = tmp;
            tmp = createTransparentSurface(src->w, src->h);

            for (int x = 0; x < src->w; x++)
            {
                for (int y = 0; y < src->h; y++)
                {
                    Cel::Colour px = getPixel(src, x, y);
                    if (!(px.r == transR && px.g == transG && px.b == transB))
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

        if (Misc::StringUtils::ciEqual(extension, "cel") || Misc::StringUtils::ciEqual(extension, "cl2"))
        {
            return new SpriteGroup(path);
        }
        else
        {
            SDL_Surface* tmp = loadNonCelImageTrans(path, extension, hasTrans, transR, transG, transB);

            std::vector<Sprite> vec(1);
            vec[0] = (Sprite)(intptr_t)getGLTexFromSurface(tmp);

            SDL_FreeSurface(tmp);

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

        for (size_t srcY = 0; srcY < (size_t)original->h - 1; srcY += vAnim)
        {
            for (size_t x = 0; x < (size_t)original->w; x++)
            {
                for (size_t y = 0; y < vAnim; y++)
                {
                    if (srcY + y < (size_t)original->h)
                    {
                        Cel::Colour px = getPixel(original, x, srcY + y);
                        setpixel(tmp, x, y, px);
                    }
                }
            }

            vec.push_back((Render::Sprite)(intptr_t)getGLTexFromSurface(tmp));

            clearTransparentSurface(tmp);
        }

        SDL_FreeSurface(original);
        SDL_FreeSurface(tmp);

        return new SpriteGroup(vec);
    }

    SpriteGroup* loadResizedSprite(
        const std::string& path, size_t width, size_t height, size_t tileWidth, size_t tileHeight, bool hasTrans, size_t transR, size_t transG, size_t transB)
    {
        std::string extension = getImageExtension(path);
        SDL_Surface* original = loadNonCelImageTrans(path, extension, hasTrans, transR, transG, transB);
        SDL_Surface* tmp = createTransparentSurface(width, height);

        size_t srcX = 0;
        size_t srcY = 0;
        size_t dstX = 0;
        size_t dstY = 0;

        while (true)
        {
            for (size_t y = 0; y < tileHeight; y += 1)
            {
                for (size_t x = 0; x < tileWidth; x += 1)
                {
                    Cel::Colour px = getPixel(original, srcX + x, srcY + y);
                    setpixel(tmp, dstX + x, dstY + y, px);
                }
            }

            srcX += tileWidth;
            if (srcX >= (size_t)original->w)
            {
                srcX = 0;
                srcY += tileHeight;
            }

            if (srcY >= (size_t)original->h)
                break;

            dstX += tileWidth;
            if (dstX >= width)
            {
                dstX = 0;
                dstY += tileHeight;
            }

            if (dstY >= height)
                break;
        }

        std::vector<Sprite> vec(1);
        vec[0] = (Sprite)(intptr_t)getGLTexFromSurface(tmp);

        SDL_FreeSurface(original);
        SDL_FreeSurface(tmp);

        return new SpriteGroup(vec);
    }

    SpriteGroup* loadCelToSingleTexture(const std::string& path)
    {
        Cel::CelFile cel(path);

        int32_t width = 0;
        int32_t height = 0;

        for (int32_t i = 0; i < cel.numFrames(); i++)
        {
            width += cel[i].width();
            height = (cel[i].height() > height ? cel[i].height() : height);
        }

        debug_assert(width > 0);
        debug_assert(height > 0);

        SDL_Surface* surface = createTransparentSurface(width, height);

        int32_t x = 0;
        for (int32_t i = 0; i < cel.numFrames(); i++)
        {
            drawFrame(surface, x, 0, cel[i]);
            x += cel[i].width();
        }

        std::vector<Sprite> vec(1);
        vec[0] = (Sprite)(intptr_t)getGLTexFromSurface(surface);

        SDL_FreeSurface(surface);

        return new SpriteGroup(vec);
    }

    SpriteGroup* loadTiledTexture(const std::string& sourcePath, size_t width, size_t height, bool hasTrans, size_t transR, size_t transG, size_t transB)
    {
        std::string extension = getImageExtension(sourcePath);
        SDL_Surface* tile = loadNonCelImageTrans(sourcePath, extension, hasTrans, transR, transG, transB);
        SDL_Surface* texture = createTransparentSurface(width, height);

        int dx = tile->w;
        int dy = tile->h;

        for (size_t y = 0; y < height; y += dy)
        {
            for (size_t x = 0; x < width; x += dx)
            {
                for (size_t sy = 0; sy < (size_t)tile->h && (y + sy) < height; sy++)
                {
                    for (size_t sx = 0; sx < (size_t)tile->w && (x + sx) < width; sx++)
                    {
                        Cel::Colour px = getPixel(tile, sx, sy);
                        setpixel(texture, x + sx, y + sy, px);
                    }
                }
            }
        }

        std::vector<Sprite> vec(1);
        vec[0] = (Sprite)(intptr_t)getGLTexFromSurface(texture);

        SDL_FreeSurface(texture);
        SDL_FreeSurface(tile);

        return new SpriteGroup(vec);
    }

    SpriteGroup* loadNonCelSprite(const std::string& path)
    {
        std::string extension = getImageExtension(path);
        SDL_Surface* image = loadNonCelImage(path, extension);

        std::vector<Sprite> vec(1);
        vec[0] = (Sprite)(intptr_t)getGLTexFromSurface(image);

        SDL_FreeSurface(image);

        return new SpriteGroup(vec);
    }

    FACursor createCursor(const Cel::CelFrame& celFrame, int32_t hot_x, int32_t hot_y)
    {
        auto surface = createTransparentSurface(celFrame.width(), celFrame.height());
        drawFrame(surface, 0, 0, celFrame);
        auto cursor = SDL_CreateColorCursor(surface, hot_x, hot_y);
        SDL_FreeSurface(surface);
        return (FACursor)cursor;
    }

    void freeCursor(FACursor cursor) { SDL_FreeCursor((SDL_Cursor*)cursor); }

    void drawCursor(FACursor cursor)
    {
        if (cursor == NULL)
        {
            cursor = (FACursor)SDL_GetDefaultCursor();
        }

        SDL_SetCursor((SDL_Cursor*)cursor);
        SDL_ShowCursor(SDL_ENABLE);
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

        SDL_Surface* surface = SDL_CreateRGBSurfaceFrom((void*)source, width, height, 32, width * 4, rmask, gmask, bmask, amask);
        // SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
        // SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

        std::vector<Sprite> vec(1);
        vec[0] = (Sprite)(intptr_t)getGLTexFromSurface(surface);

        SDL_FreeSurface(surface);

        return new SpriteGroup(vec);
    }

    bool once = false;

    GLuint vao = 0;
    GLuint shader_programme = 0;
    GLuint texture = 0;

    GLuint imageSize_vbo;
    GLuint imageOffset_vbo;
    GLuint hoverColor_vbo;
    GLuint atlasOffset_vbo;

    void draw()
    {
        if (!once)
        {

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            once = true;

            float points[] = {0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0};

            float colours[] = {0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1};

            GLuint vbo = 0;
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), points, GL_STATIC_DRAW);

            GLuint uvs_vbo = 0;
            glGenBuffers(1, &uvs_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, uvs_vbo);
            glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), colours, GL_STATIC_DRAW);

            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glBindBuffer(GL_ARRAY_BUFFER, uvs_vbo);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

            // Note: These VBOs use glVertexAttribDivisor(n, 1) which means they're
            // only updated once per instance (instead of once per vertex).
            glGenBuffers(1, &imageSize_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, imageSize_vbo);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glVertexAttribDivisor(2, 1);

            glGenBuffers(1, &imageOffset_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, imageOffset_vbo);
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glVertexAttribDivisor(3, 1);

            glGenBuffers(1, &hoverColor_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, hoverColor_vbo);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, NULL);
            glVertexAttribDivisor(4, 1);

            glGenBuffers(1, &atlasOffset_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, atlasOffset_vbo);
            glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glVertexAttribDivisor(5, 1);

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glEnableVertexAttribArray(3);
            glEnableVertexAttribArray(4);
            glEnableVertexAttribArray(5);

            std::string src = Misc::StringUtils::readAsString("resources/shaders/basic.vert");
            const GLchar* srcPtr = src.c_str();

            GLuint vs = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vs, 1, &srcPtr, NULL);
            glCompileShader(vs);

            GLint isCompiled = 0;
            glGetShaderiv(vs, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &maxLength);

                // The maxLength includes the NULL character
                std::vector<GLchar> errorLog(maxLength);
                glGetShaderInfoLog(vs, maxLength, &maxLength, &errorLog[0]);

                std::cout << &errorLog[0] << std::endl;

                // Provide the infolog in whatever manor you deem best.
                // Exit with failure.
                glDeleteShader(vs); // Don't leak the shader.
                return;
            }

            src = Misc::StringUtils::readAsString("resources/shaders/basic.frag");
            srcPtr = src.c_str();

            GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fs, 1, &srcPtr, NULL);
            glCompileShader(fs);

            glGetShaderiv(fs, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &maxLength);

                // The maxLength includes the NULL character
                std::vector<GLchar> errorLog(maxLength);
                glGetShaderInfoLog(fs, maxLength, &maxLength, &errorLog[0]);

                std::cout << &errorLog[0] << std::endl;

                // Provide the infolog in whatever manor you deem best.
                // Exit with failure.
                glDeleteShader(fs); // Don't leak the shader.
                return;
            }

            shader_programme = glCreateProgram();
            glAttachShader(shader_programme, fs);
            glAttachShader(shader_programme, vs);
            glLinkProgram(shader_programme);

            /*SDL_Surface* surf = SDL_LoadBMP("E:\\tom.bmp");

            SDL_Surface* s = createTransparentSurface(surf->w, surf->h);
            SDL_BlitSurface(surf, NULL, s, NULL);

            texture = getGLTexFromSurface(s);

            SDL_FreeSurface(s);
            SDL_FreeSurface(surf);*/
        }

        /*GLint loc = glGetUniformLocation(shader_programme, "width");
        if (loc != -1)
        {
            glUniform1f(loc, WIDTH);
        }
        loc = glGetUniformLocation(shader_programme, "height");
        if (loc != -1)
        {
            glUniform1f(loc, HEIGHT);
        }

        loc = glGetUniformLocation(shader_programme, "imgW");
        if (loc != -1)
        {
            glUniform1f(loc, 150);
        }
        loc = glGetUniformLocation(shader_programme, "imgH");
        if (loc != -1)
        {
            glUniform1f(loc, 100);
        }

        loc = glGetUniformLocation(shader_programme, "offsetX");
        if (loc != -1)
        {
            glUniform1f(loc, 150);
        }
        loc = glGetUniformLocation(shader_programme, "offsetY");
        if (loc != -1)
        {
            glUniform1f(loc, 100);
        }

        //glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_programme);
        glBindVertexArray(vao);
        // draw points 0-3 from the currently bound VAO with current in-use shader
        glDrawArrays(GL_TRIANGLES, 0, 6);*/

        // drawAt(texture, 0, 0);

        SDL_GL_SwapWindow(screen);
        // SDL_RenderPresent(renderer);
    }

    void drawSprite(GLuint sprite, int32_t x, int32_t y, boost::optional<Cel::Colour> highlightColor)
    {
        // Add to level cache, will be drawn in a batch later.
        drawLevelCache.addSprite(sprite, x, y, highlightColor);
    }

    static void drawCachedLevel()
    {
        glUseProgram(shader_programme);

        auto setUniform = [](const char* name, GLfloat value) {
            GLint loc = glGetUniformLocation(shader_programme, name);
            if (loc != -1)
            {
                glUniform1f(loc, value);
            }
        };

        setUniform("screenWidth", WIDTH);
        setUniform("screenHeight", HEIGHT);
        setUniform("atlasWidth", textureAtlas->getTextureWidth());
        setUniform("atlasHeight", textureAtlas->getTextureHeight());

        size_t spriteCount = drawLevelCache.size();

        std::vector<GLfloat> imageSize(2 * spriteCount);
        std::vector<GLfloat> atlasOffset(3 * spriteCount);

        auto& lookupMap = textureAtlas->getLookupMap();
        for (size_t i = 0; i < spriteCount; i++)
        {
            auto& atlasEntry = lookupMap.at(drawLevelCache.mSprite[i]);

            imageSize[i * 2] = atlasEntry.mWidth;
            imageSize[i * 2 + 1] = atlasEntry.mHeight;
            atlasOffset[i * 3] = atlasEntry.mX;
            atlasOffset[i * 3 + 1] = atlasEntry.mY;
            atlasOffset[i * 3 + 2] = atlasEntry.mLayer;
        }

        glBindVertexArray(vao);

        // Note: These VBOs use glVertexAttribDivisor(n, 1) which means they're
        // only updated once per instance (instead of once per vertex).
        glBindBuffer(GL_ARRAY_BUFFER, imageSize_vbo);
        glBufferData(GL_ARRAY_BUFFER, 2 * spriteCount * sizeof(GLfloat), &imageSize[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, imageOffset_vbo);
        glBufferData(GL_ARRAY_BUFFER, 2 * spriteCount * sizeof(GLfloat), &drawLevelCache.mImageOffset[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, hoverColor_vbo);
        glBufferData(GL_ARRAY_BUFFER, 4 * spriteCount * sizeof(GLfloat), &drawLevelCache.mHoverColor[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, atlasOffset_vbo);
        glBufferData(GL_ARRAY_BUFFER, 3 * spriteCount * sizeof(GLfloat), &atlasOffset[0], GL_STATIC_DRAW);

        textureAtlas->bind();

        // Draw the whole level in one batched operation.
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, spriteCount);

        // Clear cached level data after drawing.
        drawLevelCache.clear();
    }

    void handleEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            // do nothing, just clear the event queue to avoid render window hang up in ubuntu.
        }
    }

    void drawSprite(const Sprite& sprite, int32_t x, int32_t y, boost::optional<Cel::Colour> highlightColor)
    {
        drawSprite((GLuint)(intptr_t)sprite, x, y, highlightColor);
    }

    constexpr auto tileHeight = 32;
    constexpr auto tileWidth = tileHeight * 2;

    void drawAtTile(const Sprite& sprite, const Misc::Point& tileTop, int spriteW, int spriteH, boost::optional<Cel::Colour> highlightColor = boost::none)
    {
        // centering spright at the center of tile by width and at the bottom of tile by height
        drawSprite(sprite, tileTop.x - spriteW / 2, tileTop.y - spriteH + tileHeight, highlightColor);
    }

    SpriteGroup::SpriteGroup(const std::string& path)
    {
        Cel::CelFile cel(path);

        for (int32_t i = 0; i < cel.numFrames(); i++)
        {
            SDL_Surface* s = createTransparentSurface(cel[i].width(), cel[i].height());
            drawFrame(s, 0, 0, cel[i]);

            mSprites.push_back((Render::Sprite)(intptr_t)getGLTexFromSurface(s)); // SDL_CreateTextureFromSurface(renderer, s));

            SDL_FreeSurface(s);
        }
        mWidth = cel[0].width();
        mHeight = cel[0].height();

        mAnimLength = cel.animLength();
    }

    Sprite& SpriteGroup::operator[](size_t index)
    {
        debug_assert(index < mSprites.size());
        return mSprites[index];
    }

    void SpriteGroup::toPng(const std::string& celPath, const std::string& pngPath)
    {
        Cel::CelFile cel(celPath);

        int32_t numFrames = cel.numFrames();

        if (numFrames == 0)
            return;

        int32_t sumWidth = 0;
        int32_t maxHeight = 0;
        for (int32_t i = 0; i < numFrames; i++)
        {
            sumWidth += cel[i].width();
            if (cel[i].height() > maxHeight)
                maxHeight = cel[i].height();
        }

        if (sumWidth == 0)
            return;

        SDL_Surface* s = createTransparentSurface(sumWidth, maxHeight);
        unsigned int x = 0;
        unsigned int dx = 0;
        for (int32_t i = 0; i < numFrames; i++)
        {
            drawFrame(s, x, 0, cel[i]);
            dx = cel[i].width();
            x += dx;
        }

        SDL_SavePNG(s, pngPath.c_str());

        SDL_FreeSurface(s);
    }

    void SpriteGroup::toGif(const std::string& celPath, const std::string& gifPath)
    {
        Cel::CelFile cel(celPath);

        int32_t width = cel[0].width();
        int32_t height = cel[0].height();

        int32_t numFrames = cel.numFrames();

        if (numFrames == 0)
            return;

        jo_gif_t gif = jo_gif_start(gifPath.c_str(), width, height, 0, 256);

        for (int32_t i = 0; i < numFrames; i++)
        {
            SDL_Surface* s = createTransparentSurface(width, height);
            drawFrame(s, 0, 0, cel[i]);

            uint8_t** gifImage = (uint8_t**)malloc(s->h * sizeof(uint8_t*));

            for (int j = 0; j < s->h; j++)
            {
                gifImage[j] = (uint8_t*)(uint8_t**)s->pixels + j * s->pitch;
            }

            jo_gif_frame(&gif, *gifImage, 10, true, 0x00, 0xFF, 0x00);

            free(gifImage);

            SDL_FreeSurface(s);
        }

        jo_gif_end(&gif);
    }

    void SpriteGroup::destroy()
    {
        // Sprites can no longer be removed from texture atlas.
    }

    void drawMinPillarTop(SDL_Surface* s, int x, int y, const std::vector<int16_t>& pillar, Cel::CelFile& tileset);
    void drawMinPillarBase(SDL_Surface* s, int x, int y, const std::vector<int16_t>& pillar, Cel::CelFile& tileset);

    SpriteGroup* loadTilesetSprite(const std::string& celPath, const std::string& minPath, bool top)
    {
        Cel::CelFile cel(celPath);
        Level::Min min(minPath);

        SDL_Surface* newPillar = createTransparentSurface(64, 256);

        std::vector<Sprite> newMin(min.size() - 1);

        for (size_t i = 0; i < min.size() - 1; i++)
        {
            clearTransparentSurface(newPillar);

            if (top)
                drawMinPillarTop(newPillar, 0, 0, min[i], cel);
            else
                drawMinPillarBase(newPillar, 0, 0, min[i], cel);

            newMin[i] = (Sprite)(intptr_t)getGLTexFromSurface(newPillar); // NULL;// SDL_CreateTextureFromSurface(renderer, newPillar);
        }

        SDL_FreeSurface(newPillar);

        return new SpriteGroup(newMin);
    }

    void spriteSize(const Sprite& sprite, int32_t& w, int32_t& h)
    {
        auto& atlasEntry = textureAtlas->getLookupMap().at((size_t)(intptr_t)sprite);
        w = atlasEntry.mWidth;
        h = atlasEntry.mHeight;
    }

    void clear(int r, int g, int b)
    {
        glClearColor(((float)r) / 255.0, ((float)g) / 255.0, ((float)b) / 255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }

#define BPP 4
#define DEPTH 32

    void clearTransparentSurface(SDL_Surface* s) { SDL_FillRect(s, NULL, SDL_MapRGBA(s->format, 0, 0, 0, 0)); }

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

    void setpixel(SDL_Surface* surface, int x, int y, Cel::Colour c)
    {
        Uint32 pixel = SDL_MapRGBA(surface->format, c.r, c.g, c.b, ((int)c.visible) * 255);

        int bpp = surface->format->BytesPerPixel;
        // Here p is the address to the pixel we want to set
        Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

        switch (bpp)
        {
            case 1:
                *p = pixel;
                break;

            case 2:
                *(Uint16*)p = pixel;
                break;

            case 3:
                if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                {
                    p[0] = (pixel >> 16) & 0xff;
                    p[1] = (pixel >> 8) & 0xff;
                    p[2] = pixel & 0xff;
                }
                else
                {
                    p[0] = pixel & 0xff;
                    p[1] = (pixel >> 8) & 0xff;
                    p[2] = (pixel >> 16) & 0xff;
                }
                break;

            case 4:
                *(Uint32*)p = pixel;
                break;
        }
    }

    Cel::Colour getPixel(const SDL_Surface* s, int x, int y)
    {
        Uint32 pix;

        int bpp = s->format->BytesPerPixel;
        // Here p is the address to the pixel we want to retrieve
        Uint8* p = (Uint8*)s->pixels + y * s->pitch + x * bpp;

        switch (bpp)
        {
            case 1:
                pix = *p;
                break;

            case 2:
                pix = *(Uint16*)p;
                break;

            case 3:
                if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                    pix = p[0] << 16 | p[1] << 8 | p[2];
                else
                    pix = p[0] | p[1] << 8 | p[2] << 16;
                break;

            case 4:
                pix = *(Uint32*)p;
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
        for (int32_t x = 0; x < frame.width(); x++)
        {
            for (int32_t y = 0; y < frame.height(); y++)
            {
                auto& c = frame.get(x, y);
                if (c.visible)
                    setpixel(s, start_x + x, start_y + y, c);
            }
        }
    }

    void drawMinTile(SDL_Surface* s, Cel::CelFile& f, int x, int y, int16_t l, int16_t r)
    {
        if (l != -1)
            drawFrame(s, x, y, f[l]);

        if (r != -1)
            drawFrame(s, x + 32, y, f[r]);
    }

    void drawMinPillar(SDL_Surface* s, int x, int y, const std::vector<int16_t>& pillar, Cel::CelFile& tileset, bool top)
    {
        // compensate for maps using 5-row min files
        if (pillar.size() == 10)
            y += 3 * 32;

        size_t i, lim;

        if (top)
        {
            i = 0;
            lim = pillar.size() - 2;
        }
        else
        {
            i = pillar.size() - 2;
            lim = pillar.size();
            y += i * 16;
        }

        // Each iteration draw one row of the min
        for (; i < lim; i += 2)
        {
            int16_t l = (pillar[i] & 0x0FFF) - 1;
            int16_t r = (pillar[i + 1] & 0x0FFF) - 1;

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
    static Misc::Point tileTopPoint(const Tile& tile) { return {(tileWidth / 2) * (tile.pos.x - tile.pos.y), (tile.pos.y + tile.pos.x) * (tileHeight / 2)}; }

    // this function simply does the reverse of the above function, could be found by solving linear equation system
    // it obviously uses the fact that ttileWidth = tileHeight * 2
    Tile getTileFromScreenCoords(const Misc::Point& screenPos, const Misc::Point& toScreen)
    {
        auto point = screenPos - toScreen;
        auto x = std::div(2 * point.y + point.x, tileWidth); // division by 64 is pretty fast btw
        auto y = std::div(2 * point.y - point.x, tileWidth);
        return {x.quot, y.quot, x.rem > y.rem ? TileHalf::right : TileHalf::left};
    }

    static Misc::Point pointBetween(const Tile& start, const Tile& finish, const size_t& percent)
    {
        auto pointA = tileTopPoint(start);
        auto pointB = tileTopPoint(finish);
        return pointA + (pointB - pointA) * (percent * 0.01);
    }

    static void drawMovingSprite(const Sprite& sprite,
                                 const Tile& start,
                                 const Tile& finish,
                                 size_t dist,
                                 const Misc::Point& toScreen,
                                 boost::optional<Cel::Colour> highlightColor = boost::none)
    {
        int32_t w, h;
        spriteSize(sprite, w, h);
        auto point = pointBetween(start, finish, dist);
        auto res = point + toScreen;
        drawAtTile(sprite, res, w, h, highlightColor);
    }

    constexpr auto bottomMenuSize = 144; // TODO: pass it as a variable
    Misc::Point worldToScreenVector(const Tile& start, const Tile& finish, size_t dist)
    {
        // centering takes in accord bottom menu size to be consistent with original game centering
        return Misc::Point{WIDTH / 2, (HEIGHT - bottomMenuSize) / 2} - pointBetween(start, finish, dist);
    }

    Tile getTileByScreenPos(size_t x, size_t y, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist)
    {
        auto toScreen = worldToScreenVector({x1, y1}, {x2, y2}, dist);
        return getTileFromScreenCoords({static_cast<int32_t>(x), static_cast<int32_t>(y)}, toScreen);
    }

    constexpr auto staticObjectHeight = 256;

    template <typename ProcessTileFunc> void drawObjectsByTiles(const Misc::Point& toScreen, ProcessTileFunc processTile)
    {
        Misc::Point start{-2 * tileWidth, -2 * tileHeight};
        auto startingTile = getTileFromScreenCoords(start, toScreen);

        auto startingPoint = tileTopPoint(startingTile) + toScreen;
        auto processLine = [&]() {
            auto point = startingPoint;
            auto tile = startingTile;

            while (point.x < WIDTH + tileWidth / 2)
            {
                point.x += tileWidth;
                ++tile.pos.x;
                --tile.pos.y;
                processTile(tile, point);
            }
        };

        // then from top left to top-bottom
        while (startingPoint.y < HEIGHT + staticObjectHeight - tileHeight)
        {
            ++startingTile.pos.y;
            startingPoint.x -= tileWidth / 2;
            startingPoint.y += tileHeight / 2;
            processLine();
            ++startingTile.pos.x;
            startingPoint.x += tileWidth / 2;
            startingPoint.y += tileHeight / 2;
            processLine();
        }
    }

    void drawLevel(const Level::Level& level,
                   size_t minTopsHandle,
                   size_t minBottomsHandle,
                   size_t specialSpritesHandle,
                   const std::map<int32_t, int32_t>& specialSpritesMap,
                   SpriteCacheBase* cache,
                   LevelObjects& objs,
                   LevelObjects& items,
                   int32_t x1,
                   int32_t y1,
                   int32_t x2,
                   int32_t y2,
                   size_t dist)
    {
        auto toScreen = worldToScreenVector({x1, y1}, {x2, y2}, dist);
        SpriteGroup* minBottoms = cache->get(minBottomsHandle);
        auto isInvalidTile = [&](const Tile& tile) {
            return tile.pos.x < 0 || tile.pos.y < 0 || tile.pos.x >= static_cast<int32_t>(level.width()) || tile.pos.y >= static_cast<int32_t>(level.height());
        };

        // drawing on the ground objects
        drawObjectsByTiles(toScreen, [&](const Tile& tile, const Misc::Point& topLeft) {
            if (isInvalidTile(tile))
            {
                // For some reason this code stopped working so for now out of map tiles should be black
                drawAtTile((*minBottoms)[0], topLeft, tileWidth, staticObjectHeight);
                return;
            }

            size_t index = level.get(tile.pos).index();
            if (index < minBottoms->size())
                drawAtTile((*minBottoms)[index], topLeft, tileWidth, staticObjectHeight); // all static objects have the same sprite size
        });

        SpriteGroup* minTops = cache->get(minTopsHandle);
        cache->setImmortal(minTopsHandle, true);

        // drawing above the ground and moving object
        drawObjectsByTiles(toScreen, [&](const Tile& tile, const Misc::Point& topLeft) {
            if (isInvalidTile(tile))
                return;

            size_t index = level.get(tile.pos).index();
            if (index < minTops->size())
            {
                drawAtTile((*minTops)[index], topLeft, tileWidth, staticObjectHeight);

                // Add special sprites (arches / open door frames) if required.
                if (specialSpritesMap.count(index))
                {
                    int32_t specialSpriteIndex = specialSpritesMap.at(index);
                    SpriteGroup* specialSpriteGroup = cache->get(specialSpritesHandle);
                    Sprite& sprite = (*specialSpriteGroup)[specialSpriteIndex];
                    int w, h;
                    spriteSize(sprite, w, h);
                    drawAtTile(sprite, topLeft, w, h);
                }
            }

            auto& itemsForTile = items.get(tile.pos.x, tile.pos.y);
            for (auto& item : itemsForTile)
            {
                int32_t w, h;
                auto sprite = (*cache->get(item.spriteCacheIndex))[item.spriteFrame];
                spriteSize(sprite, w, h);
                drawAtTile(sprite, topLeft, w, h, item.hoverColor);
            }

            auto& objsForTile = objs.get(tile.pos.x, tile.pos.y);
            for (auto& obj : objsForTile)
            {
                if (obj.valid)
                {
                    auto sprite = cache->get(obj.spriteCacheIndex);
                    drawMovingSprite((*sprite)[obj.spriteFrame], tile, {obj.x2, obj.y2}, obj.dist, toScreen, obj.hoverColor);
                }
            }
        });

        cache->setImmortal(minTopsHandle, false);

        drawCachedLevel();

        GL_CHECK_ERROR();

        // Diagnostic only.
        static size_t loop = 0;
        if ((loop++ % 1000) == 0)
        {
            printf("Texture atlas occupancy %.1f%%\n", textureAtlas->getOccupancy());
            auto latest = textureAtlas->getLookupMap().rbegin();
            printf("Latest texture atlas entry: %zu, %d, %d, %d, %d, %d\n",
                   latest->first,
                   latest->second.mX,
                   latest->second.mY,
                   latest->second.mLayer,
                   latest->second.mWidth,
                   latest->second.mHeight);
        }
    }
}
