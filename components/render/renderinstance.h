#pragma once
#include <cstdint>
#include <memory>
#include <misc/misc.h>
#include <render/vertexlayout.h>

struct SDL_Window;

namespace Render
{
    class DescriptorSet;
    class DescriptorSetSpec;
    class Pipeline;
    struct PipelineSpec;
    class Buffer;
    class VertexArrayObject;
    class CommandQueue;
    class Texture;
    struct BaseTextureInfo;
    class Framebuffer;
    struct FramebufferInfo;

    struct RenderCapabilities
    {
        int32_t maxTextureSize;
        int32_t uniformBufferOffsetAlignment;
    };

    class RenderInstance
    {
    public:
        RenderInstance(RenderInstance&) = delete;
        explicit RenderInstance(SDL_Window& window) : mWindow(window) {}
        virtual ~RenderInstance() = default;

        virtual std::unique_ptr<DescriptorSet> createDescriptorSet(DescriptorSetSpec spec) = 0;
        virtual std::unique_ptr<Pipeline> createPipeline(const PipelineSpec& spec) = 0;
        virtual std::unique_ptr<Texture> createTexture(const BaseTextureInfo& info) = 0;
        virtual std::unique_ptr<Framebuffer> createFramebuffer(const FramebufferInfo& info) = 0;
        virtual std::unique_ptr<Buffer> createBuffer(size_t sizeInBytes) = 0;
        virtual std::unique_ptr<VertexArrayObject> createVertexArrayObject(std::vector<size_t> bufferSizeCounts,
                                                                           std::vector<NonNullConstPtr<VertexLayout>> bindings,
                                                                           size_t indexBufferSizeInElements) = 0;
        virtual std::unique_ptr<CommandQueue> createCommandQueue() = 0;

        const RenderCapabilities& capabilities() const { return mRenderCapabilities; }

        virtual void onWindowResized(int32_t width, int32_t height) = 0;

        enum class Type
        {
            OpenGL
        };
        static RenderInstance* createRenderInstance(Type type, SDL_Window& window);

    public:
        SDL_Window& mWindow;

    protected:
        RenderCapabilities mRenderCapabilities = {};
    };
}
