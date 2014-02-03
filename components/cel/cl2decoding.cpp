#ifdef CEL_DEBUG
    #include <iostream>
#endif

#include "cl2decoding.h"

namespace Cel
{
    int32_t cl2Width(const std::vector<uint8_t>& frame, uint16_t offset)
    {
        int32_t pixels = 0;

        size_t i = 10; // CL2 frames always have headers

        for(; i < frame.size(); i++)
        {
            if(i == offset)
                return pixels / 32;

            // Color command
            if(frame[i] > 127)
            {
                uint8_t val = 256 - frame[i];
               
                // Regular command
                if(val <= 65)
                {
                    pixels += val;
                    i+= val;
                }

                // RLE (run length encoded) Colour command
                else
                {
                    pixels += val-65; 
                    i += 1;
                }
            }

            // Transparency command
            else
            {
                pixels += frame[i];
            }
        }
        
        return -1; // keep the compiler happy
    }

    int32_t cl2Decode(const std::vector<uint8_t>& frame, const Pal& pal, std::vector<Colour>& rawImage)
    {
        #ifdef CEL_DEBUG
            std::cout << "CL2 DECODE" << std::endl;
        #endif

        size_t i = 10; // CL2 frames always have headers

        for(; i < frame.size(); i++)
        {
            // Color command
            if(frame[i] > 127)
            {
                uint8_t val = 256 - frame[i];
               
                // Regular command
                if(val <= 65)
                {
                    #ifdef CEL_DEBUG
                        std::cout << i << " regular: " << (int)val << std::endl;
                    #endif

                    size_t j;
                    // Just push the number of pixels specified by the command
                    for(j = 1; j < val+1 && i+j < frame.size(); j++)
                    {
                        int index = i+j;
                        uint8_t f = frame[index];
                        
                        if(index > frame.size()-1)
                            std::cout << "invalid read from f " << index << " " << frame.size() << std::endl;

                        Colour col = pal[f];

                        rawImage.push_back(col);
                    }
                    
                    i+= val;
                }

                // RLE (run length encoded) Colour command
                else
                {
                    #ifdef CEL_DEBUG
                        std::cout << "RLE Colour: " << (int)val-65 << std::endl;
                    #endif

                    for(int j = 0; j < val-65; j++)
                        rawImage.push_back(pal[frame[i+1]]);
                    
                    i += 1;
                }
            }

            // Transparency command
            else
            {
                #ifdef CEL_DEBUG
                    std::cout << i << " transparency: " << (int)frame[i] <<  std::endl;
                #endif
                
                // Push transparent pixels
                for(size_t j = 0; j < frame[i]; j++)
                    rawImage.push_back(Colour(255, 0, 255, false));
            }
        }

        uint16_t offset = (uint16_t) (frame[3] << 8 | frame[2]);
        return cl2Width(frame, offset);
    }
}
