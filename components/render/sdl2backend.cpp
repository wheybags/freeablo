#include "render.h"

#include <iostream>
#include <complex>

#include <SDL.h>
//#include <SDL_opengl.h>
#include <SDL_image.h>


#include "sdl_gl_funcs.h"

#include "../cel/celfile.h"
#include "../cel/celframe.h"



#include "../level/level.h"
#include <misc/stringops.h>
#include <misc/savePNG.h>
#include <faio/fafileobject.h>


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


namespace Render
{
    int32_t WIDTH = 1280;
    int32_t HEIGHT = 960;

    SDL_Window* screen;
    SDL_Renderer* renderer;
    SDL_GLContext glContext;

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
        if(screen == NULL)
            printf("Could not create window: %s\n", SDL_GetError());

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        glContext = SDL_GL_CreateContext(screen);
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

        initGlFuncs();

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        if (nk_ctx)
        {
            memset(&nuklearGraphics, 0, sizeof(nuklearGraphics));
            nk_sdl_device_create(nuklearGraphics.dev);
        }
    }

    void setWindowSize(const RenderSettings& settings)
    {
        SDL_SetWindowSize(screen, settings.windowWidth, settings.windowHeight);
    }


    void destroyNuklearGraphicsContext(NuklearGraphicsContext& nuklearGraphics)
    {
        nk_font_atlas_clear(&nuklearGraphics.atlas);
        nk_sdl_device_destroy(nuklearGraphics.dev);
    }

    void quit()
    {
        SDL_GL_DeleteContext(glContext);
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
        settings.windowWidth = 0;
        settings.windowHeight = 0;
        SDL_GetRendererOutputSize(renderer, &settings.windowWidth, &settings.windowHeight);
        //SDL_GetWindowSize(screen, &settings.windowWidth, &settings.windowHeight);
        return settings;
    }

    GLuint getGLTexFromSurface(SDL_Surface* surf)
    {
        GLenum data_fmt = GL_RGBA;
        /*Uint8 test = SDL_MapRGB(surf->format, 0xAA, 0xBB, 0xCC) & 0xFF;
        if (test == 0xAA) data_fmt = GL_RGB;
        else if (test == 0xCC) data_fmt = GL_BGR;//GL_BGR;
        else {
        printf("Error: \"Loaded surface was neither RGB or BGR!\""); return;
        }*/

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
            surf = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_ABGR8888, 0); // SDL is stupid and interprets pixel formats by endianness, so on LE, it calls RGBA ABGR...

        assert(surf->pitch == 4 * surf->w);

        GLuint tex = 0;

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, data_fmt, GL_UNSIGNED_BYTE, surf->pixels);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int32_t w, h;
        spriteSize((Sprite)(intptr_t)tex, w, h);

        if (!validFormat)
            SDL_FreeSurface(surf);

        return tex;
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
        for(i = path.length() - 1; i > 0; i--)
        {
            if(path[i] == '.')
                break;
        }

        return path.substr(i+1, path.length()-i);
    }


    bool getImageInfo(const std::string& path, std::vector<int32_t>& widths, std::vector<int32_t>& heights, int32_t& animLength)
    {
        //TODO: get better image decoders that allow you to peek image dimensions without loading full image

        std::string extension = getImageExtension(path);

        if(Misc::StringUtils::ciEqual(extension, "cel") || Misc::StringUtils::ciEqual(extension, "cl2"))
        {
            Cel::CelFile cel(path);
            widths.resize (cel.animLength()); heights.resize (cel.animLength());
            for (int i = 0; i < cel.animLength(); ++i)
                {
                    widths[i] = cel[i].mWidth;
                    heights[i] = cel[i].mHeight;
                }
            animLength = cel.animLength();
        }
        else
        {
            SDL_Surface* surface = loadNonCelImage(path, extension);

            if(surface)
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

        for(int32_t i = 0; i < cel.numFrames(); i++)
        {
            width += cel[i].mWidth;
            height = (cel[i].mHeight > height ? cel[i].mHeight : height);
        }

        assert(width > 0);
        assert(height > 0);

        SDL_Surface* surface = createTransparentSurface(width, height);

        int32_t x = 0;
        for(int32_t i = 0; i < cel.numFrames(); i++)
        {
            drawFrame(surface, x, 0, cel[i]);
            x += cel[i].mWidth;
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

    void drawCursor(Sprite s, CursorHotspotLocation hotspotLocation)
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
            int32_t w, h;
            spriteSize(s, w, h);
            int shiftX = 0;
            int shiftY = 0;
            switch (hotspotLocation) {
            case CursorHotspotLocation::topLeft: break;
            case CursorHotspotLocation::center:
                shiftX = w / 2;
                shiftY = h / 2;
                break;
            }
            drawSprite (s, x - shiftX, y - shiftY); // this shouldn't be the case for default cursor
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
        //SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
        //SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

        std::vector<Sprite> vec(1);
        vec[0] = (Sprite)(intptr_t)getGLTexFromSurface(surface);

        SDL_FreeSurface(surface);

        return new SpriteGroup(vec);
    }


    bool once = false;



    GLuint vao = 0;
    GLuint shader_programme = 0;
    GLuint texture = 0;

    void draw()
    {
        if (!once)
        {

            glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            once = true;

            float points[] = {
                0, 0, 0,
                1, 0, 0,
                1, 1, 0,
                0, 0, 0,
                1, 1, 0,
                0, 1, 0
            };





            float colours[] = {
                0, 0,
                1, 0,
                1, 1,
                0, 0,
                1, 1,
                0, 1
            };


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

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);




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

        //drawAt(texture, 0, 0);

        SDL_RenderPresent(renderer);
    }

    void drawSprite(GLuint sprite, int32_t x, int32_t y, boost::optional<Cel::Colour> highlightColor)
    {
        glUseProgram(shader_programme);

        auto setUniform = [](const char *name, double value)
        {
          GLint loc = glGetUniformLocation(shader_programme, name);
          if (loc != -1)
              {
                  glUniform1f(loc, value);
              }
        };

        setUniform ("width", WIDTH);
        setUniform ("height", HEIGHT);

        int32_t w, h;
        spriteSize((Sprite)(intptr_t)sprite, w, h);

        setUniform ("imgW", w);
        setUniform ("imgH", h);
        setUniform ("offsetX", x);
        setUniform ("offsetY", y);
        if (auto c = highlightColor) {
          setUniform ("h_color_r", c->r/255.f);
          setUniform ("h_color_g", c->g/255.f);
          setUniform ("h_color_b", c->b/255.f);
          setUniform ("h_color_a", 1.0f);
        }
        else
          setUniform ("h_color_a", 0.0f);

        glBindTexture(GL_TEXTURE_2D, sprite);

        glBindVertexArray(vao);
        // draw points 0-3 from the currently bound VAO with current in-use shader
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }


    void handleEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            //do nothing, just clear the event queue to avoid render window hang up in ubuntu.
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

        for(int32_t i = 0; i < cel.numFrames(); i++)
        {
            SDL_Surface* s = createTransparentSurface(cel[i].mWidth, cel[i].mHeight);
            drawFrame(s, 0, 0, cel[i]);

            mSprites.push_back((Render::Sprite)(intptr_t)getGLTexFromSurface(s));// SDL_CreateTextureFromSurface(renderer, s));

            SDL_FreeSurface(s);
        }


        mAnimLength = cel.animLength();
    }

    void SpriteGroup::toPng(const std::string& celPath, const std::string& pngPath)
    {
        Cel::CelFile cel(celPath);

        int32_t numFrames = cel.animLength();
        if(numFrames == 0)
            return;

        int32_t sumWidth = 0;
        int32_t maxHeight = 0;
        for(int32_t i = 0; i < numFrames; i++)
        {
            sumWidth += cel[i].mWidth;
            if(cel[i].mHeight > maxHeight) maxHeight = cel[i].mHeight;
        }

        if(sumWidth == 0)
            return;

        SDL_Surface* s = createTransparentSurface(sumWidth, maxHeight);
        unsigned int x = 0;
        unsigned int dx = 0;
        for(int32_t i = 0; i < numFrames; i++)
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
        for (size_t i = 0; i < mSprites.size(); i++)
        {
            GLuint tex = (GLuint)(intptr_t)mSprites[i];
            glDeleteTextures(1, &tex);
        }
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

            newMin[i] = (Sprite)(intptr_t)getGLTexFromSurface(newPillar);// NULL;// SDL_CreateTextureFromSurface(renderer, newPillar);
        }

        SDL_FreeSurface(newPillar);

        return new SpriteGroup(newMin);
    }

    void spriteSize(const Sprite& sprite, int32_t& w, int32_t& h)
    {
        GLint tmpW = 0, tmpH = 0;

        glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)sprite);

        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tmpW);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tmpH);
        //SDL_QueryTexture((SDL_Texture*)sprite, NULL, NULL, &tmpW, &tmpH);
        w = tmpW;
        h = tmpH;
    }

    void clear(int r, int g, int b)
    {
        glClearColor(((float)r)/255.0, ((float)g)/255.0, ((float)b)/255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
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
        for(int32_t x = 0; x < frame.mWidth; x++)
        {
            for(int32_t y = 0; y < frame.mHeight; y++)
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
         auto x = std::div (2 * point.y + point.x, tileWidth); // division by 64 is pretty fast btw
         auto y = std::div (2 * point.y - point.x, tileWidth);
         return {x.quot, y.quot, x.rem > y.rem ? TileHalf::right : TileHalf::left};
    }

    static Misc::Point pointBetween (const Tile &start, const Tile &finish, const size_t &percent) {
      auto pointA = tileTopPoint (start);
      auto pointB = tileTopPoint (finish);
      return pointA + (pointB - pointA) * (percent * 0.01);
    }

    static void drawMovingSprite(const Sprite& sprite, const Tile& start, const Tile& finish, size_t dist, const Misc::Point& toScreen, boost::optional<Cel::Colour> highlightColor = boost::none)
    {
        int32_t w, h;
        spriteSize(sprite, w, h);
        auto point = pointBetween (start, finish, dist);
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

    void drawLevel(const Level::Level& level, size_t minTopsHandle, size_t minBottomsHandle, SpriteCacheBase* cache, LevelObjects& objs, LevelObjects
                   & items, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist) {
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

        auto &itemsForTile = items[tile.x][tile.y];
        for (auto &item : itemsForTile)
            {
                int32_t w, h;
                auto sprite = (*cache->get(item.spriteCacheIndex))[item.spriteFrame];
                spriteSize(sprite, w, h);
                drawAtTile(sprite, topLeft, w, h, item.hoverColor);
            }

        auto &objsForTile = objs[tile.x][tile.y];
        for (auto &obj : objsForTile) {
            if (obj.valid)
                drawMovingSprite((*cache->get(obj.spriteCacheIndex))[obj.spriteFrame], tile, {obj.x2, obj.y2}, obj.dist, toScreen, obj.hoverColor);
        }
      });

      cache->setImmortal(minTopsHandle, false);
    }
}
