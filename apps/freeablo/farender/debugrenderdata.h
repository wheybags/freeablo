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

struct TileData
{
    Vec2i worldPosition;
    Render::Color color;
};

struct PointData
{
    Vec2Fix worldPosition;
    Render::Color color;
    int32_t radiusInPixels;
};

namespace FARender
{
    using DebugRenderItem = std::variant<RectData, TileData, PointData>;
    using DebugRenderData = std::vector<DebugRenderItem>;
}