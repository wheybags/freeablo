#include "../cel/celfile.h"
#include "../level/level.h"
#include "atlastexture.h"
#include "nuklear_sdl_gl3.h"
#include "render.h"
#include "vertextypes.h"
#include <SDL.h>
#include <cel/tilesetimage.h>
#include <fa_nuklear.h>
#include <faio/fafileobject.h>
#include <misc/assert.h>
#include <misc/savePNG.h>
#include <misc/stringops.h>
#include <render/alignedcpubuffer.h>
#include <render/buffer.h>
#include <render/commandqueue.h>
#include <render/pipeline.h>
#include <render/renderinstance.h>
#include <render/texture.h>
#include <render/vertexarrayobject.h>

#if defined(WIN32) || defined(_WIN32)
extern "C" {
__declspec(dllexport) unsigned NvOptimusEnablement = 0x00000001;
}
extern "C" {
__declspec(dllexport) unsigned AmdPowerXpressRequestHighPerformance = 0x00000001;
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
    // atlasTexture is unique_ptr as to not instantiate until opengl is setup.
    std::unique_ptr<AtlasTexture> atlasTexture;

    namespace DrawLevelUniforms
    {
        struct Vertex
        {
            float screenSize[2];

            float pad1[2];
        };

        struct Fragment
        {
            float atlasSize[2];

            float pad2[2];
        };

        using CpuBufferType = TypedAlignedCpuBuffer<Vertex, Fragment>;
    }

    int32_t WIDTH = 1280;
    int32_t HEIGHT = 960;

    SDL_Window* screen;
    // SDL_Renderer* renderer;
    RenderInstance* renderInstance = nullptr;
    RenderInstance* mainRenderInstance = nullptr;

    CommandQueue* mainCommandQueue = nullptr;
    VertexArrayObject* vertexArrayObject = nullptr;
    Pipeline* drawLevelPipeline = nullptr;
    Buffer* drawLevelUniformBuffer = nullptr;
    DrawLevelUniforms::CpuBufferType* drawLevelUniformCpuBuffer = nullptr;
    DescriptorSet* drawLevelDescriptorSet = nullptr;

    /* Caches level sprites/positions etc in a format that can be directly injected into GL VBOs. */
    class DrawLevelCache
    {
    public:
        void addSprite(uint32_t sprite, int32_t x, int32_t y, std::optional<Cel::Colour> highlightColor)
        {
            const AtlasTextureLookupMap& lookupMap = atlasTexture->getLookupMap();
            const AtlasTextureEntry& atlasEntry = lookupMap.at(sprite);

            mSpritesToDraw.push_back(SpriteData{&atlasEntry, x, y, highlightColor});
        }

        void end()
        {
            for (size_t i = 0; i < mSpritesToDraw.size(); i++)
                mSpritesToDraw[i].zBufferValue = 1.0f - (i / float(mSpritesToDraw.size()));

            // explicit z buffer values ensure the draws act like they were done in-order, so we're free to batch by texture as aggressively as possible
            auto sortByTexture = [](const SpriteData& a, const SpriteData& b) { return a.atlasEntry->mTexture < b.atlasEntry->mTexture; };
            std::sort(mSpritesToDraw.begin(), mSpritesToDraw.end(), sortByTexture);

            for (const auto& spriteData : mSpritesToDraw)
                batchDrawSprite(*spriteData.atlasEntry, spriteData.x, spriteData.y, spriteData.highlightColor, spriteData.zBufferValue);

            mSpritesToDraw.clear();

            draw();
        }

    private:
        void batchDrawSprite(const AtlasTextureEntry& atlasEntry, int32_t x, int32_t y, std::optional<Cel::Colour> highlightColor, float zBufferVal)
        {
            if (atlasEntry.mTexture != mTexture)
                draw();

            mTexture = atlasEntry.mTexture;
            debug_assert(mTexture);

            SpriteVertexPerInstance vertexData = {};

            vertexData.v_imageSize[0] = atlasEntry.mTrimmedWidth;
            vertexData.v_imageSize[1] = atlasEntry.mTrimmedHeight;

            vertexData.v_atlasOffset[0] = atlasEntry.mX;
            vertexData.v_atlasOffset[1] = atlasEntry.mY;
            vertexData.v_atlasOffset[2] = atlasEntry.mLayer;

            vertexData.v_imageOffset[0] = x + atlasEntry.mTrimmedOffsetX;
            vertexData.v_imageOffset[1] = y + atlasEntry.mTrimmedOffsetY;

            vertexData.v_zValue = zBufferVal;

            if (auto c = highlightColor)
            {
                vertexData.v_hoverColor[0] = c->r;
                vertexData.v_hoverColor[1] = c->g;
                vertexData.v_hoverColor[2] = c->b;
                vertexData.v_hoverColor[3] = 255;
            }

            mInstanceData.push_back(vertexData);
        }

        void draw()
        {
            if (mInstanceData.empty())
                return;

            auto vertexUniforms = drawLevelUniformCpuBuffer->getMemberPointer<DrawLevelUniforms::Vertex>();
            vertexUniforms->screenSize[0] = WIDTH;
            vertexUniforms->screenSize[1] = HEIGHT;

            auto fragmentUniforms = drawLevelUniformCpuBuffer->getMemberPointer<DrawLevelUniforms::Fragment>();
            fragmentUniforms->atlasSize[0] = mTexture->width();
            fragmentUniforms->atlasSize[1] = mTexture->height();

            drawLevelUniformBuffer->setData(drawLevelUniformCpuBuffer->data(), drawLevelUniformCpuBuffer->getSizeInBytes());

            vertexArrayObject->getVertexBuffer(1)->setData(mInstanceData.data(), mInstanceData.size() * sizeof(SpriteVertexPerInstance));

            drawLevelDescriptorSet->updateItems({
                {2, mTexture},
            });

            Bindings bindings;
            bindings.vao = vertexArrayObject;
            bindings.pipeline = drawLevelPipeline;
            bindings.descriptorSet = drawLevelDescriptorSet;

            mainCommandQueue->cmdDrawInstances(0, 6, mInstanceData.size(), bindings);

            mInstanceData.clear();
            mTexture = nullptr;
        }

    private:
        struct SpriteData
        {
            const AtlasTextureEntry* atlasEntry = nullptr;
            int32_t x = 0;
            int32_t y = 0;
            std::optional<Cel::Colour> highlightColor;
            float zBufferValue = 0;
        };

        std::vector<SpriteData> mSpritesToDraw;
        std::vector<SpriteVertexPerInstance> mInstanceData;
        Texture* mTexture = nullptr;
    };

    DrawLevelCache drawLevelCache;
    std::string windowTitle;

    void init(const std::string& title, const RenderSettings& settings, NuklearGraphicsContext& nuklearGraphics, nk_context* nk_ctx)
    {
        WIDTH = settings.windowWidth;
        HEIGHT = settings.windowHeight;
        int flags = SDL_WINDOW_OPENGL;
        windowTitle = title;

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
        if (screen == nullptr)
            printf("Could not create window: %s\n", SDL_GetError());

        renderInstance = RenderInstance::createRenderInstance(RenderInstance::Type::OpenGL, *screen);
        mainCommandQueue = renderInstance->createCommandQueue().release();
        mainCommandQueue->begin();

        mainRenderInstance = renderInstance;

        PipelineSpec drawLevelPipelineSpec;
        drawLevelPipelineSpec.depthTest = true;
        drawLevelPipelineSpec.vertexLayouts = {SpriteVertexMain::layout(), SpriteVertexPerInstance::layout()};
        drawLevelPipelineSpec.vertexShaderPath = Misc::getResourcesPath().str() + "/shaders/basic.vert";
        drawLevelPipelineSpec.fragmentShaderPath = Misc::getResourcesPath().str() + "/shaders/basic.frag";
        drawLevelPipelineSpec.descriptorSetSpec = {{
            {DescriptorType::UniformBuffer, "vertexUniforms"},
            {DescriptorType::UniformBuffer, "fragmentUniforms"},
            {DescriptorType::Texture, "tex"},
        }};

        drawLevelPipeline = renderInstance->createPipeline(drawLevelPipelineSpec).release();
        vertexArrayObject = renderInstance->createVertexArrayObject({0, 0}, drawLevelPipelineSpec.vertexLayouts, 0).release();

        drawLevelUniformCpuBuffer = new DrawLevelUniforms::CpuBufferType(renderInstance->capabilities().uniformBufferOffsetAlignment);

        drawLevelUniformBuffer = renderInstance->createBuffer(drawLevelUniformCpuBuffer->getSizeInBytes()).release();
        drawLevelDescriptorSet = renderInstance->createDescriptorSet(drawLevelPipelineSpec.descriptorSetSpec).release();
        atlasTexture = std::make_unique<AtlasTexture>(*renderInstance, *mainCommandQueue);

        // clang-format off

        drawLevelDescriptorSet->updateItems({
            {0, BufferSlice{drawLevelUniformBuffer, drawLevelUniformCpuBuffer->getMemberOffset<DrawLevelUniforms::Vertex>(), sizeof(DrawLevelUniforms::Vertex)}},
            {1, BufferSlice{drawLevelUniformBuffer, drawLevelUniformCpuBuffer->getMemberOffset<DrawLevelUniforms::Fragment>(), sizeof(DrawLevelUniforms::Fragment)}},
        });

        SpriteVertexMain baseVertices[] =
        {
            {{0, 0, 0},  {0, 0}},
            {{1, 0, 0},  {1, 0}},
            {{1, 1, 0},  {1, 1}},
            {{0, 0, 0},  {0, 0}},
            {{1, 1, 0},  {1, 1}},
            {{0, 1, 0},  {0, 1}}
        };
        // clang-format on
        vertexArrayObject->getVertexBuffer(0)->setData(baseVertices, sizeof(baseVertices));

        // Update screen with/height, as starting full screen window in
        // Windows does not trigger a SDL_WINDOWEVENT_RESIZED event.
        SDL_GetWindowSize(screen, &WIDTH, &HEIGHT);

        if (nk_ctx)
        {
            nuklearGraphics = {};
            nk_sdl_device_create(nuklearGraphics.dev, *renderInstance);
        }
    }

    const std::string& getWindowTitle() { return windowTitle; }
    void setWindowTitle(const std::string& title) { SDL_SetWindowTitle(screen, title.c_str()); }

    void destroyNuklearGraphicsContext(NuklearGraphicsContext& nuklearGraphics)
    {
        nk_font_atlas_clear(&nuklearGraphics.atlas);
        nk_sdl_device_destroy(nuklearGraphics.dev);
    }

    void quit()
    {
        atlasTexture.reset();
        delete drawLevelUniformCpuBuffer;
        delete drawLevelDescriptorSet;
        delete drawLevelUniformBuffer;
        delete vertexArrayObject;
        delete drawLevelPipeline;
        delete mainCommandQueue;
        delete renderInstance;
        // SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(screen);
        SDL_Quit();
    }

    void resize(size_t w, size_t h)
    {
        WIDTH = w;
        HEIGHT = h;

        renderInstance->onWindowResized(WIDTH, HEIGHT);
    }

    RenderSettings getWindowSize()
    {
        RenderSettings settings;
        settings.windowWidth = WIDTH;
        settings.windowHeight = HEIGHT;
        return settings;
    }

    void drawGui(NuklearFrameDump& dump) { nk_sdl_render_dump(dump, screen, *atlasTexture, *mainCommandQueue); }

    void draw()
    {
        mainCommandQueue->cmdPresent();
        mainCommandQueue->end();
        mainCommandQueue->submit();
        mainCommandQueue->begin();
    }

    void drawSprite(uint32_t sprite, int32_t x, int32_t y, std::optional<Cel::Colour> highlightColor)
    {
        // Add to level cache, will be drawn in a batch later.
        drawLevelCache.addSprite(sprite, x, y, highlightColor);
    }

    void drawSprite(const Sprite& sprite, int32_t x, int32_t y, std::optional<Cel::Colour> highlightColor)
    {
        drawSprite((uint32_t)(intptr_t)sprite, x, y, highlightColor);
    }

    constexpr auto tileHeight = 32;
    constexpr auto tileWidth = tileHeight * 2;

    void drawAtTile(const Sprite& sprite, const Misc::Point& tileTop, int spriteW, int spriteH, std::optional<Cel::Colour> highlightColor = std::nullopt)
    {
        // centering spright at the center of tile by width and at the bottom of tile by height
        drawSprite(sprite, tileTop.x - spriteW / 2, tileTop.y - spriteH + tileHeight, highlightColor);
    }

    SpriteGroup::SpriteGroup(std::vector<Sprite>&& sprites, int32_t animLength) : mSprites(std::move(sprites))
    {
        if (animLength != -1)
            mAnimLength = animLength;
        else
            mAnimLength = mSprites.size();

        debug_assert(!mSprites.empty());
        spriteSize(mSprites[0], mWidth, mHeight);
    }

    Sprite& SpriteGroup::operator[](size_t index)
    {
        debug_assert(index < mSprites.size());
        return mSprites[index];
    }
    void spriteSize(const Sprite& sprite, int32_t& w, int32_t& h)
    {
        auto& atlasEntry = atlasTexture->getLookupMap().at((size_t)(intptr_t)sprite);
        w = atlasEntry.mWidth;
        h = atlasEntry.mHeight;
    }

    void clear(int r, int g, int b)
    {
        mainCommandQueue->cmdClearCurrentFramebuffer(Color(float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f, 1.0f), true);
    }

    // basic transform of isometric grid to normal, (0, 0) tile coordinate maps to (0, 0) pixel coordinates
    // since eventually we're gonna shift coordinates to viewport center, it's better to keep transform itself
    // as simple as possible
    template <typename T> static Vec2i tileTopPoint(Vec2<T> tile)
    {
        return Vec2i(Vec2<T>(T(tileWidth / 2) * (tile.x - tile.y), (tile.y + tile.x) * (tileHeight / 2)));
    }

    // this function simply does the reverse of the above function, could be found by solving linear equation system
    // it obviously uses the fact that ttileWidth = tileHeight * 2
    Tile getTileFromScreenCoords(const Misc::Point& screenPos, const Misc::Point& toScreen)
    {
        auto point = screenPos - toScreen;
        auto x = std::div(2 * point.y + point.x, tileWidth); // division by 64 is pretty fast btw
        auto y = std::div(2 * point.y - point.x, tileWidth);
        return {x.quot, y.quot, x.rem > y.rem ? TileHalf::right : TileHalf::left};
    }

    static void
    drawMovingSprite(const Sprite& sprite, const Vec2Fix& fractionalPos, const Misc::Point& toScreen, std::optional<Cel::Colour> highlightColor = std::nullopt)
    {
        int32_t w, h;
        spriteSize(sprite, w, h);
        Vec2i point = tileTopPoint(fractionalPos);
        Vec2i res = point + toScreen;

        drawAtTile(sprite, Vec2i(res), w, h, highlightColor);
    }

    constexpr auto bottomMenuSize = 144; // TODO: pass it as a variable
    Misc::Point worldToScreenVector(const Vec2Fix& fractionalPos)
    {
        // centering takes in accord bottom menu size to be consistent with original game centering
        Vec2i point = tileTopPoint(fractionalPos);

        return Misc::Point{WIDTH / 2, (HEIGHT - bottomMenuSize) / 2} - point;
    }

    Tile getTileByScreenPos(size_t x, size_t y, const Vec2Fix& fractionalPos)
    {
        Misc::Point toScreen = worldToScreenVector(fractionalPos);
        return getTileFromScreenCoords({static_cast<int32_t>(x), static_cast<int32_t>(y)}, toScreen);
    }

    constexpr auto staticObjectHeight = 256;

    template <typename ProcessTileFunc> void drawObjectsByTiles(const Misc::Point& toScreen, ProcessTileFunc processTile)
    {
        Misc::Point start{-2 * tileWidth, -2 * tileHeight};
        Tile startingTile = getTileFromScreenCoords(start, toScreen);

        Misc::Point startingPoint = tileTopPoint(startingTile.pos) + toScreen;
        auto processLine = [&]() {
            Misc::Point point = startingPoint;
            Tile tile = startingTile;

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
                   SpriteGroup* minTops,
                   SpriteGroup* minBottoms,
                   SpriteGroup* specialSprites,
                   const std::map<int32_t, int32_t>& specialSpritesMap,
                   LevelObjects& objs,
                   LevelObjects& items,
                   const Vec2Fix& fractionalPos)
    {
        auto toScreen = worldToScreenVector(fractionalPos);
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
                    Sprite& sprite = (*specialSprites)[specialSpriteIndex];
                    int w, h;
                    spriteSize(sprite, w, h);
                    drawAtTile(sprite, topLeft, w, h);
                }
            }

            auto& itemsForTile = items.get(tile.pos.x, tile.pos.y);
            for (auto& item : itemsForTile)
            {
                int32_t w, h;
                const Sprite& sprite = item.sprite->operator[](item.spriteFrame);
                spriteSize(sprite, w, h);
                drawAtTile(sprite, topLeft, w, h, item.hoverColor);
            }

            auto& objsForTile = objs.get(tile.pos.x, tile.pos.y);
            for (auto& obj : objsForTile)
            {
                if (obj.valid)
                {
                    const Sprite& sprite = obj.sprite->operator[](obj.spriteFrame);
                    drawMovingSprite(sprite, obj.fractionalPos, toScreen, obj.hoverColor);
                }
            }
        });

        drawLevelCache.end();
    }
}
