#ifndef CEL_FILE_H
#define CEL_FILE_H

#include <stdint.h>
#include <vector>
#include <map>

class Cel_frame;


struct colour
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    bool visible;

    colour(uint8_t _r, uint8_t _g, uint8_t _b, bool _visible)
    {
        r = _r; g = _g; b = _b; visible = _visible;
    }

    colour(){ visible = true; }
};

class Cel_file
{
    public:
        Cel_file(std::string filename);
        

        size_t get_num_frames();

        Cel_frame& operator[] (size_t index);

    private:
        size_t get_frame(size_t frame_num, std::vector<colour>& raw_image);


        size_t read_num_frames();
        void read_frame_offsets();
        
        void fix_image(std::vector<colour>& raw_image, size_t width);
        
        static bool greater_than_first(uint8_t* frame, size_t frame_size);
        static bool greater_than_second(uint8_t* frame, size_t frame_size);
        static bool is_greater_than(uint8_t* frame, size_t frame_size);
        
        void decode_greater_than(uint8_t* frame, size_t frame_size, colour* pal, std::vector<colour>& raw_image);


        FILE * mFile;
        
        std::vector<colour> mRaw_image;
        
        colour* mPal;
        
        size_t mNum_frames;
        uint32_t* mFrame_offsets;

        bool mIs_tile_cel;
        
        std::map<size_t, Cel_frame> mCache;
};

void get_pal(std::string pal_filename, colour* pal);
bool less_than_first(uint8_t* frame, size_t frame_size);
bool less_than_second(uint8_t* frame, size_t frame_size);
bool is_less_than(uint8_t* frame, size_t frame_size);
void decode_less_than(uint8_t* frame, size_t frame_size, colour* pal, std::vector<colour>& raw_image);
int32_t normal_width(uint8_t* frame, size_t frame_size, bool from_header, uint16_t offset);
void normal_decode(uint8_t* frame, size_t frame_size, size_t width, bool from_header, colour* pal, std::vector<colour>& raw_image);
size_t decode_raw_32(uint8_t* frame, size_t frame_size, colour* pal, std::vector<colour>& raw_image);


#endif
