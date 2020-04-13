#pragma once
#include <filesystem/path.h>
#include <misc/misc.h>
#include <render/descriptorset.h>
#include <render/vertexlayout.h>
#include <utility>

namespace Render
{
    struct PipelineSpec
    {
        DescriptorSetSpec descriptorSetSpec;
        std::vector<NonNullConstPtr<VertexLayout>> vertexLayouts;
        filesystem::path vertexShaderPath;
        filesystem::path fragmentShaderPath;
        bool scissor = false;
        bool depthTest = false;
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