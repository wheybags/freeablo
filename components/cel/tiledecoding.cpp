#ifdef CEL_DEBUG
    #include <iostream>
#endif

#include "tiledecoding.h"
#include "celdecoding.h"

#include <misc/stringops.h>

bool greaterThanFirst(const std::vector<uint8_t>& frame)
{
    return frame.size() >= 196 &&
    frame[  2] == 0 && frame[  3] == 0 &&
    frame[ 14] == 0 && frame[ 15] == 0 &&
    frame[ 34] == 0 && frame[ 35] == 0 &&
    frame[ 62] == 0 && frame[ 63] == 0 &&
    frame[ 98] == 0 && frame[ 99] == 0 &&
    frame[142] == 0 && frame[143] == 0 &&
    frame[194] == 0 && frame[195] == 0;
}

bool greaterThanSecond(const std::vector<uint8_t>& frame)
{
    return frame.size() >= 196 &&
    frame[254] == 0 && frame[255] == 0 &&
    frame[318] == 0 && frame[319] == 0 &&
    frame[374] == 0 && frame[375] == 0 &&
    frame[422] == 0 && frame[423] == 0 &&
    frame[462] == 0 && frame[463] == 0 &&
    frame[494] == 0 && frame[495] == 0 &&
    frame[518] == 0 && frame[519] == 0 &&
    frame[534] == 0 && frame[535] == 0;
}

bool isGreaterThan(const std::vector<uint8_t>& frame)
{
    return greaterThanFirst(frame);
}

bool lessThanFirst(const std::vector<uint8_t>& frame)
{
    return frame.size() >= 226 &&
    frame[  0] == 0 && frame[  1] == 0 &&
    frame[  8] == 0 && frame[  9] == 0 &&
    frame[ 24] == 0 && frame[ 25] == 0 &&
    frame[ 48] == 0 && frame[ 49] == 0 &&
    frame[ 80] == 0 && frame[ 81] == 0 &&
    frame[120] == 0 && frame[121] == 0 &&
    frame[168] == 0 && frame[169] == 0 &&
    frame[224] == 0 && frame[225] == 0;
}

bool lessThanSecond(const std::vector<uint8_t>& frame)
{
    return frame.size() >= 530 &&
    frame[288] == 0 && frame[289] == 0 &&
    frame[348] == 0 && frame[349] == 0 &&
    frame[400] == 0 && frame[401] == 0 &&
    frame[444] == 0 && frame[445] == 0 &&
    frame[480] == 0 && frame[481] == 0 &&
    frame[508] == 0 && frame[509] == 0 &&
    frame[528] == 0 && frame[529] == 0;
}

bool isLessThan(const std::vector<uint8_t>& frame)
{
    return lessThanFirst(frame);
}

void fillTransparent(size_t pixels, std::vector<Colour>& rawImage)
{
    for(int px = 0; px < pixels; px++)
        rawImage.push_back(Colour(255, 255, 255, false));
}

void drawRow(int row, int lastRow, int& framePos, const std::vector<uint8_t>& frame, const Pal& pal, std::vector<Colour>& rawImage, bool lessThan)
{
    for(; row < lastRow; row++)
    {
        // Skip markers - for less than, when on the first half of the image (row < 16), all even rows will start with a pair of marker bits
        // for the second half of the image (row >= 16), all odd rows will start with a pair of marker bits.
        // The inverse is true of greater than images.
        if( (lessThan && ((row < 16 && row % 2 == 0) || (row >= 16 && row % 2 != 0))) ||
           (!lessThan && ((row < 16 && row % 2 != 0) || (row >= 16 && row % 2 == 0))))
            framePos += 2;
        
        int toDraw;
        if(row < 16)
            toDraw = 2 + (row * 2);
        else
            toDraw = 32 - ((row - 16) * 2);
        
        #ifdef CEL_DEBUG
            std::cout << "\trow: " << row << ", draw: " << toDraw << std::endl;
        #endif

        if(lessThan)
            fillTransparent(32-toDraw, rawImage);

        for(int px = 0; px < toDraw; px++)
        {
                rawImage.push_back(pal[frame[framePos]]);
                framePos++;
        }

        if(!lessThan)
            fillTransparent(32-toDraw, rawImage);
        
        #ifdef CEL_DEBUG
            std::cout << "len: " << rawImage.size() << std::endl;
        #endif
    }
}

void decodeGreaterLessThan(const std::vector<uint8_t>& frame, const Pal& pal, std::vector<Colour>& rawImage, bool lessThan)
{
    #ifdef CEL_DEBUG
        std::cout << (lessThan ? "Less" : "Greater") << " than" << std::endl;
    #endif

    int framePos = 0;
    
    drawRow(0, 15, framePos, frame, pal, rawImage, lessThan);

    
    if((lessThan && lessThanSecond(frame)) || (!lessThan && greaterThanSecond(frame)))
    {
        drawRow(16, 33, framePos, frame, pal, rawImage, lessThan);
    }
    else
    {
        for(framePos = 256; framePos < frame.size(); framePos++)
            rawImage.push_back(pal[frame[framePos]]);
    }
    
    #ifdef CEL_DEBUG
        std::cout << (lessThan ? "LT" : "GT") << rawImage.size() << std::endl;
    #endif

}

void decodeGreaterThan(const std::vector<uint8_t>& frame, const Pal& pal, std::vector<Colour>& rawImage)
{
    decodeGreaterLessThan(frame, pal, rawImage, false);
}

void decodeLessThan(const std::vector<uint8_t>& frame, const Pal& pal, std::vector<Colour>& rawImage)
{
    decodeGreaterLessThan(frame, pal, rawImage, true);
}

size_t decodeRaw32(const std::vector<uint8_t>& frame, const Pal& pal, std::vector<Colour>& rawImage)
{

    for(int i = 0; i < frame.size(); i++)
    {
        rawImage.push_back(pal[frame[i]]);
    }

    return 32;
}

bool isTileCel(const std::string& fileName)
{
    return 
    Misc::StringUtils::endsWith(fileName, "l1.cel") ||
    Misc::StringUtils::endsWith(fileName, "l2.cel") ||
    Misc::StringUtils::endsWith(fileName, "l3.cel") ||
    Misc::StringUtils::endsWith(fileName, "l4.cel") ||
    Misc::StringUtils::endsWith(fileName, "town.cel");
}

size_t decodeTileFrame(const std::vector<uint8_t>& frame, const Pal& pal, std::vector<Colour>& rawImage)
{
    if(frame.size() == 1024 /*&& frame_num != 2593*/) // It's a fully opaque raw frame, width 32, from a level tileset
        decodeRaw32(frame, pal, rawImage);

    else if(isLessThan(frame))
        decodeLessThan(frame, pal, rawImage);
    
    else if(isGreaterThan(frame))
        decodeGreaterThan(frame, pal, rawImage);
    else
        normalDecode(frame, pal, rawImage, true);

    return 32;
}
    

