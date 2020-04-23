#pragma once
#include <SDL.h>
#include <render/renderinstance.h>

namespace Render
{
    class RenderInstanceOpenGL final : public RenderInstance
    {
        using super = RenderInstance;

    public:
        explicit RenderInstanceOpenGL(SDL_Window& window);
        ~RenderInstanceOpenGL() override;

        std::unique_ptr<DescriptorSet> createDescriptorSet(DescriptorSetSpec spec) override;
        std::unique_ptr<Pipeline> createPipeline(const PipelineSpec& spec) override;
        std::unique_ptr<Texture> createTexture(const BaseTextureInfo& info) override;
        std::unique_ptr<Framebuffer> createFramebuffer(const FramebufferInfo& info) override;
        std::unique_ptr<Buffer> createBuffer(size_t sizeInBytes) override;
        std::unique_ptr<VertexArrayObject> createVertexArrayObject(std::vector<size_t> bufferSizeCounts,
                                                                   std::vector<NonNullConstPtr<VertexLayout>> bindings,
                                                                   size_t indexBufferSizeInElements) override;
        std::unique_ptr<CommandQueue> createCommandQueue() override;

        void onWindowResized(int32_t width, int32_t height) override;

    private:
        static void setupGlobalState();

    private:
        SDL_GLContext mGlContext = nullptr;
    };
}
