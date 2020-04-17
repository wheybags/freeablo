#include "levelrenderer.h"
#include <level/level.h>
#include <render/commandqueue.h>
#include <render/levelobjects.h>
#include <render/pipeline.h>
#include <render/render.h>
#include <render/renderinstance.h>
#include <render/texture.h>
#include <render/vertexarrayobject.h>
#include <render/vertextypes.h>

class Vertex;
namespace FARender
{
    static Vec2i getCurrentResolution() { return {Render::getWindowSize().windowWidth, Render::getWindowSize().windowHeight}; }

    void DrawLevelCache::addSprite(Render::Sprite atlasEntry, int32_t x, int32_t y, std::optional<Cel::Colour> highlightColor)
    {
        mSpritesToDraw.push_back(SpriteData{atlasEntry, x, y, highlightColor});
    }

    void DrawLevelCache::end(DrawLevelUniforms::CpuBufferType* drawLevelUniformCpuBuffer,
                             Render::Buffer* drawLevelUniformBuffer,
                             Render::VertexArrayObject* vertexArrayObject,
                             Render::DescriptorSet* drawLevelDescriptorSet,
                             Render::Pipeline* drawLevelPipeline)
    {
        for (size_t i = 0; i < mSpritesToDraw.size(); i++)
            mSpritesToDraw[i].zBufferValue = 1.0f - (i / float(mSpritesToDraw.size()));

        // explicit z buffer values ensure the draws act like they were done in-order, so we're free to batch by texture as aggressively as possible
        auto sortByTexture = [](const SpriteData& a, const SpriteData& b) { return a.atlasEntry->mTexture < b.atlasEntry->mTexture; };
        std::sort(mSpritesToDraw.begin(), mSpritesToDraw.end(), sortByTexture);

        for (const auto& spriteData : mSpritesToDraw)
            batchDrawSprite(*spriteData.atlasEntry,
                            spriteData.x,
                            spriteData.y,
                            spriteData.highlightColor,
                            spriteData.zBufferValue,
                            drawLevelUniformCpuBuffer,
                            drawLevelUniformBuffer,
                            vertexArrayObject,
                            drawLevelDescriptorSet,
                            drawLevelPipeline);

        mSpritesToDraw.clear();

        draw(drawLevelUniformCpuBuffer, drawLevelUniformBuffer, vertexArrayObject, drawLevelDescriptorSet, drawLevelPipeline);
    }

    void DrawLevelCache::batchDrawSprite(const Render::AtlasTextureEntry& atlasEntry,
                                         int32_t x,
                                         int32_t y,
                                         std::optional<Cel::Colour> highlightColor,
                                         float zBufferVal,
                                         DrawLevelUniforms::CpuBufferType* drawLevelUniformCpuBuffer,
                                         Render::Buffer* drawLevelUniformBuffer,
                                         Render::VertexArrayObject* vertexArrayObject,
                                         Render::DescriptorSet* drawLevelDescriptorSet,
                                         Render::Pipeline* drawLevelPipeline)
    {
        if (atlasEntry.mTexture != mTexture)
            draw(drawLevelUniformCpuBuffer, drawLevelUniformBuffer, vertexArrayObject, drawLevelDescriptorSet, drawLevelPipeline);

        mTexture = atlasEntry.mTexture;

        Render::SpriteVertexPerInstance vertexData = {};

        vertexData.v_imageSize[0] = atlasEntry.mTrimmedWidth;
        vertexData.v_imageSize[1] = atlasEntry.mTrimmedHeight;

        vertexData.v_atlasOffset[0] = atlasEntry.mX;
        vertexData.v_atlasOffset[1] = atlasEntry.mY;

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

    void DrawLevelCache::draw(DrawLevelUniforms::CpuBufferType* drawLevelUniformCpuBuffer,
                              Render::Buffer* drawLevelUniformBuffer,
                              Render::VertexArrayObject* vertexArrayObject,
                              Render::DescriptorSet* drawLevelDescriptorSet,
                              Render::Pipeline* drawLevelPipeline)
    {
        if (mInstanceData.empty())
            return;

        auto vertexUniforms = drawLevelUniformCpuBuffer->getMemberPointer<DrawLevelUniforms::Vertex>();
        vertexUniforms->screenSize[0] = getCurrentResolution().w;
        vertexUniforms->screenSize[1] = getCurrentResolution().h;

        auto fragmentUniforms = drawLevelUniformCpuBuffer->getMemberPointer<DrawLevelUniforms::Fragment>();
        fragmentUniforms->atlasSize[0] = mTexture->width();
        fragmentUniforms->atlasSize[1] = mTexture->height();

        drawLevelUniformBuffer->setData(drawLevelUniformCpuBuffer->data(), drawLevelUniformCpuBuffer->getSizeInBytes());

        vertexArrayObject->getVertexBuffer(1)->setData(mInstanceData.data(), mInstanceData.size() * sizeof(Render::SpriteVertexPerInstance));

        drawLevelDescriptorSet->updateItems({
            {2, mTexture},
        });

        Render::Bindings bindings;
        bindings.vao = vertexArrayObject;
        bindings.pipeline = drawLevelPipeline;
        bindings.descriptorSet = drawLevelDescriptorSet;

        Render::mainCommandQueue->cmdDrawInstances(0, 6, mInstanceData.size(), bindings);

        mInstanceData.clear();
        mTexture = nullptr;
    }

    constexpr auto tileHeight = 32;
    constexpr auto tileWidth = tileHeight * 2;
    constexpr auto staticObjectHeight = 256;

    void LevelRenderer::drawAtTile(Render::Sprite sprite, const Misc::Point& tileTop, int spriteW, int spriteH, std::optional<Cel::Colour> highlightColor)
    {
        // centering sprite at the center of tile by width and at the bottom of tile by height
        mDrawLevelCache.addSprite(sprite, tileTop.x - spriteW / 2, tileTop.y - spriteH + tileHeight, highlightColor);
    }

    // basic transform of isometric grid to normal, (0, 0) tile coordinate maps to (0, 0) pixel coordinates
    // since eventually we're gonna shift coordinates to viewport center, it's better to keep transform itself
    // as simple as possible
    template <typename T> static Vec2i tileTopPoint(Vec2<T> tile)
    {
        return Vec2i(Vec2<T>(T(tileWidth / 2) * (tile.x - tile.y), (tile.y + tile.x) * (tileHeight / 2)));
    }

    // this function simply does the reverse of the above function, could be found by solving linear equation system
    // it obviously uses the fact that tileWidth = tileHeight * 2
    Render::Tile getTileFromScreenCoords(const Misc::Point& screenPos, const Misc::Point& toScreen)
    {
        auto point = screenPos - toScreen;
        auto x = std::div(2 * point.y + point.x, tileWidth); // division by 64 is pretty fast btw
        auto y = std::div(2 * point.y - point.x, tileWidth);
        return {x.quot, y.quot, x.rem > y.rem ? Render::TileHalf::right : Render::TileHalf::left};
    }

    void
    LevelRenderer::drawMovingSprite(Render::Sprite sprite, const Vec2Fix& fractionalPos, const Misc::Point& toScreen, std::optional<Cel::Colour> highlightColor)
    {
        Vec2i point = tileTopPoint(fractionalPos);
        Vec2i res = point + toScreen;

        drawAtTile(sprite, Vec2i(res), sprite->mWidth, sprite->mHeight, highlightColor);
    }

    constexpr auto bottomMenuSize = 144; // TODO: pass it as a variable
    static Misc::Point worldToScreenVector(const Vec2Fix& fractionalPos)
    {
        // centering takes in accord bottom menu size to be consistent with original game centering
        Vec2i point = tileTopPoint(fractionalPos);

        Vec2i resolution = getCurrentResolution();
        return Misc::Point{resolution.w / 2, (resolution.h - bottomMenuSize) / 2} - point;
    }

    Render::Tile LevelRenderer::getTileByScreenPos(size_t x, size_t y, const Vec2Fix& fractionalPos)
    {
        Misc::Point toScreen = worldToScreenVector(fractionalPos);
        return getTileFromScreenCoords({static_cast<int32_t>(x), static_cast<int32_t>(y)}, toScreen);
    }

    template <typename ProcessTileFunc> void drawObjectsByTiles(const Misc::Point& toScreen, ProcessTileFunc processTile)
    {
        Misc::Point start{-2 * tileWidth, -2 * tileHeight};
        Render::Tile startingTile = getTileFromScreenCoords(start, toScreen);

        Vec2i resolution = getCurrentResolution();

        Misc::Point startingPoint = tileTopPoint(startingTile.pos) + toScreen;
        auto processLine = [&]() {
            Misc::Point point = startingPoint;
            Render::Tile tile = startingTile;

            while (point.x < resolution.w + tileWidth / 2)
            {
                point.x += tileWidth;
                ++tile.pos.x;
                --tile.pos.y;
                processTile(tile, point);
            }
        };

        // then from top left to top-bottom
        while (startingPoint.y < resolution.h + staticObjectHeight - tileHeight)
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

    void LevelRenderer::drawLevel(const Level::Level& level,
                                  Render::SpriteGroup* minTops,
                                  Render::SpriteGroup* minBottoms,
                                  Render::SpriteGroup* specialSprites,
                                  const std::map<int32_t, int32_t>& specialSpritesMap,
                                  Render::LevelObjects& objs,
                                  Render::LevelObjects& items,
                                  const Vec2Fix& fractionalPos)
    {
        auto toScreen = worldToScreenVector(fractionalPos);
        auto isInvalidTile = [&](const Render::Tile& tile) {
            return tile.pos.x < 0 || tile.pos.y < 0 || tile.pos.x >= static_cast<int32_t>(level.width()) || tile.pos.y >= static_cast<int32_t>(level.height());
        };

        // drawing on the ground objects
        drawObjectsByTiles(toScreen, [&](const Render::Tile& tile, const Misc::Point& topLeft) {
            if (isInvalidTile(tile))
            {
                drawAtTile((*minBottoms)[0], topLeft, tileWidth, staticObjectHeight);
                return;
            }

            size_t index = level.get(tile.pos).index();
            if (index < minBottoms->size())
                drawAtTile((*minBottoms)[index], topLeft, tileWidth, staticObjectHeight); // all static objects have the same sprite size
        });

        // drawing above the ground and moving object
        drawObjectsByTiles(toScreen, [&](const Render::Tile& tile, const Misc::Point& topLeft) {
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
                    Render::Sprite& sprite = (*specialSprites)[specialSpriteIndex];
                    drawAtTile(sprite, topLeft, sprite->mWidth, sprite->mHeight);
                }
            }

            auto& itemsForTile = items.get(tile.pos.x, tile.pos.y);
            for (auto& item : itemsForTile)
            {
                const Render::Sprite& sprite = item.sprite->operator[](item.spriteFrame);
                drawAtTile(sprite, topLeft, sprite->mWidth, sprite->mHeight, item.hoverColor);
            }

            auto& objsForTile = objs.get(tile.pos.x, tile.pos.y);
            for (auto& obj : objsForTile)
            {
                if (obj.valid)
                {
                    const Render::Sprite& sprite = obj.sprite->operator[](obj.spriteFrame);
                    drawMovingSprite(sprite, obj.fractionalPos, toScreen, obj.hoverColor);
                }
            }
        });

        mDrawLevelCache.end(drawLevelUniformCpuBuffer, drawLevelUniformBuffer, vertexArrayObject, drawLevelDescriptorSet, drawLevelPipeline);
    }
    LevelRenderer::LevelRenderer()
    {
        Render::PipelineSpec drawLevelPipelineSpec;
        drawLevelPipelineSpec.depthTest = true;
        drawLevelPipelineSpec.vertexLayouts = {Render::SpriteVertexMain::layout(), Render::SpriteVertexPerInstance::layout()};
        drawLevelPipelineSpec.vertexShaderPath = Misc::getResourcesPath().str() + "/shaders/basic.vert";
        drawLevelPipelineSpec.fragmentShaderPath = Misc::getResourcesPath().str() + "/shaders/basic.frag";
        drawLevelPipelineSpec.descriptorSetSpec = {{
            {Render::DescriptorType::UniformBuffer, "vertexUniforms"},
            {Render::DescriptorType::UniformBuffer, "fragmentUniforms"},
            {Render::DescriptorType::Texture, "tex"},
        }};

        drawLevelPipeline = Render::mainRenderInstance->createPipeline(drawLevelPipelineSpec).release();
        vertexArrayObject = Render::mainRenderInstance->createVertexArrayObject({0, 0}, drawLevelPipelineSpec.vertexLayouts, 0).release();

        drawLevelUniformCpuBuffer = new DrawLevelUniforms::CpuBufferType(Render::mainRenderInstance->capabilities().uniformBufferOffsetAlignment);

        drawLevelUniformBuffer = Render::mainRenderInstance->createBuffer(drawLevelUniformCpuBuffer->getSizeInBytes()).release();
        drawLevelDescriptorSet = Render::mainRenderInstance->createDescriptorSet(drawLevelPipelineSpec.descriptorSetSpec).release();

        // clang-format off
        drawLevelDescriptorSet->updateItems(
        {
            {0, Render::BufferSlice{drawLevelUniformBuffer, drawLevelUniformCpuBuffer->getMemberOffset<DrawLevelUniforms::Vertex>(), sizeof(DrawLevelUniforms::Vertex)}},
            {1, Render::BufferSlice{drawLevelUniformBuffer, drawLevelUniformCpuBuffer->getMemberOffset<DrawLevelUniforms::Fragment>(), sizeof(DrawLevelUniforms::Fragment)}},
        });

        Render::SpriteVertexMain baseVertices[] =
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
    }
}
