#include <render/framebuffer.h>
#include <render/texture.h>

namespace Render
{
    Framebuffer::Framebuffer(const FramebufferInfo& info) : mInfo(info)
    {
        release_assert(mInfo.colorBuffer);

        if (mInfo.depthStencilBuffer)
        {
            release_assert(mInfo.depthStencilBuffer->getInfo().format == Format::Depth24Stencil8);
            release_assert(mInfo.colorBuffer->width() == mInfo.depthStencilBuffer->width());
            release_assert(mInfo.colorBuffer->height() == mInfo.depthStencilBuffer->height());
        }
    }

    Framebuffer::~Framebuffer()
    {
        if (mInfo.ownsColorBuffer == FramebufferInfo::OwnsBufferType::Owns)
            delete mInfo.colorBuffer;
        if (mInfo.ownsDepthBuffer == FramebufferInfo::OwnsBufferType::Owns)
            delete mInfo.depthStencilBuffer;
    }

}