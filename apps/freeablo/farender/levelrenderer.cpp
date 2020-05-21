#include "levelrenderer.h"
#include <level/level.h>
#include <render/commandqueue.h>
#include <render/debugrenderer.h>
#include <render/framebuffer.h>
#include <render/pipeline.h>
#include <render/render.h>
#include <render/renderinstance.h>
#include <render/spritegroup.h>
#include <render/texture.h>
#include <render/vertexarrayobject.h>
#include <render/vertextypes.h>

class Vertex;
namespace FARender
{
    static Vec2i getCurrentResolution() { return {Render::getWindowSize().windowWidth, Render::getWindowSize().windowHeight}; }

    void DrawLevelCache::addSprite(const Render::TextureReference* atlasEntry, int32_t x, int32_t y, std::optional<ByteColour> highlightColor)
    {
        mSpritesToDraw.push_back(SpriteData{atlasEntry, x, y, highlightColor});
    }

    void DrawLevelCache::end(DrawLevelUniforms::CpuBufferType& drawLevelUniformCpuBuffer,
                             Render::Buffer& drawLevelUniformBuffer,
                             Render::VertexArrayObject& vertexArrayObject,
                             Render::DescriptorSet& drawLevelDescriptorSet,
                             Render::Pipeline& drawLevelPipeline,
                             Render::Framebuffer* nonDefaultFramebuffer)
    {
        for (size_t i = 0; i < mSpritesToDraw.size(); i++)
            mSpritesToDraw[i].zBufferValue = 1.0f - ((i + 1) / float(mSpritesToDraw.size() + 2));

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
                            drawLevelPipeline,
                            nonDefaultFramebuffer);

        mSpritesToDraw.clear();

        draw(drawLevelUniformCpuBuffer, drawLevelUniformBuffer, vertexArrayObject, drawLevelDescriptorSet, drawLevelPipeline, nonDefaultFramebuffer);
    }

    void DrawLevelCache::batchDrawSprite(const Render::TextureReference& atlasEntry,
                                         int32_t x,
                                         int32_t y,
                                         std::optional<ByteColour> highlightColor,
                                         float zBufferVal,
                                         DrawLevelUniforms::CpuBufferType& drawLevelUniformCpuBuffer,
                                         Render::Buffer& drawLevelUniformBuffer,
                                         Render::VertexArrayObject& vertexArrayObject,
                                         Render::DescriptorSet& drawLevelDescriptorSet,
                                         Render::Pipeline& drawLevelPipeline,
                                         Render::Framebuffer* nonDefaultFramebuffer)
    {
        if (atlasEntry.mTexture != mTexture)
            draw(drawLevelUniformCpuBuffer, drawLevelUniformBuffer, vertexArrayObject, drawLevelDescriptorSet, drawLevelPipeline, nonDefaultFramebuffer);

        mTexture = atlasEntry.mTexture;

        Render::SpriteVertexPerInstance vertexData = {};

        vertexData.v_spriteSizeInPixels[0] = atlasEntry.mTrimmedWidth;
        vertexData.v_spriteSizeInPixels[1] = atlasEntry.mTrimmedHeight;

        vertexData.v_atlasOffsetInPixels[0] = atlasEntry.mX;
        vertexData.v_atlasOffsetInPixels[1] = atlasEntry.mY;

        vertexData.v_destinationInPixels[0] = x + atlasEntry.mTrimmedOffsetX;
        vertexData.v_destinationInPixels[1] = y + atlasEntry.mTrimmedOffsetY;

        vertexData.v_zValue = zBufferVal;

        if (auto c = highlightColor)
        {
            // This forces a buffer around the texture being drawn so we can fit the outline.
            int32_t pad = Render::AtlasTexture::PADDING - 1;
            vertexData.v_spriteSizeInPixels[0] += pad * 2;
            vertexData.v_spriteSizeInPixels[1] += pad * 2;
            vertexData.v_atlasOffsetInPixels[0] -= pad;
            vertexData.v_atlasOffsetInPixels[1] -= pad;
            vertexData.v_destinationInPixels[0] -= pad;
            vertexData.v_destinationInPixels[1] -= pad;

            vertexData.v_hoverColor[0] = c->r;
            vertexData.v_hoverColor[1] = c->g;
            vertexData.v_hoverColor[2] = c->b;
            vertexData.v_hoverColor[3] = 255;
        }

        mInstanceData.push_back(vertexData);
    }

    void DrawLevelCache::draw(DrawLevelUniforms::CpuBufferType& drawLevelUniformCpuBuffer,
                              Render::Buffer& drawLevelUniformBuffer,
                              Render::VertexArrayObject& vertexArrayObject,
                              Render::DescriptorSet& drawLevelDescriptorSet,
                              Render::Pipeline& drawLevelPipeline,
                              Render::Framebuffer* nonDefaultFramebuffer)
    {
        if (mInstanceData.empty())
            return;

        auto vertexUniforms = drawLevelUniformCpuBuffer.getMemberPointer<DrawLevelUniforms::Vertex>();
        vertexUniforms->screenSizeInPixels[0] = getCurrentResolution().w;
        vertexUniforms->screenSizeInPixels[1] = getCurrentResolution().h;

        auto fragmentUniforms = drawLevelUniformCpuBuffer.getMemberPointer<DrawLevelUniforms::Fragment>();
        fragmentUniforms->atlasSizeInPixels[0] = mTexture->width();
        fragmentUniforms->atlasSizeInPixels[1] = mTexture->height();

        drawLevelUniformBuffer.setData(drawLevelUniformCpuBuffer.data(), drawLevelUniformCpuBuffer.getSizeInBytes());

        vertexArrayObject.getVertexBuffer(1)->setData(mInstanceData.data(), mInstanceData.size() * sizeof(Render::SpriteVertexPerInstance));

        drawLevelDescriptorSet.updateItems({
            {2, mTexture},
        });

        Render::Bindings bindings;
        bindings.vao = &vertexArrayObject;
        bindings.pipeline = &drawLevelPipeline;
        bindings.descriptorSet = &drawLevelDescriptorSet;
        bindings.nonDefaultFramebuffer = nonDefaultFramebuffer;

        Render::mainCommandQueue->cmdDrawInstances(0, 6, mInstanceData.size(), bindings);

        mInstanceData.clear();
        mTexture = nullptr;
    }

    constexpr auto tileHeight = 32;
    constexpr auto tileWidth = tileHeight * 2;
    constexpr auto staticObjectHeight = 256;

    // For a given isometric tile, tileScreenPosition will be the point marked
    // with an X in the diagram below, at the top left of the bounding box of
    // the tile.
    //
    // X-----.
    // |  .     .
    // .           .
    //    .     .
    //       .
    //
    void
    LevelRenderer::drawTilesetSprite(const Render::TextureReference* sprite, const Misc::Point& tileScreenPosition, std::optional<ByteColour> highlightColor)
    {
        // centering sprite at the center of tile by width and at the bottom of tile by height
        mDrawLevelCache.addSprite(sprite, tileScreenPosition.x - sprite->mWidth / 2, tileScreenPosition.y - sprite->mHeight + tileHeight, highlightColor);
    }

    // basic transform of isometric grid to normal, (0, 0) tile coordinate maps to (0, 0) pixel coordinates
    // since eventually we're gonna shift coordinates to viewport center, it's better to keep transform itself
    // as simple as possible
    template <typename T> static Vec2<T> tileTopPoint(Vec2<T> tile, int32_t scale = 1)
    {
        return Vec2<T>(T((tileWidth * scale) / 2) * (tile.x - tile.y), (tile.y + tile.x) * ((tileHeight * scale) / 2));
    }

    // this function simply does the reverse of the above function, could be found by solving linear equation system
    // it obviously uses the fact that tileWidth = tileHeight * 2
    Render::Tile getTileFromScreenCoords(Vec2i screenPos, Vec2i screenSpaceOffset, int32_t scale = 1)
    {
        Vec2i point = screenPos - screenSpaceOffset;
        auto x = std::div(2 * point.y + point.x, tileWidth * scale);
        auto y = std::div(2 * point.y - point.x, tileWidth * scale);
        return {x.quot, y.quot, x.rem > y.rem ? Render::TileHalf::right : Render::TileHalf::left};
    }

    void LevelRenderer::drawAtWorldPosition(const Render::TextureReference* sprite,
                                            const Vec2Fix& fractionalPos,
                                            const Misc::Point& toScreen,
                                            std::optional<ByteColour> highlightColor)
    {
        Vec2i point = Vec2i(tileTopPoint(fractionalPos));
        Vec2i res = point + toScreen;

        mDrawLevelCache.addSprite(sprite, res.x - sprite->mWidth / 2, res.y - sprite->mHeight + (tileHeight / 2), highlightColor);
    }

    constexpr auto bottomMenuSize = 0; // 144; // TODO: pass it as a variable
    static Misc::Point worldPositionToScreenSpace(const Vec2Fix& worldPosition, int32_t scale = 1)
    {
        // centering takes in accord bottom menu size to be consistent with original game centering
        Vec2i point = Vec2i(tileTopPoint(worldPosition, scale));

        Vec2i resolution = getCurrentResolution();
        return Misc::Point{resolution.w / 2, (resolution.h - bottomMenuSize) / 2} - point;
    }

    Render::Tile LevelRenderer::getTileByScreenPos(size_t x, size_t y, const Vec2Fix& worldPositionOffset)
    {
        Misc::Point screenSpaceOffset = worldPositionToScreenSpace(worldPositionOffset, mRenderScale);
        return getTileFromScreenCoords({static_cast<int32_t>(x), static_cast<int32_t>(y)}, screenSpaceOffset, mRenderScale);
    }

    template <typename ProcessTileFunc> void drawObjectsByTiles(const Misc::Point& toScreen, ProcessTileFunc processTile)
    {
        Misc::Point start{-2 * tileWidth, -2 * tileHeight};
        Render::Tile startingTile = getTileFromScreenCoords(start, toScreen);

        Vec2i resolution = getCurrentResolution();

        Misc::Point startingPoint = Vec2i(tileTopPoint(startingTile.pos)) + toScreen;
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
                                  LevelObjects& objs,
                                  LevelObjects& items,
                                  const Vec2Fix& fractionalPos,
                                  const DebugRenderData& debugData)
    {
        if (levelDrawFramebuffer->getColorBuffer().width() != getCurrentResolution().w ||
            levelDrawFramebuffer->getColorBuffer().height() != getCurrentResolution().h)
            createNewLevelDrawFramebuffer();

        Render::mainCommandQueue->cmdClearFramebuffer(Render::Colors::black, true, levelDrawFramebuffer.get());

        Vec2i toScreen = worldPositionToScreenSpace(fractionalPos);
        auto isInvalidTile = [&](const Render::Tile& tile) {
            return tile.pos.x < 0 || tile.pos.y < 0 || tile.pos.x >= static_cast<int32_t>(level.width()) || tile.pos.y >= static_cast<int32_t>(level.height());
        };

        // draw the ground diamonds
        drawObjectsByTiles(toScreen, [&](const Render::Tile& tile, const Misc::Point& topLeft) {
            if (isInvalidTile(tile))
            {
                drawTilesetSprite(minBottoms->getFrame(0), topLeft);
                return;
            }

            int32_t index = level.get(tile.pos).index();
            if (index >= 0 && index < minBottoms->size())
                drawTilesetSprite(minBottoms->getFrame(index), topLeft); // all static objects have the same sprite size
        });

        if (mDrawGrid)
        {
            mDrawLevelCache.end(*drawLevelUniformCpuBuffer,
                                *drawLevelUniformBuffer,
                                *vertexArrayObject,
                                *drawLevelDescriptorSet,
                                *drawLevelPipeline,
                                levelDrawFramebuffer.get());

            Render::mainCommandQueue->cmdClearFramebuffer(std::nullopt, true, levelDrawFramebuffer.get());

            Render::Color gridColor(1.0f, 1.0f, 1.0f, 0.8f);

            // Lines from northwest to southeast
            {
                Render::Tile startTile = getTileFromScreenCoords({-getCurrentResolution().w, -tileHeight}, toScreen);
                Misc::Point startingPoint = Vec2i(tileTopPoint(startTile.pos)) + toScreen;

                for (int32_t i = 0; i < getCurrentResolution().w / tileWidth * 2; i++)
                {
                    Vec2f top = Vec2f(startingPoint) + Vec2f(0.5, 0.5);

                    Vec2f oneTileOffset(tileWidth, tileHeight);
                    Vec2f bottom = top + oneTileOffset * (getCurrentResolution().h / tileHeight + 2);

                    int32_t realY = startTile.pos.y - i;
                    float thickness = std::abs(realY) % 2 == 0 ? 3.0f : 1.0f;
                    mDebugRenderer->drawLine(*Render::mainCommandQueue, levelDrawFramebuffer.get(), gridColor, top, bottom, thickness);

                    startingPoint.x += tileWidth;
                }
            }

            // Lines from southwest to northeast
            {
                Render::Tile startTile = getTileFromScreenCoords({-tileWidth, 0}, toScreen);
                Misc::Point startingPoint = Vec2i(tileTopPoint(startTile.pos)) + toScreen;

                for (int32_t i = 0; i < getCurrentResolution().h / tileHeight * 2; i++)
                {
                    Vec2f bottom = Vec2f(startingPoint) + Vec2f(0.5, 0.5);

                    Vec2f oneTileOffset(tileWidth, -tileHeight);
                    Vec2f top = bottom + oneTileOffset * (getCurrentResolution().h / tileHeight + 2);

                    int32_t realX = startTile.pos.x + i;
                    float thickness = std::abs(realX) % 2 == 0 ? 3.0f : 1.0f;
                    mDebugRenderer->drawLine(*Render::mainCommandQueue, levelDrawFramebuffer.get(), gridColor, top, bottom, thickness);

                    startingPoint.y += tileHeight;
                }
            }
        }

        // draw above ground objects (walls, players, town buildings etc)
        drawObjectsByTiles(toScreen, [&](const Render::Tile& tile, const Misc::Point& topLeft) {
            if (isInvalidTile(tile))
                return;

            int32_t index = level.get(tile.pos).index();
            if (index >= 0 && index < minTops->size())
            {
                drawTilesetSprite(minTops->getFrame(index), topLeft);

                // Add special sprites (arches / open door frames) if required.
                if (specialSpritesMap.count(index))
                {
                    int32_t specialSpriteIndex = specialSpritesMap.at(index);
                    const Render::TextureReference* sprite = specialSprites->getFrame(specialSpriteIndex);
                    drawTilesetSprite(sprite, topLeft);
                }
            }

            const std::vector<LevelObject>& itemsForTile = items.get(tile.pos.x, tile.pos.y);
            for (const auto& item : itemsForTile)
            {
                const Render::TextureReference* sprite = item.sprite->getFrame(item.spriteFrame);
                Vec2Fix position = Vec2Fix(tile.pos) + Vec2Fix(FixedPoint("0.5"), FixedPoint("0.5"));
                drawAtWorldPosition(sprite, position, toScreen, item.hoverColor);
            }

            const std::vector<LevelObject>& objsForTile = objs.get(tile.pos.x, tile.pos.y);
            for (auto& obj : objsForTile)
            {
                if (obj.valid)
                {
                    const Render::TextureReference* sprite = obj.sprite->getFrame(obj.spriteFrame);
                    drawAtWorldPosition(sprite, obj.fractionalPos, toScreen, obj.hoverColor);
                }
            }
        });

        mDrawLevelCache.end(
            *drawLevelUniformCpuBuffer, *drawLevelUniformBuffer, *vertexArrayObject, *drawLevelDescriptorSet, *drawLevelPipeline, levelDrawFramebuffer.get());

        for (const auto& item : debugData)
        {
            if (std::holds_alternative<RectData>(item))
            {
                const RectData& rectData = std::get<RectData>(item);
                Misc::Point rectPoint = Vec2i(tileTopPoint(rectData.worldPosition)) + toScreen;
                mDebugRenderer->drawRectangle(*Render::mainCommandQueue,
                                              levelDrawFramebuffer.get(),
                                              rectData.color,
                                              rectPoint.x,
                                              rectPoint.y,
                                              int32_t(rectData.w * tileWidth),
                                              int32_t(rectData.h * tileHeight));
            }
            if (std::holds_alternative<TileData>(item))
            {
                const TileData& tileData = std::get<TileData>(item);

                Vec2f quad[4];
                quad[0] = tileTopPoint(Vec2f(tileData.worldPosition)) + Vec2f(toScreen);
                quad[1] = quad[0] + Vec2f(tileWidth / 2, tileHeight / 2);
                quad[2] = quad[0] + Vec2f(0, tileHeight);
                quad[3] = quad[0] + Vec2f(-tileWidth / 2, tileHeight / 2);

                mDebugRenderer->drawQuad(*Render::mainCommandQueue, levelDrawFramebuffer.get(), tileData.color, quad);
            }
            if (std::holds_alternative<PointData>(item))
            {
                const PointData& pointData = std::get<PointData>(item);
                Misc::Point point = Vec2i(tileTopPoint(pointData.worldPosition)) + toScreen;

                // TODO: this should proooobably be a circle, not a rect but it's fine for now
                mDebugRenderer->drawRectangle(*Render::mainCommandQueue,
                                              levelDrawFramebuffer.get(),
                                              pointData.color,
                                              point.x - pointData.radiusInPixels,
                                              point.y - pointData.radiusInPixels,
                                              pointData.radiusInPixels * 2,
                                              pointData.radiusInPixels * 2);
            }
        }

        Render::Bindings fullscreenBindings;
        fullscreenBindings.pipeline = fullscreenPipeline.get();
        fullscreenBindings.vao = fullscreenVao.get();
        fullscreenBindings.descriptorSet = fullscreenDescriptorSet.get();

        FullscreenUniforms::Vertex fullscreenSettings = {};
        fullscreenSettings.scaleOrigin[0] = 0;
        fullscreenSettings.scaleOrigin[1] = 0;
        fullscreenSettings.scale = mRenderScale;
        fullscreenUniformBuffer->setData(&fullscreenSettings, sizeof(FullscreenUniforms::Vertex));

        Render::Texture& levelTexture = levelDrawFramebuffer->getColorBuffer();
        if (mTextureFilter && levelTexture.getInfo().magFilter == Render::Filter::Nearest)
            levelTexture.setFilter(levelTexture.getInfo().minFilter, Render::Filter::Linear);
        else if (!mTextureFilter && levelTexture.getInfo().magFilter == Render::Filter::Linear)
            levelTexture.setFilter(levelTexture.getInfo().minFilter, Render::Filter::Nearest);

        Render::mainCommandQueue->cmdDraw(0, 6, fullscreenBindings);
    }
    LevelRenderer::LevelRenderer()
    {
        mDebugRenderer = std::make_unique<Render::DebugRenderer>(*Render::mainRenderInstance);

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

        drawLevelPipeline = Render::mainRenderInstance->createPipeline(drawLevelPipelineSpec);
        vertexArrayObject = Render::mainRenderInstance->createVertexArrayObject({0, 0}, drawLevelPipelineSpec.vertexLayouts, 0);

        drawLevelUniformCpuBuffer = std::make_unique<DrawLevelUniforms::CpuBufferType>(Render::mainRenderInstance->capabilities().uniformBufferOffsetAlignment);

        drawLevelUniformBuffer = Render::mainRenderInstance->createBuffer(drawLevelUniformCpuBuffer->getSizeInBytes());
        drawLevelDescriptorSet = Render::mainRenderInstance->createDescriptorSet(drawLevelPipelineSpec.descriptorSetSpec);

        // clang-format off
        drawLevelDescriptorSet->updateItems(
        {
            {0, Render::BufferSlice{drawLevelUniformBuffer.get(), drawLevelUniformCpuBuffer->getMemberOffset<DrawLevelUniforms::Vertex>(), sizeof(DrawLevelUniforms::Vertex)}},
            {1, Render::BufferSlice{drawLevelUniformBuffer.get(), drawLevelUniformCpuBuffer->getMemberOffset<DrawLevelUniforms::Fragment>(), sizeof(DrawLevelUniforms::Fragment)}},
        });

        Render::SpriteVertexMain baseVertices[] =
        {
            {{0, 0},  {0, 0}},
            {{1, 0},  {1, 0}},
            {{1, 1},  {1, 1}},
            {{0, 0},  {0, 0}},
            {{1, 1},  {1, 1}},
            {{0, 1},  {0, 1}}
        };
        // clang-format on
        vertexArrayObject->getVertexBuffer(0)->setData(baseVertices, sizeof(baseVertices));

        {
            // clang-format off
            Render::BasicVertex topLeft  {{-1, -1}, {0, 0}};
            Render::BasicVertex topRight {{ 1, -1}, {1, 0}};
            Render::BasicVertex botLeft  {{-1,  1}, {0, 1}};
            Render::BasicVertex botRight {{ 1,  1}, {1, 1}};

            Render::BasicVertex fullscreenVertices[] =
            {
                topLeft, topRight, botLeft,
                topRight, botRight, botLeft,
            };
            // clang-format on

            Render::PipelineSpec fullscreenPipelineSpec;
            fullscreenPipelineSpec.vertexLayouts = {Render::BasicVertex::layout()};
            fullscreenPipelineSpec.vertexShaderPath = Misc::getResourcesPath().str() + "/shaders/fullscreen.vert";
            fullscreenPipelineSpec.fragmentShaderPath = Misc::getResourcesPath().str() + "/shaders/fullscreen.frag";
            fullscreenPipelineSpec.descriptorSetSpec = {{
                {Render::DescriptorType::UniformBuffer, "vertexUniforms"},
                {Render::DescriptorType::Texture, "tex"},
            }};

            fullscreenPipeline = Render::mainRenderInstance->createPipeline(fullscreenPipelineSpec);
            fullscreenVao = Render::mainRenderInstance->createVertexArrayObject({sizeof(fullscreenVertices)}, fullscreenPipelineSpec.vertexLayouts, 0);
            fullscreenUniformBuffer = Render::mainRenderInstance->createBuffer(sizeof(FullscreenUniforms::Vertex));
            fullscreenDescriptorSet = Render::mainRenderInstance->createDescriptorSet(fullscreenPipelineSpec.descriptorSetSpec);

            fullscreenVao->getVertexBuffer(0)->setData(fullscreenVertices, sizeof(baseVertices));
            fullscreenDescriptorSet->updateItems({{0, this->fullscreenUniformBuffer.get()}});
        }

        createNewLevelDrawFramebuffer();
    }

    void LevelRenderer::createNewLevelDrawFramebuffer()
    {
        Render::BaseTextureInfo textureInfo;
        textureInfo.width = getCurrentResolution().w;
        textureInfo.height = getCurrentResolution().h;
        textureInfo.minFilter = Render::Filter::Linear;
        textureInfo.magFilter = Render::Filter::Nearest;

        if (levelDrawFramebuffer)
            textureInfo.magFilter = levelDrawFramebuffer->getColorBuffer().getInfo().magFilter;

        textureInfo.format = Render::Format::RGBA8UNorm;
        std::unique_ptr<Render::Texture> colorBuffer = Render::mainRenderInstance->createTexture(textureInfo);

        textureInfo.format = Render::Format::Depth24Stencil8;
        std::unique_ptr<Render::Texture> depthStencilBuffer = Render::mainRenderInstance->createTexture(textureInfo);

        Render::FramebufferInfo framebufferInfo;
        framebufferInfo.colorBuffer = colorBuffer.release();
        framebufferInfo.depthStencilBuffer = depthStencilBuffer.release();
        levelDrawFramebuffer = Render::mainRenderInstance->createFramebuffer(framebufferInfo);

        fullscreenDescriptorSet->updateItems({{1, &levelDrawFramebuffer->getColorBuffer()}});
    }

    LevelRenderer::~LevelRenderer() = default;
}
