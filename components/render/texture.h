#pragma once
#include <cstdint>
#include <render/vertexlayout.h>

namespace Render
{
    enum class Filter
    {
        Nearest,
        Linear,
    };

    struct BaseTextureInfo
    {
        int32_t width = 0;
        int32_t height = 0;
        int32_t arrayLayers = 1;
        bool forceTextureToBeATextureArray = false;

        Format format = Format::RGBA8UNorm;

        Filter minFilter = Filter::Linear;
        Filter magFilter = Filter::Linear;
    };

    class RenderInstance;

    class Texture
    {
    public:
        Texture(Texture&) = delete;
        Texture(RenderInstance& instance, const BaseTextureInfo& info) : mInstance(instance), mInfo(info) {}
        virtual ~Texture() = default;

        const BaseTextureInfo& getInfo() const { return mInfo; }
        int32_t width() const { return mInfo.width; }
        int32_t height() const { return mInfo.height; }

        virtual void
        updateImageData(int32_t x, int32_t y, int32_t layer, int32_t width, int32_t height, const uint8_t* rgba8UnormData, int32_t pitchInPixels) = 0;
        virtual void readImageData(uint8_t* rgba8UnormDestination) = 0;

        virtual void setFilter(Filter minFilter, Filter magFilter)
        {
            mInfo.minFilter = minFilter;
            mInfo.magFilter = magFilter;
        }

    protected:
        RenderInstance& mInstance;
        BaseTextureInfo mInfo;
    };
}