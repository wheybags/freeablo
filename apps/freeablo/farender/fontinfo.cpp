#include "fontinfo.h"

#include <boost/range/irange.hpp>
#include "cel/celdecoder.h"

#include <numeric>

namespace FARender
{
void FontInfo::initByTexture(Cel::CelDecoder& cel, int totalWidth)
{
    // this part is mostly based on fontgenerator/main.cpp
    std::array <int , 128 > mapping;
    uvLeft.fill(1.f);
    mapping.fill(-1);
    int index = 0, curOffset = 0;
    auto addLetters = [&](std::initializer_list <char> letters)
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
        addLetters({c, static_cast <char>(toupper(c))});

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

float FontInfo::getWidth(nk_handle handle, float /*h*/, const char* s, int len)
{
    auto info = static_cast <self *>(handle.ptr);
    return std::accumulate(s, s + len, 0, [info](int sum, char c) { return sum += (c >= 0 ? info->widthPx[c] : 0); });
}

void FontInfo::queryGlyph(nk_handle handle, float /*font_height*/, nk_user_font_glyph* glyph, nk_rune codepoint, nk_rune /*next_codepoint*/)
{
    if(codepoint > 127)
        return;
    auto info = static_cast <self *>(handle.ptr);
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

}
