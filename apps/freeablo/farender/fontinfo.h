#pragma once
#include "fa_nuklear.h"
#include <array>

namespace DiabloExe
{
    class FontData;
}

namespace FARender
{
    class CelFontInfo
    {
    private:
        using self = CelFontInfo;

    public:
        static const int charCount = 256;
        nk_user_font nkFont;

    private:
        std::array<int, charCount> widthPx;
        std::array<float, charCount> uvLeft, uvWidth;
        int mSpacing;

    private:
        void initByFontData(const DiabloExe::FontData& fontData, int textureWidthPx, int spacing);
        static float getWidth(nk_handle handle, float h, const char* s, int len);
        static void queryGlyph(nk_handle handle, float font_height, struct nk_user_font_glyph* glyph, nk_rune codepoint, nk_rune next_codepoint);
        friend class Renderer;
    };

    struct PcxFontInitData
    {
        int32_t fontSize;
        int32_t textureWidth;
        int32_t textureHeight;
        int32_t spacingX;
        int32_t spacingY;
        enum class LayoutOrder
        {
            horizontal,
            vertical
        } fontDirection;
    };

    class PcxFontInfo
    {
    private:
        using self = PcxFontInfo;

    public:
        nk_user_font nkFont;

    private:
        static const int charCount = 256;
        std::array<struct nk_user_font_glyph, charCount> mGlyphs;

    private:
        void init(const std::string& binPath, const PcxFontInitData& fontInitData);
        static float getWidth(nk_handle handle, float h, const char* s, int len);
        static void queryGlyph(nk_handle handle, float font_height, struct nk_user_font_glyph* glyph, nk_rune codepoint, nk_rune next_codepoint);
        friend class Renderer;
    };
}
