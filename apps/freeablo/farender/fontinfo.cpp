#include "fontinfo.h"

#include <boost/range/irange.hpp>
#include "cel/celdecoder.h"

#include <numeric>
#include "render/levelobjects.h"

namespace FARender
{
    void CelFontInfo::initByCel(Cel::CelDecoder& cel, int totalWidth)
    {
        // this part is mostly based on fontgenerator/main.cpp
        std::array<int, 128> mapping;
        uvLeft.fill(1.f);
        mapping.fill(-1);
        int index = 0, curOffset = 0;
        auto addLetters = [&](std::initializer_list<char> letters)
        {
            int actualWidth = 0;
            for(auto i : boost::irange(0, cel[index].mWidth))
                for(auto j : boost::irange(0, cel[index].mHeight))
                    if(cel[index][i][j].visible)
                        actualWidth = std::max(actualWidth, i);
            actualWidth += 2;
            actualWidth = std::min(actualWidth, cel[index].mWidth);
            for(auto c : letters)
            {
                widthPx[c] = actualWidth;
                uvLeft[c] = (curOffset + 0.f) / totalWidth;
                uvWidth[c] = (actualWidth + 0.f) / totalWidth;
            }
            curOffset += cel[index].mWidth;
            ++index;
        };

        for(char c = 'a'; c <= 'z'; c++)
            addLetters({c, static_cast<char>(toupper(c))});

        for(char c = '1'; c <= '9'; c++)
            addLetters({c});

        addLetters({'0'});

        for(auto c : {
            '-', '=', '+', '(', ')', '[', ']', '"', '\0'/*TODO: figure out unicode for this symbol*/,
            '`', '\'',':',';',',','.','/','?','!','&','%',
            '#','$','*','<','>','@','\\','^','_','|','~'
        })
            addLetters({c});
        widthPx[' '] = 11;
    }

    float CelFontInfo::getWidth(nk_handle handle, float /*h*/, const char* s, int len)
    {
        auto info = static_cast<self *>(handle.ptr);
        return std::accumulate(s, s + len, 0, [info](int sum, char c) { return sum += (c >= 0 ? info->widthPx[c] : 0); });
    }

    void CelFontInfo::queryGlyph(nk_handle handle, float /*font_height*/, nk_user_font_glyph* glyph, nk_rune codepoint,
                                 nk_rune /*next_codepoint*/)
    {
        if(codepoint > 127)
            return;
        auto info = static_cast<self *>(handle.ptr);
        glyph->width = info->widthPx[codepoint];
        glyph->height = info->nkFont.height;
        glyph->xadvance = info->widthPx[codepoint];
        glyph->uv[0].x = info->uvLeft[codepoint];
        glyph->uv[0].y = 0.f;
        glyph->uv[1].x = info->uvLeft[codepoint] + info->uvWidth[codepoint];
        glyph->uv[1].y = 1.f;
        if(codepoint == ' ')
            glyph->uv[0] = glyph->uv[1] = {0.f, 0.f};
        glyph->offset.x = 0.f;
        glyph->offset.y = 0.f;
    }

    void PcxFontInfo::initBySurface(const SDL_Surface* surface)
    {
        auto char_height = surface->h / charCount;
        for(int i = 0; i < charCount; ++i)
        {
            int actualWidth = 0;
            for(auto j : boost::irange(0, surface->w))
            {
                for(auto k : boost::irange(0, char_height))
                {
                    if(Render::getPixel(surface, j, k + i * char_height).visible)
                        actualWidth = std::max(actualWidth, j);
                }
            }
            actualWidth += 2;
            widthPx[i] = actualWidth;
            uvWidth[i] = (actualWidth + 0.0) / surface->w;
        }
        widthPx[' '] = 22;
    }

    float PcxFontInfo::getWidth(nk_handle handle, float h, const char* s, int len)
    {
        auto info = static_cast<self *>(handle.ptr);
        return std::accumulate(s, s + len, 0, [info](int sum, char c) { return sum += info->widthPx[static_cast<unsigned char>(c)]; });
    }

    void PcxFontInfo::queryGlyph(nk_handle handle, float font_height, nk_user_font_glyph* glyph, nk_rune codepoint, nk_rune next_codepoint)
    {
        if(codepoint > 255)
            return;
        auto info = static_cast<self *>(handle.ptr);
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
