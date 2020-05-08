#pragma once
#include "debugrenderdata.h"
#include <Image/image.h>
#include <atomic>
#include <cstdint>
#include <map>
#include <misc/simplevec2.h>
#include <render/alignedcpubuffer.h>
#include <render/atlastexture.h>
#include <render/vertextypes.h>

namespace Level
{
    class Level;
}

namespace Render
{
    class TextureReference;
    class Texture;
    class VertexArrayObject;
    class Pipeline;
    class Buffer;
    class DescriptorSet;
    class Framebuffer;
    struct Tile;
    class SpriteGroup;
    class DebugRenderer;
}

namespace FARender
{
    namespace DrawLevelUniforms
    {
        struct Vertex
        {
            float screenSizeInPixels[2];

            float pad1[2];
        };

        struct Fragment
        {
            float atlasSizeInPixels[2];

            float pad2[2];
        };

        using CpuBufferType = Render::TypedAlignedCpuBuffer<Vertex, Fragment>;
    }

    namespace FullscreenUniforms
    {
        struct Vertex
        {
            float scaleOrigin[2];
            float scale;

            float pad1;
        };
    }

    class DrawLevelCache
    {
    public:
        void addSprite(const Render::TextureReference* atlasEntry, int32_t x, int32_t y, std::optional<ByteColour> highlightColor);
        void end(DrawLevelUniforms::CpuBufferType& drawLevelUniformCpuBuffer,
                 Render::Buffer& drawLevelUniformBuffer,
                 Render::VertexArrayObject& vertexArrayObject,
                 Render::DescriptorSet& drawLevelDescriptorSet,
                 Render::Pipeline& drawLevelPipeline,
                 Render::Framebuffer* nonDefaultFramebuffer);

    private:
        void batchDrawSprite(const Render::TextureReference& atlasEntry,
                             int32_t x,
                             int32_t y,
                             std::optional<ByteColour> highlightColor,
                             float zBufferVal,
                             DrawLevelUniforms::CpuBufferType& drawLevelUniformCpuBuffer,
                             Render::Buffer& drawLevelUniformBuffer,
                             Render::VertexArrayObject& vertexArrayObject,
                             Render::DescriptorSet& drawLevelDescriptorSet,
                             Render::Pipeline& drawLevelPipeline,
                             Render::Framebuffer* nonDefaultFramebuffer);
        void draw(DrawLevelUniforms::CpuBufferType& drawLevelUniformCpuBuffer,
                  Render::Buffer& drawLevelUniformBuffer,
                  Render::VertexArrayObject& vertexArrayObject,
                  Render::DescriptorSet& drawLevelDescriptorSet,
                  Render::Pipeline& drawLevelPipeline,
                  Render::Framebuffer* nonDefaultFramebuffer);

    private:
        struct SpriteData
        {
            const Render::TextureReference* atlasEntry = nullptr;
            int32_t x = 0;
            int32_t y = 0;
            std::optional<ByteColour> highlightColor;
            float zBufferValue = 0;
        };

        std::vector<SpriteData> mSpritesToDraw;
        std::vector<Render::SpriteVertexPerInstance> mInstanceData;
        Render::Texture* mTexture = nullptr;
    };

    struct LevelObject
    {
        bool valid = false;
        Render::SpriteGroup* sprite = nullptr;
        int32_t spriteFrame = 0;
        Vec2Fix fractionalPos;
        std::optional<ByteColour> hoverColor;
    };

    typedef Misc::Array2D<std::vector<LevelObject>> LevelObjects; // TODO: get a custom small vector class + use it here

    class LevelRenderer
    {
    public:
        LevelRenderer();
        ~LevelRenderer();

        void drawLevel(const Level::Level& level,
                       Render::SpriteGroup* minTops,
                       Render::SpriteGroup* minBottoms,
                       Render::SpriteGroup* specialSprites,
                       const std::map<int32_t, int32_t>& specialSpritesMap,
                       LevelObjects& objs,
                       LevelObjects& items,
                       const Vec2Fix& fractionalPos,
                       const DebugRenderData& debugData);

        Render::Tile getTileByScreenPos(size_t x, size_t y, const Vec2Fix& worldPositionOffset);

        void toggleTextureFiltering() { mTextureFilter = !mTextureFilter; }
        void toggleGrid() { mDrawGrid = !mDrawGrid; }
        void adjustZoom(int32_t delta) { mRenderScale = std::clamp(mRenderScale + delta, 1, 5); }

    private:
        void createNewLevelDrawFramebuffer();

        void drawTilesetSprite(const Render::TextureReference* sprite,
                               const Misc::Point& tileScreenPosition,
                               std::optional<ByteColour> highlightColor = std::nullopt);
        void drawAtWorldPosition(const Render::TextureReference* sprite,
                                 const Vec2Fix& fractionalPos,
                                 const Misc::Point& toScreen,
                                 std::optional<ByteColour> highlightColor = std::nullopt);

    private:
        DrawLevelCache mDrawLevelCache;

        std::unique_ptr<Render::VertexArrayObject> vertexArrayObject;
        std::unique_ptr<Render::Pipeline> drawLevelPipeline;
        std::unique_ptr<Render::Buffer> drawLevelUniformBuffer;
        std::unique_ptr<DrawLevelUniforms::CpuBufferType> drawLevelUniformCpuBuffer;
        std::unique_ptr<Render::DescriptorSet> drawLevelDescriptorSet;

        std::unique_ptr<Render::Framebuffer> levelDrawFramebuffer;

        std::unique_ptr<Render::Pipeline> fullscreenPipeline;
        std::unique_ptr<Render::VertexArrayObject> fullscreenVao;
        std::unique_ptr<Render::Buffer> fullscreenUniformBuffer;
        std::unique_ptr<Render::DescriptorSet> fullscreenDescriptorSet;

        std::atomic_bool mTextureFilter = false;
        std::atomic_int mRenderScale = 2;
        std::atomic_bool mDrawGrid = false;
        std::unique_ptr<Render::DebugRenderer> mDebugRenderer;
    };
}
