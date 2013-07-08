#ifndef CEL_FRAME_H
#define CEL_FRAME_H

#include <stdint.h>
#include <vector>

class colour;
class Cel_file;

class Cel_frame
{
    class Cel_Frame_Helper
    {
        private:
            const Cel_frame& parent;
            size_t x;
        public:    
            Cel_Frame_Helper(const Cel_frame& _p, size_t _x) : parent(_p), x(_x) {}
            
            const colour& operator[](size_t y){return parent.raw_image[x + (parent.height-1-y)*parent.width];}
    };

    public:
        size_t width;
        size_t height;
        
        Cel_Frame_Helper operator[] (size_t x) const { return Cel_Frame_Helper(*this, x); }

    private:
        friend class Cel_Frame_Helper;
        friend class Cel_file;
        
        std::vector<colour> raw_image;
};

#endif
