#ifndef CEL_FRAME_H
#define CEL_FRAME_H

#include <stdint.h>
#include <vector>

class Colour;
class CelFile;

class CelFrame
{
    class Cel_Frame_Helper
    {
        private:
            const CelFrame& parent;
            size_t x;
        public:    
            Cel_Frame_Helper(const CelFrame& _p, size_t _x) : parent(_p), x(_x) {}
            
            const Colour& operator[](size_t y){return parent.rawImage[x + (parent.height-1-y)*parent.width];}
    };

    public:
        size_t width;
        size_t height;
        
        Cel_Frame_Helper operator[] (size_t x) const { return Cel_Frame_Helper(*this, x); }

    private:
        friend class Cel_Frame_Helper;
        friend class CelFile;
        
        std::vector<Colour> rawImage;
};

#endif
