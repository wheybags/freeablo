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

        void cmdClearTexture(Texture& texture, const Color& color) override;
        void cmdDraw(size_t firstVertex, size_t vertexCount, Bindings& bindings) override;
        void cmdDrawIndexed(size_t firstIndex, size_t vertexCount, Bindings& bindings) override;
        void cmdDrawInstances(size_t firstVertex, size_t vertexCount, size_t instanceCount, Bindings& bindings) override;
        void cmdClearFramebuffer(std::optional<Color> color, bool clearDepth, Framebuffer* nonDefaultFramebuffer = nullptr) override;

        void cmdPresent() override;

    public:
        RenderInstanceOpenGL& getInstance() { return safe_downcast<RenderInstanceOpenGL&>(mInstance); }

    private:
        // Binds the whole state necessary for a draw in its constructor, and unbinds it all in its (auto-generated) destructor.
        class DrawScopedBinderGL
        {
        public:
            DrawScopedBinderGL() = delete;
            DrawScopedBinderGL(DrawScopedBinderGL&) = delete;

            explicit DrawScopedBinderGL(Bindings bindings);

        private:
            std::vector<std::unique_ptr<BindableGL>> mTempBindables;
            std::vector<ScopedBindGL> mBinders;
        };

        std::unique_ptr<DrawScopedBinderGL> setupState(Bindings& bindings);
    };
}