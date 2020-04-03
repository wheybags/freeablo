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
    Render::SpriteGroup* spriteGroup = nullptr;
    Render::Texture* texture = nullptr;

    uint32_t frameNumber = 0;
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

struct nk_gl_device
{
    nk_buffer cmds;
    nk_draw_null_texture null;
    Render::VertexArrayObject* vertexArrayObject = nullptr;
    Render::Pipeline* pipeline = nullptr;
    Render::DescriptorSet* descriptorSet = nullptr;
    Render::Buffer* uniformBuffer = nullptr;
    GuiUniforms::CpuBufferType* uniformCpuBuffer = nullptr;
    nk_handle font_tex;
};

class NuklearFrameDump
{
public:
    NuklearFrameDump() {}
    NuklearFrameDump(const NuklearFrameDump&) = delete;

    NuklearFrameDump(nk_gl_device& dev);
    ~NuklearFrameDump();

    void init(nk_gl_device& dev);

    void fill(nk_context* ctx);
    nk_gl_device& getDevice();

    nk_buffer vbuf; // vertices
    nk_buffer ebuf; // indices

    std::vector<nk_draw_command> drawCommands;

private:
    nk_gl_device* dev = nullptr;
    nk_convert_config config;
    nk_buffer cmds; // draw commands temp storage
};

void nk_sdl_render_dump(NuklearFrameDump& dump, SDL_Window* win, Render::AtlasTexture& atlasTexture, Render::CommandQueue& commandQueue);
void nk_sdl_device_destroy(nk_gl_device& dev);
void nk_sdl_device_create(nk_gl_device& dev, Render::RenderInstance& renderInstance);
