#pragma once
#include <cstdint>
#include <filesystem/path.h>
#include <memory>
#include <misc/misc.h>
#include <render/vertexlayout.h>

namespace Render
{
    //    enum class Topology
    //    {
    //        TriangleList,
    //    };

    //    struct BaseTextureInfo
    //    {
    //        int32_t width = 0;
    //        int32_t height = 0;
    //        Format format = Format::RGBA8U;
    //    };

    //    class RenderInstance;

    //    class Texture
    //    {
    //    public:
    //        Texture(RenderInstance& instance, const BaseTextureInfo& info) : mInstance(instance), mInfo(info) {}
    //        virtual ~Texture(){};

    //        const BaseTextureInfo& info() const { return mInfo; }
    //        int32_t width() const { return mInfo.width; }
    //        int32_t height() const { return mInfo.height; }

    //        virtual void* map() = 0;
    //        virtual void unmap() = 0;

    //    protected:
    //        RenderInstance& mInstance;
    //        BaseTextureInfo mInfo;
    //    };

    class Buffer
    {
    public:
        Buffer(size_t sizeInBytes) : mSizeInBytes(sizeInBytes) {}

        virtual ~Buffer() = default;

        size_t getSizeInBytes() { return mSizeInBytes; }

        virtual void setData(void* data, size_t dataSizeInBytes) = 0;

    protected:
        size_t mSizeInBytes = 0;
    };

    class VertexArrayObject
    {
    public:
        VertexArrayObject(std::vector<NonNullConstPtr<VertexLayout>> bindings) : mBindings(bindings) {}

        virtual ~VertexArrayObject() = default;

        size_t getVertexBufferCount() { return mBindings.size(); }
        virtual Buffer* getVertexBuffer(size_t index) = 0;
        virtual Buffer* getIndexBuffer() = 0;

    public:
        std::vector<NonNullConstPtr<VertexLayout>> mBindings;
    };

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

    //    class CommandQueue
    //    {
    //    public:
    //        virtual ~CommandQueue() {}

    //        virtual void begin() = 0;
    //        virtual void end() = 0;

    //        virtual void cmdBindPipeline(Pipeline* pipeline) = 0;
    //        virtual void cmdBindVertexBuffer(VertexBuffer* buffer) = 0;
    //        virtual void cmdBindDescriptorSet(DescriptorSet* descriptorSet) = 0;

    //        virtual void cmdDraw(size_t firstVertex, size_t vertexCount) = 0;

    //        virtual void cmdPresent() = 0;
    //    };

    //    class RenderInstance
    //    {
    //    public:
    //        virtual ~RenderInstance() {}

    //        virtual std::unique_ptr<Texture> createTexture(const BaseTextureInfo& info) = 0;
    //    };
}
