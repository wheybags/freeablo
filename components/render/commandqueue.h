#pragma once
#include "color.h"
#include <render/pipeline.h>
#include <render/vertexarrayobject.h>

namespace Render
{
    class Texture;

    struct Bindings
    {
        VertexArrayObject* vao = nullptr;

        void assertFilled() { debug_assert(vao); }
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