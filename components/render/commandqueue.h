#pragma once
#include "color.h"
#include <cstddef>
#include <misc/assert.h>

namespace Render
{
    class Texture;
    class VertexArrayObject;
    class Pipeline;
    class RenderInstance;

    struct Bindings
    {
        Pipeline* pipeline = nullptr;
        VertexArrayObject* vao = nullptr;

        void assertBindingsCorrect();
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

        virtual void cmdPresent() = 0;

    protected:
        RenderInstance& mInstance;
        bool mBegun = false;
        bool mSubmitted = true;
    };
}