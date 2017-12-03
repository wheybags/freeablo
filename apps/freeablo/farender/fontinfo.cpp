#include "fontinfo.h"

#include "cel/celdecoder.h"
#include <boost/range/irange.hpp>

#include "faio/fafileobject.h"
#include "render/levelobjects.h"
#include <numeric>
#include "diabloexe/diabloexe.h"

namespace FARender
{
    void CelFontInfo::initByFontData(const DiabloExe::FontData &fontData, int textureWidthPx, int spacing)
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
        auto info = static_cast<self *>(handle.ptr);
        return std::accumulate(s, s + len, 0, [info](int sum, char c)
        {
            auto uc = static_cast<unsigned char> (c);
            return sum + info->widthPx[uc] + info->mSpacing;
        });
    }

    void CelFontInfo::queryGlyph(nk_handle handle, float /*font_height*/, nk_user_font_glyph* glyph, nk_rune codepoint, nk_rune /*next_codepoint*/)
    {
        auto info = static_cast<self *>(handle.ptr);
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

    void PcxFontInfo::initWidths(const std::string& binPath, int textureWidth)
    {
        FAIO::FAFileObject file_handle(binPath);
        std::vector<unsigned char> buf(file_handle.FAsize());
        file_handle.FAfread(buf.data(), 1, buf.size());
        for (int i = 0; i < charCount; ++i)
        {
            widthPx[i] = buf[i + 2];
            uvWidth[i] = (widthPx[i] + 0.0) / textureWidth;
        }
        widthPx[' '] = buf[0];
    }

    float PcxFontInfo::getWidth(nk_handle handle, float /*h*/, const char* s, int len)
    {
        auto info = static_cast<self*>(handle.ptr);
        return std::accumulate(s, s + len, 0, [info](int sum, char c) { return sum += info->widthPx[static_cast<unsigned char>(c)]; });
    }

    void PcxFontInfo::queryGlyph(nk_handle handle, float /*font_height*/, nk_user_font_glyph* glyph, nk_rune codepoint, nk_rune /*next_codepoint*/)
    {
        if (codepoint > 255)
            return;
        auto info = static_cast<self*>(handle.ptr);
        glyph->width = info->widthPx[codepoint];
        glyph->height = info->nkFont.height;
        glyph->xadvance = info->widthPx[codepoint];
        glyph->uv[0].x = 0.0f;
        glyph->uv[0].y = (codepoint + 0.f) / charCount;
        glyph->uv[1].x = 0.0f + info->uvWidth[codepoint];
        glyph->uv[1].y = (codepoint + 1.f) / charCount;
        glyph->offset.x = 0.f;
        glyph->offset.y = 0.f;
    }
}
