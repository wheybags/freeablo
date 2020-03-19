#pragma once
#include <filesystem/path.h>
#include <misc/misc.h>
#include <render/vertexlayout.h>

#include <utility>

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
        Pipeline(RenderInstance& instance, PipelineSpec spec) : mSpec(std::move(spec)), mInstance(instance) {}
        virtual ~Pipeline() = default;

    public:
        const PipelineSpec mSpec;

    protected:
        RenderInstance& mInstance;
    };
}