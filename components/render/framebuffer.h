#pragma once
#include <misc/assert.h>

namespace Render
{
    class Texture;

    struct FramebufferInfo
    {
        enum class OwnsBufferType
        {
            Owns,
            External,
        };

        Texture* colorBuffer = nullptr;
        OwnsBufferType ownsColorBuffer = OwnsBufferType::Owns;
        Texture* depthStencilBuffer = nullptr;
        OwnsBufferType ownsDepthBuffer = OwnsBufferType::Owns;
    };

    class Framebuffer
    {
    public:
        Framebuffer(const FramebufferInfo& info);
        virtual ~Framebuffer();

        Texture& getColorBuffer() { return *mInfo.colorBuffer; }
        Texture* getDepthBuffer() { return mInfo.depthStencilBuffer; }

    protected:
        FramebufferInfo mInfo;
    };
}