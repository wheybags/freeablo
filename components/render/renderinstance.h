#pragma once
#include <cstdint>
#include <memory>
#include <misc/misc.h>
#include <render/vertexlayout.h>

struct SDL_Window;

namespace Render
{
    //    enum class Topology
    //    {
    //        TriangleList,
    //    };

    //    class DescriptorSet
    //    {
    //    public:
    //        virtual ~DescriptorSet() {}
    //    };

    //    struct PipelineSpec
    //    {
    //        Topology topology = Topology::TriangleList;
    //        const VertexLayout* vertexLayout = nullptr;
    //        filesystem::path vertexShaderPath;
    //        filesystem::path fragmentShaderPath;
    //        // target framebuffer format
    //    };

    //    class Pipeline
    //    {
    //    public:
    //        Pipeline(RenderInstance& instance, const PipelineSpec& spec) : mInstance(instance), mSpec(spec) {}
    //        virtual ~Pipeline() {}

    //    protected:
    //        RenderInstance& mInstance;
    //        const PipelineSpec& mSpec;
    //    };

    class Buffer;
    class VertexArrayObject;
    class CommandQueue;
    class Texture;
    struct BaseTextureInfo;

    class RenderInstance
    {
    public:
        RenderInstance(RenderInstance&) = delete;
        explicit RenderInstance(SDL_Window& window) : mWindow(window) {}
        virtual ~RenderInstance() = default;

        virtual std::unique_ptr<Texture> createTexture(const BaseTextureInfo& info) = 0;
        virtual std::unique_ptr<Buffer> createBuffer(size_t sizeInBytes) = 0;
        virtual std::unique_ptr<VertexArrayObject> createVertexArrayObject(std::vector<size_t> bufferSizeCounts,
                                                                           std::vector<NonNullConstPtr<VertexLayout>> bindings,
                                                                           size_t indexBufferSizeInElements) = 0;
        virtual std::unique_ptr<CommandQueue> createCommandQueue() = 0;

        enum class Type
        {
            OpenGL
        };
        static RenderInstance* createRenderInstance(Type type, SDL_Window& window);

    public:
        SDL_Window& mWindow;
    };
}
