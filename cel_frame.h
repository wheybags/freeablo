#ifndef CEL_FRAME_H
#define CEL_FRAME_H

#include <stdint.h>
#include <vector>

class colour;
class Cel_file;

class Cel_frame
{
    class Helper
    {
        private:
            const Cel_frame& parent;
            size_t x;
        public:    
            Helper(const Cel_frame& _p, size_t _x) : parent(_p), x(_x) {}
            
            const colour& operator[](size_t y){return parent.raw_image[x + y*parent.width];}
    };

    public:
        size_t width;
        size_t height;
        
        Helper operator[] (size_t x) const { return Helper(*this, x); }

    private:
        friend class Helper;
        friend class Cel_file;
        
        std::vector<colour> raw_image;
};

#endif
