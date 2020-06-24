#pragma once
#include "nuklearbuffer.h"
#include "nukleardevice.h"

namespace Render
{
    class CommandQueue;
    class TextureReference;
    class Texture;
}

enum class GuiEffectType
{
    none = 0,
    highlighted,
    checkerboarded,
};

class NuklearFrameDump
{
public:
    NuklearFrameDump() = delete;
    NuklearFrameDump(const NuklearFrameDump&) = delete;

    NuklearFrameDump(NuklearDevice& dev);
    NuklearFrameDump(NuklearFrameDump&& other) = default;

    void fill(nk_context* ctx);
    void render(Vec2i screenResolution, Render::CommandQueue& commandQueue);

public:
    NuklearDevice& mDevice;

private:
    nk_convert_config mConvertConfig = {};

    NuklearBuffer mVertexBuffer = {};
    NuklearBuffer mIndexBuffer = {};

    NuklearBuffer mCommandsTemp = {};
    std::vector<nk_draw_command> mDrawCommands;
};