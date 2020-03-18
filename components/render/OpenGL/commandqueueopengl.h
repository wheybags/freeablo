#pragma once
#include "vertexarrayobjectopengl.h"
#include <render/commandqueue.h>
#include <variant>

namespace Render
{
    class CommandQueueOpenGL final : public CommandQueue
    {
        using super = CommandQueue;

    public:
        explicit CommandQueueOpenGL(RenderInstanceOpenGL& instance);

        void cmdDraw(size_t firstVertex, size_t vertexCount, Bindings& bindings) override;
        void cmdDrawIndexed(size_t firstIndex, size_t vertexCount, Bindings& bindings) override;
        void cmdDrawInstances(size_t firstVertex, size_t vertexCount, size_t instanceCount, Bindings& bindings) override;

        void cmdPresent() override;

    public:
        RenderInstanceOpenGL& getInstance() { return static_cast<RenderInstanceOpenGL&>(mInstance); }

    private:
        VertexArrayObject* mBoundVao = nullptr;
    };
}