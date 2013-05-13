#ifndef CEL_FILE_H
#define CEL_FILE_H

#include <stdint.h>
#include <vector>

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
        
        size_t get_frame(size_t frame_num, std::vector<colour>& raw_image);

        size_t get_num_frames();

    private:
        size_t get_frame(FILE* cel_file, colour* pal, uint32_t* frame_offsets, size_t frame_num, std::vector<colour>& raw_image, bool is_tile_cel = false, size_t width_override = 0);
        
        FILE * mFile;
        
        std::vector<colour> mRaw_image;
        size_t mWidth;
        size_t mHeight;
        
        colour* mPal;
        
        size_t mNum_frames;
        uint32_t* mFrame_offsets;

        bool mIs_tile_cel;
};

void get_pal(std::string pal_filename, colour* pal);
size_t get_num_frames(FILE* cel_file);
void get_frame_offsets(FILE* cel_file, uint32_t* frame_offsets, size_t num_frames);
void fix_image(std::vector<colour>& raw_image, size_t width);
void print_cel(uint8_t* frame, int size);
void fill_t(size_t pixels, std::vector<colour>& raw_image);
bool greater_than_first(uint8_t* frame, size_t frame_size);
bool greater_than_second(uint8_t* frame, size_t frame_size);
bool is_greater_than(uint8_t* frame, size_t frame_size);
void decode_greater_than(uint8_t* frame, size_t frame_size, colour* pal, std::vector<colour>& raw_image);
bool less_than_first(uint8_t* frame, size_t frame_size);
bool less_than_second(uint8_t* frame, size_t frame_size);
bool is_less_than(uint8_t* frame, size_t frame_size);
void decode_less_than(uint8_t* frame, size_t frame_size, colour* pal, std::vector<colour>& raw_image);
int32_t normal_width(uint8_t* frame, size_t frame_size, bool from_header, uint16_t offset);
bool normal_decode(uint8_t* frame, size_t frame_size, size_t width, bool from_header, colour* pal, std::vector<colour>& raw_image);
size_t decode_raw_32(uint8_t* frame, size_t frame_size, colour* pal, std::vector<colour>& raw_image);


#endif
