#pragma once
#include <cstdint>
#include <misc/simplevec2.h>
#include <render/color.h>
#include <variant>
#include <vector>

struct RectData
{
    Vec2Fix worldPosition;
    float w, h;
    Render::Color color;
};

namespace FARender
{
    using DebugRenderItem = std::variant<RectData>;
    using DebugRenderData = std::vector<DebugRenderItem>;
}