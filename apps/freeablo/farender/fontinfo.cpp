#include "fontinfo.h"
#include "cel/celdecoder.h"
#include "diabloexe/diabloexe.h"
#include "faio/fafileobject.h"
#include "render/levelobjects.h"
#include <numeric>

namespace FARender
{
    void CelFontInfo::initByFontData(const DiabloExe::FontData& fontData, int textureWidthPx, int spacing)
    {
        mSpacing = spacing;
        for (int i = 0; i < charCount; ++i)
        {
            auto fontIndex = fontData.charToFontIndex[i];
            auto frame = fontData.fontIndexToFrame[fontIndex];
            uvLeft[i] = (frame - 1.) / fontData.frameCount;
            widthPx[i] = fontData.frameToWidth[frame];
            uvWidth[i] = (widthPx[i] + 0.) / textureWidthPx;
        }
    }

    float CelFontInfo::getWidth(nk_handle handle, float /*h*/, const char* s, int len)
    {
        auto info = static_cast<self*>(handle.ptr);
        return std::accumulate(s, s + len, 0, [info](int sum, char c) {
            auto uc = static_cast<unsigned char>(c);
            return sum + info->widthPx[uc] + info->mSpacing;
        });
    }

    void CelFontInfo::queryGlyph(nk_handle handle, float /*font_height*/, nk_user_font_glyph* glyph, nk_rune codepoint, nk_rune /*next_codepoint*/)
    {
        auto info = static_cast<self*>(handle.ptr);
        glyph->width = info->widthPx[codepoint];
        glyph->height = info->nkFont.height;
        glyph->xadvance = info->widthPx[codepoint] + info->mSpacing;
        glyph->uv[0].x = info->uvLeft[codepoint];
        glyph->uv[0].y = 0.f;
        glyph->uv[1].x = info->uvLeft[codepoint] + info->uvWidth[codepoint];
        glyph->uv[1].y = 1.f;
        if (codepoint == ' ')
            glyph->uv[0] = glyph->uv[1] = {0.f, 0.f};
        glyph->offset.x = 0.f;
        glyph->offset.y = 0.f;
    }

    void PcxFontInfo::init(const std::string& binPath, const PcxFontInitData& fontInitData)
    {
        FAIO::FAFileObject file_handle(binPath);
        std::vector<unsigned char> buf(file_handle.FAsize());
        file_handle.FAfread(buf.data(), 1, buf.size());

        for (int i = 0; i < charCount; ++i)
        {
            auto& glyph = mGlyphs[i];
            int32_t charWidth = buf[i + 2];
            if (i == ' ')
                charWidth = buf[0];

            int32_t x, y;
            if (fontInitData.fontDirection == PcxFontInitData::LayoutOrder::horizontal)
            {
                x = (i * fontInitData.spacingX) % fontInitData.textureWidth;
                y = (i * fontInitData.spacingX) / fontInitData.textureWidth * fontInitData.spacingY;
            }
            else
            {
                x = (i * fontInitData.spacingY) / fontInitData.textureHeight * fontInitData.spacingX;
                y = (i * fontInitData.spacingY) % fontInitData.textureHeight;
            }

            glyph.width = charWidth;
            glyph.height = fontInitData.spacingY;
            glyph.xadvance = charWidth;
            glyph.uv[0].x = (float)x / fontInitData.textureWidth;
            glyph.uv[0].y = (float)y / fontInitData.textureHeight;
            glyph.uv[1].x = (float)(x + charWidth) / fontInitData.textureWidth;
            glyph.uv[1].y = (float)(y + fontInitData.spacingY) / fontInitData.textureHeight;
            glyph.offset.x = 0.f;
            glyph.offset.y = 0.f;
        }
    }

    float PcxFontInfo::getWidth(nk_handle handle, float /*h*/, const char* s, int len)
    {
        auto info = static_cast<self*>(handle.ptr);
        return std::accumulate(s, s + len, 0, [info](float sum, char c) { return sum += info->mGlyphs[static_cast<unsigned char>(c)].width; });
    }

    void PcxFontInfo::queryGlyph(nk_handle handle, float /*font_height*/, nk_user_font_glyph* glyph, nk_rune codepoint, nk_rune /*next_codepoint*/)
    {
        if (codepoint > 255)
            return;
        auto info = static_cast<self*>(handle.ptr);

        *glyph = info->mGlyphs[codepoint];
    }
}
