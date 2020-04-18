#pragma once
#include <cstdint>
#include <fa_nuklear.h>
#include <misc/assert.h>
#include <render/vertexlayout.h>

#pragma pack(push, 1)
struct NuklearVertex
{
    float position[2];
    float uv[2];
    uint8_t color[4];

    static const Render::VertexLayout& layout()
    {
        static Render::VertexLayout layout{{
                                               Render::Format::RG32F,
                                               Render::Format::RG32F,
                                               Render::Format::RGBA8UNorm,
                                           },
                                           Render::VertexInputRate::ByVertex};

        debug_assert(layout.getSizeInBytes() == sizeof(NuklearVertex));

        return layout;
    }
};

// clang-format off
static const struct nk_draw_vertex_layout_element nuklearVertexLayout[] =
{
    {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct NuklearVertex, position)},
    {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct NuklearVertex, uv)},
    {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(struct NuklearVertex, color)},
    {NK_VERTEX_LAYOUT_END}
};
//clang-format on

#pragma pack(pop)