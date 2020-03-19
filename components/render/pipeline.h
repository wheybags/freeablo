#pragma once
#include <filesystem/path.h>
#include <render/vertexlayout.h>
#include <misc/misc.h>

namespace Render
{
    struct PipelineSpec
    {
        std::vector<NonNullConstPtr<VertexLayout>> vertexLayouts;
        filesystem::path vertexShaderPath;
        filesystem::path fragmentShaderPath;
        // target framebuffer format
    };

    class RenderInstance;

    class Pipeline
    {
    public:
        Pipeline(Pipeline&) = delete;
        Pipeline(RenderInstance& instance, const PipelineSpec& spec) : mInstance(instance), mSpec(spec) {}
        virtual ~Pipeline() = default;

    protected:
        RenderInstance& mInstance;
        const PipelineSpec& mSpec;
    };
}