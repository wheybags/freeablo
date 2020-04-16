#pragma once
#include "atlastexture.h"
#include "misc.h"
#include <SDL.h>
#include <fa_nuklear.h>
#include <render/alignedcpubuffer.h>
#include <vector>

namespace Render
{
    class RenderInstance;
    class Pipeline;
    class VertexArrayObject;
    class Buffer;
    class DescriptorSet;
    class SpriteGroup;
    class Texture;
}

struct FANuklearTextureHandle
{
    const Render::AtlasTextureEntry* spriteGroup = nullptr;
    Render::Texture* texture = nullptr;
};

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
        float atlasOffset[4];
        float checkerboarded;

        float _pad[3];
    };

    using CpuBufferType = Render::TypedAlignedCpuBuffer<Vertex, Fragment>;
}

struct NuklearDevice
{
    NuklearDevice(Render::RenderInstance& renderInstance);
    ~NuklearDevice();

    nk_buffer commands = {};
    nk_draw_null_texture nullTexture = {};
    nk_handle fontTexture = {};

    std::unique_ptr<Render::Pipeline> pipeline;
    std::unique_ptr<Render::VertexArrayObject> vertexArrayObject;
    std::unique_ptr<Render::DescriptorSet> descriptorSet;
    std::unique_ptr<GuiUniforms::CpuBufferType> uniformCpuBuffer;
    std::unique_ptr<Render::Buffer> uniformBuffer;
};

class NuklearFrameDump
{
public:
    NuklearFrameDump() = delete;
    NuklearFrameDump(const NuklearFrameDump&) = delete;

    NuklearFrameDump(NuklearDevice& dev);
    ~NuklearFrameDump();

    void fill(nk_context* ctx);
    void render(Vec2i screenResolution, Render::CommandQueue& commandQueue);

public:
    NuklearDevice& mDevice;

private:
    nk_convert_config mConvertConfig = {};

    nk_buffer mVertexBuffer;
    nk_buffer mIndexBuffer;

    nk_buffer mCommandsTemp = {};
    std::vector<nk_draw_command> mDrawCommands;
};