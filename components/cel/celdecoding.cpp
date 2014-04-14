#include <iostream>

#include "celdecoding.h"

namespace Cel
{
    int32_t normalWidth(const std::vector<uint8_t>& frame, bool fromHeader, uint16_t offset)
    {
        
        // If we have a header, we know that offset points to the end of the 32nd line.
        // So, when we reach that point, we will have produced 32 lines of pixels, so we 
        // can divide the number of pixels we have passed at this point by 32, to get the 
        // width.
        if(fromHeader)
        {
            int32_t widthHeader = 0; 
            
            for(size_t i = 10; i < frame.size(); i++){
                
                if(i == offset && fromHeader)
                {
                    widthHeader = widthHeader/32;
                    break;
                }
                // Regular command
                if(frame[i] <= 127){
                    widthHeader += frame[i];
                    i += frame[i];
                }

                // Transparency command
                else if(128 <= frame[i]){
                    widthHeader += 256 - frame[i];
                }
            }

            return widthHeader;
        }
        
        // If we do not have a header we probably (definitely?) don't have any transparency.
        // The maximum stretch of opaque pixels following a command byte is 127.
        // Since commands can't wrap over lines (it seems), if the width is shorter than 127,
        // the first (command) byte will indicate an entire line, so it's value is the width.
        // If the width is larger than 127, it will be some sequence of 127 byte long stretches,
        // followed by some other value to bring it to the end of a line (presuming the width is
        // not divisible by 127).
        // So, for all image except those whose width is divisible by 127, we can determine width
        // by looping through control bits, adding 127 each time, until we find some value which
        // is not 127, then add that to the 127-total and that is our width.
        else
        {
            int32_t widthRegular = 0;
            
            for(size_t i = 0; i < frame.size(); i++){

                // Regular command
                if(frame[i] <= 127){
                    widthRegular += frame[i];
                    i += frame[i];
                }

                // Transparency command - who knows, it might be possible
                else if(128 <= frame[i]){
                    widthRegular += 256 - frame[i];
                }

                if(frame[i] != 127)
                    break;

            }

            return widthRegular;
        }
    }


    int32_t normalDecode(const std::vector<uint8_t>& frame, const Pal& pal, std::vector<Colour>& rawImage, bool tileCel)
    {
        #ifdef CEL_DEBUG
            std::cout << "NORMAL DECODE" << std::endl;
        #endif 
        
        size_t i = 0;

        uint16_t offset = 0;
        bool fromHeader = false;
        
        // The frame has a header which we can use to determine width
        if(!tileCel && frame[0] == 10)
        {
            fromHeader = true;
            offset = (uint16_t) (frame[3] << 8 | frame[2]);
            i = 10; // Skip the header
        }
     
        for(; i < frame.size(); i++)
        {
            // Regular command
            if(frame[i] <= 127)
            {
                #ifdef CEL_DEBUG
                    std::cout << i << " regular: " << (int)frame[i] << std::endl;
                #endif
                
                size_t j;
                // Just push the number of pixels specified by the command
                for(j = 1; j < frame[i]+1 && i+j < frame.size(); j++)
                {
                    int index = i+j;
                    uint8_t f = frame[index];
                    
                    if(index > frame.size()-1)
                        std::cout << "invalid read from f " << index << " " << frame.size() << std::endl;

                    Colour col = pal[f];

                    rawImage.push_back(col);
                }
                
                i+= frame[i];
            }


            // Transparency command
            else if(128 <= frame[i])
            {
                #ifdef CEL_DEBUG
                    std::cout << i << " transparency: " << (int)frame[i] << " " << (256 - frame[i]) << std::endl;
                #endif
                
                // Push (256 - command value) transparent pixels
                for(size_t j = 0; j < 256-frame[i]; j++)
                    rawImage.push_back(Colour(255, 0, 255, false));
            }
        }

        return normalWidth(frame, fromHeader, offset);
    }
}
