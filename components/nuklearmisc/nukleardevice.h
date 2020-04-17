#pragma once
#include <fa_nuklear.h>
#include <memory>
#include <render/alignedcpubuffer.h>

namespace Render
{
    class RenderInstance;
    class Pipeline;
    class VertexArrayObject;
    class DescriptorSet;
    class Buffer;
}

namespace GuiUniforms
{
    struct Vertex
    {
        float ProjMtx[4][4];
    };

    struct Fragment
    {
        float hoverColor[4];
        float imageSize[2];
        float atlasSize[2];
        float atlasOffset[2];
        float checkerboarded;

        float pad1;
    };

    using CpuBufferType = Render::TypedAlignedCpuBuffer<Vertex, Fragment>;
}

struct NuklearDevice
{
    struct InitData
    {
        nk_font_atlas atlas = {};
        nk_draw_null_texture nullTexture = {};
    };

    NuklearDevice(Render::RenderInstance& renderInstance, InitData&& initData);
    ~NuklearDevice();

    nk_draw_null_texture nullTexture = {};
    nk_font_atlas atlas = {};

    std::unique_ptr<Render::Pipeline> pipeline;
    std::unique_ptr<Render::VertexArrayObject> vertexArrayObject;
    std::unique_ptr<Render::DescriptorSet> descriptorSet;
    std::unique_ptr<GuiUniforms::CpuBufferType> uniformCpuBuffer;
    std::unique_ptr<Render::Buffer> uniformBuffer;
};
