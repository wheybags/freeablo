#pragma once
#include <Image/image.h>
#include <cstdint>
#include <map>
#include <misc/simplevec2.h>
#include <render/alignedcpubuffer.h>
#include <render/atlastexture.h>
#include <render/levelobjects.h>
#include <render/vertextypes.h>

namespace Level
{
    class Level;
}

namespace Render
{
    class SpriteGroup;
    class AtlasTextureEntry;
    class Texture;
    class VertexArrayObject;
    class Pipeline;
    class Buffer;
    class DescriptorSet;
    class Tile;

    typedef const AtlasTextureEntry* Sprite;
}

namespace FARender
{
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

        using CpuBufferType = Render::TypedAlignedCpuBuffer<Vertex, Fragment>;
    }

    class DrawLevelCache
    {
    public:
        void addSprite(Render::Sprite atlasEntry, int32_t x, int32_t y, std::optional<ByteColour> highlightColor);
        void end(DrawLevelUniforms::CpuBufferType* drawLevelUniformCpuBuffer,
                 Render::Buffer* drawLevelUniformBuffer,
                 Render::VertexArrayObject* vertexArrayObject,
                 Render::DescriptorSet* drawLevelDescriptorSet,
                 Render::Pipeline* drawLevelPipeline);

    private:
        void batchDrawSprite(const Render::AtlasTextureEntry& atlasEntry,
                             int32_t x,
                             int32_t y,
                             std::optional<ByteColour> highlightColor,
                             float zBufferVal,
                             DrawLevelUniforms::CpuBufferType* drawLevelUniformCpuBuffer,
                             Render::Buffer* drawLevelUniformBuffer,
                             Render::VertexArrayObject* vertexArrayObject,
                             Render::DescriptorSet* drawLevelDescriptorSet,
                             Render::Pipeline* drawLevelPipeline);
        void draw(DrawLevelUniforms::CpuBufferType* drawLevelUniformCpuBuffer,
                  Render::Buffer* drawLevelUniformBuffer,
                  Render::VertexArrayObject* vertexArrayObject,
                  Render::DescriptorSet* drawLevelDescriptorSet,
                  Render::Pipeline* drawLevelPipeline);

    private:
        struct SpriteData
        {
            const Render::AtlasTextureEntry* atlasEntry = nullptr;
            int32_t x = 0;
            int32_t y = 0;
            std::optional<ByteColour> highlightColor;
            float zBufferValue = 0;
        };

        std::vector<SpriteData> mSpritesToDraw;
        std::vector<Render::SpriteVertexPerInstance> mInstanceData;
        Render::Texture* mTexture = nullptr;
    };

    class LevelRenderer
    {
    public:
        LevelRenderer();

        void drawLevel(const Level::Level& level,
                       Render::SpriteGroup* minTops,
                       Render::SpriteGroup* minBottoms,
                       Render::SpriteGroup* specialSprites,
                       const std::map<int32_t, int32_t>& specialSpritesMap,
                       Render::LevelObjects& objs,
                       Render::LevelObjects& items,
                       const Vec2Fix& fractionalPos);

        static Render::Tile getTileByScreenPos(size_t x, size_t y, const Vec2Fix& fractionalPos);

    private:
        void drawAtTile(Render::Sprite sprite, const Misc::Point& tileTop, int spriteW, int spriteH, std::optional<ByteColour> highlightColor = std::nullopt);
        void drawMovingSprite(Render::Sprite sprite,
                              const Vec2Fix& fractionalPos,
                              const Misc::Point& toScreen,
                              std::optional<Cel::Colour> highlightColor = std::nullopt);

        DrawLevelCache mDrawLevelCache;

        Render::VertexArrayObject* vertexArrayObject = nullptr;
        Render::Pipeline* drawLevelPipeline = nullptr;
        Render::Buffer* drawLevelUniformBuffer = nullptr;
        DrawLevelUniforms::CpuBufferType* drawLevelUniformCpuBuffer = nullptr;
        Render::DescriptorSet* drawLevelDescriptorSet = nullptr;
    };
}
