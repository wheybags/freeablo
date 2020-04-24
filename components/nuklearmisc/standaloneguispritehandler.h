#pragma once
#include "nuklearframedump.h"
#include <Image/image.h>
#include <cstdint>
#include <fa_nuklear.h>
#include <input/inputmanager.h>
#include <map>
#include <render/render.h>
#include <render/texture.h>
#include <render/texturereference.h>
#include <vector>

namespace Render
{
    class SpriteGroup;
}

namespace NuklearMisc
{
    class StandaloneGuiHandler;

    class StandaloneGuiHandler
    {
    public:
        StandaloneGuiHandler(const std::string& title, const Render::RenderSettings& renderSettings);
        ~StandaloneGuiHandler();

        nk_context* getNuklearContext() { return &mCtx; }
        bool update();

    private:
        static void fontStashBegin(nk_font_atlas& atlas);
        static std::unique_ptr<Render::SpriteGroup> fontStashEnd(nk_context* ctx, NuklearDevice::InitData& initData);

        std::unique_ptr<NuklearDevice> mNuklearGraphicsContext;
        std::unique_ptr<Render::SpriteGroup> mNuklearFontTexture;

        nk_context mCtx = {};
        Input::InputManager mInput;
        std::unique_ptr<NuklearFrameDump> mNuklearData;
    };
}
