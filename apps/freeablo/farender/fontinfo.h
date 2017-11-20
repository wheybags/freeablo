#pragma once
#include "fa_nuklear.h"

#include <array>
#include <SDL_surface.h>

namespace Cel {
    class CelDecoder;
}

namespace FARender
{
class CelFontInfo
{
private:
    using self = CelFontInfo;
public:
    nk_user_font nkFont;
private:
    std::array <int, 128> widthPx;
    std::array <float, 128> uvLeft, uvWidth;

private:
    void initByCel(Cel::CelDecoder& cel, int totalWidth);
    static float getWidth(nk_handle handle, float h, const char* s, int len);
    static void queryGlyph(nk_handle handle, float font_height,
                           struct nk_user_font_glyph* glyph,
                           nk_rune codepoint, nk_rune next_codepoint);
    friend class Renderer;
};

class PcxFontInfo
{
private:
    using self = PcxFontInfo;
public:
    nk_user_font nkFont;
private:
    static const int charCount = 256;
    std::array <int, charCount> widthPx;
    std::array <float, charCount> uvWidth;

private:
    void initBySurface(const SDL_Surface *surface);
    static float getWidth(nk_handle handle, float h, const char* s, int len);
    static void queryGlyph(nk_handle handle, float font_height,
                           struct nk_user_font_glyph* glyph,
                           nk_rune codepoint, nk_rune next_codepoint);
    friend class Renderer;
};
}
