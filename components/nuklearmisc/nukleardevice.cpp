#include "nukleardevice.h"
#include "nuklearvertex.h"
#include <render/pipeline.h>
#include <render/renderinstance.h>
#include <render/vertexarrayobject.h>

NuklearDevice::NuklearDevice(Render::RenderInstance& renderInstance, InitData&& initData)
{
    nullTexture = initData.nullTexture;
    atlas = initData.atlas;
    initData = {};

    Render::PipelineSpec pipelineSpec;
    pipelineSpec.vertexLayouts = {NuklearVertex::layout()};
    pipelineSpec.vertexShaderPath = Misc::getResourcesPath().str() + "/shaders/gui.vert";
    pipelineSpec.fragmentShaderPath = Misc::getResourcesPath().str() + "/shaders/gui.frag";
    pipelineSpec.descriptorSetSpec = {{
        {Render::DescriptorType::UniformBuffer, "vertexUniforms"},
        {Render::DescriptorType::UniformBuffer, "fragmentUniforms"},
        {Render::DescriptorType::Texture, "Texture"},
    }};
    pipelineSpec.scissor = true;

    pipeline = renderInstance.createPipeline(pipelineSpec);
    vertexArrayObject = renderInstance.createVertexArrayObject({0}, pipelineSpec.vertexLayouts, 1);
    descriptorSet = renderInstance.createDescriptorSet(pipelineSpec.descriptorSetSpec);
    uniformCpuBuffer = std::make_unique<GuiUniforms::CpuBufferType>(renderInstance.capabilities().uniformBufferOffsetAlignment);
    uniformBuffer = renderInstance.createBuffer(uniformCpuBuffer->getSizeInBytes());
}

NuklearDevice::~NuklearDevice() { nk_font_atlas_clear(&atlas); }
