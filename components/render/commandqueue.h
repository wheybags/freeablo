#pragma once
#include "color.h"
#include <cstddef>
#include <cstdint>
#include <misc/assert.h>
#include <optional>

namespace Render
{
    class Texture;
    class VertexArrayObject;
    class Pipeline;
    class RenderInstance;
    class DescriptorSet;
    class Framebuffer;

    struct Bindings
    {
        Pipeline* pipeline = nullptr;
        VertexArrayObject* vao = nullptr;
        DescriptorSet* descriptorSet = nullptr;
        Framebuffer* nonDefaultFramebuffer = nullptr;

        void assertBindingsCorrect();
    };

    struct ScissorRect
    {
        int32_t x;
        int32_t y;
        int32_t w;
        int32_t h;
    };

    class CommandQueue
    {
    public:
        CommandQueue(CommandQueue&) = delete;
        explicit CommandQueue(RenderInstance& instance) : mInstance(instance) {}

        virtual ~CommandQueue() = default;

        virtual void begin();
        virtual void end();
        virtual void submit();

        virtual void cmdClearTexture(Texture& texture, const Color& clearColor) = 0;
        virtual void cmdDraw(size_t firstVertex, size_t vertexCount, Bindings& bindings);
        virtual void cmdDrawIndexed(size_t firstIndex, size_t vertexCount, Bindings& bindings);
        virtual void cmdDrawInstances(size_t firstVertex, size_t vertexCount, size_t instanceCount, Bindings& bindings);
        void cmdScissor(ScissorRect scissorRect) { mScissor = scissorRect; }
        virtual void cmdClearFramebuffer(std::optional<Color> color, bool clearDepth, Framebuffer* nonDefaultFramebuffer = nullptr) = 0;

        virtual void cmdPresent() = 0;

    protected:
        RenderInstance& mInstance;
        bool mBegun = false;
        bool mSubmitted = true;
        ScissorRect mScissor = {};
    };
}