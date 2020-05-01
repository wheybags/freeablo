#pragma once
#include <cstdint>
#include <memory>

namespace Render
{
    class RenderInstance;
    class CommandQueue;
    class Pipeline;
    class VertexArrayObject;
    class Color;
    class Framebuffer;

    class DebugRenderer
    {
    public:
        DebugRenderer(RenderInstance& renderInstance);
        ~DebugRenderer();

        void drawRectangle(CommandQueue& commandQueue, Framebuffer* nonDefaultFramebuffer, const Color& color, int32_t x, int32_t y, int32_t w, int32_t h);

    private:
        std::unique_ptr<Pipeline> mPipeline;
        std::unique_ptr<VertexArrayObject> mVao;

        RenderInstance& mInstance;
    };
}
