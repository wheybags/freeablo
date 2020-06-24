#pragma once
#include "vertexlayout.h"
#include <cstdint>
#include <misc/assert.h>

namespace Render
{
#pragma pack(push, 1)
    struct BasicVertex
    {
        float vertex_position[2];
        float vertex_uv[2];

        static const VertexLayout& layout()
        {
            static VertexLayout layout{{
                                           Format::RG32F,
                                           Format::RG32F,
                                       },
                                       VertexInputRate::ByVertex};

            debug_assert(layout.getSizeInBytes() == sizeof(BasicVertex));

            return layout;
        }
    };

    struct DebugVertex
    {
        float vertex_position[2];
        float vertex_color[4];

        static const VertexLayout& layout()
        {
            static VertexLayout layout{{
                                           Format::RG32F,
                                           Format::RGBA32F,
                                       },
                                       VertexInputRate::ByVertex};

            debug_assert(layout.getSizeInBytes() == sizeof(DebugVertex));

            return layout;
        }
    };

    struct SpriteVertexMain
    {
        float vertex_position[2];
        float v_uv[2];

        static const VertexLayout& layout()
        {
            static VertexLayout layout{{
                                           Format::RG32F,
                                           Format::RG32F,
                                       },
                                       VertexInputRate::ByVertex};

            debug_assert(layout.getSizeInBytes() == sizeof(SpriteVertexMain));

            return layout;
        }
    };

    struct SpriteVertexPerInstance
    {
        float v_zValue;
        uint16_t v_spriteSizeInPixels[2];
        int16_t v_destinationInPixels[2];
        uint8_t v_hoverColor[4];
        uint16_t v_atlasOffsetInPixels[2];

        static const VertexLayout& layout()
        {
            static VertexLayout layout{{
                                           Format::R32F,
                                           Format::RG16U,
                                           Format::RG16I,
                                           Format::RGBA8UNorm,
                                           Format::RG16U,
                                       },
                                       VertexInputRate::ByInstance};

            debug_assert(layout.getSizeInBytes() == sizeof(SpriteVertexPerInstance));

            return layout;
        }
    };
#pragma pack(pop)
}