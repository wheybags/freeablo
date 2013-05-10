#include <stdio.h>
#include <stdint.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <iostream>
#include <vector>
#include <algorithm>

#define WIDTH 1280
#define HEIGHT 960
#define BPP 4
#define DEPTH 32

struct colour
{
    uint8_t r;
    uint8_t g;
    uint8_t b;

    colour(uint8_t _r, uint8_t _g, uint8_t _b)
    {
        r = _r; g = _g; b = _b;
    }

    colour(){}
};

void setpixel_real(SDL_Surface *screen, int x, int y, colour c)
{
    y = y*screen->pitch/BPP;

    Uint32 *pixmem32;
    Uint32 colour;  
 
    colour = SDL_MapRGB( screen->format, c.r, c.g, c.b );
  
    pixmem32 = (Uint32*) screen->pixels  + y + x;
    *pixmem32 = colour;
}

void setpixel(SDL_Surface* screen, int x, int y, colour c)
{
    //setpixel_real(screen, x, y, c);
    //return;

    setpixel_real(screen, x*2, y*2, c);
    setpixel_real(screen, x*2+1, y*2, c);
    setpixel_real(screen, x*2, y*2+1, c);
    setpixel_real(screen, x*2+1, y*2+1, c);
}

void get_pal(std::string pal_filename, colour* pal)
{
    FILE * pal_file;

    pal_file = fopen(pal_filename.c_str(), "rb");
    
    for(int i = 0; i < 256; i++)
    {
            fread(&pal[i].r, 1, 1, pal_file);
            fread(&pal[i].g, 1, 1, pal_file);
            fread(&pal[i].b, 1, 1, pal_file);
    }

    fclose(pal_file);
}

size_t get_num_frames(FILE* cel_file)
{
    size_t num_frames;
    
    fread(&num_frames, 4, 1, cel_file);
    std::cout << ftell(cel_file) << ": Num frames: " << num_frames << std::endl;

    return num_frames;
}

void get_frame_offsets(FILE* cel_file, uint32_t* frame_offsets, size_t num_frames)
{
    for(int i = 0; i < num_frames; i++){
            fread(&frame_offsets[i], 4, 1, cel_file);
            std::cout << ftell(cel_file) << ": offset " << i << ": " << frame_offsets[i] << std::endl;
    }

    fread(&frame_offsets[num_frames], 4, 1, cel_file);
    std::cout << ftell(cel_file) << ": end offset: " << frame_offsets[num_frames] << std::endl;
}

void fix_image(std::vector<colour>& raw_image, size_t width)
{
    for(int i = 0; i < raw_image.size()/2; i++)
    {
        colour tmp = raw_image[i];
        raw_image[i] = raw_image[raw_image.size() - i];
        raw_image[raw_image.size() - i] = tmp;
    }

    for(int i = 0; i < raw_image.size(); i++)
    {
        int x = i%width;
        if(x < width/2)
        {
            colour tmp = raw_image[i];
            raw_image[i] = raw_image[(i-x+width) - x];
            raw_image[(i-x+width) - x] = tmp;
        }
    }
}

void print_cel(unsigned char* frame, int size)
{
    for(int i = 0; i < size; i++)
    {
        std::cout << i << ": " << (unsigned int) frame[i] << std::endl;
    }
}

void fill_t(size_t pixels, std::vector<colour>& raw_image)
{
    for(int px = 0; px < pixels; px++)
        raw_image.push_back(colour(255, 255, 255));
}


bool less_than_first(unsigned char* frame, size_t frame_size)
{
    return frame_size >= 256 &&
    frame[  0] == 0 && frame[  1] == 0 &&
    frame[  8] == 0 && frame[  9] == 0 &&
    frame[ 24] == 0 && frame[ 25] == 0 &&
    frame[ 48] == 0 && frame[ 49] == 0 &&
    frame[ 80] == 0 && frame[ 81] == 0 &&
    frame[120] == 0 && frame[121] == 0 &&
    frame[168] == 0 && frame[169] == 0 &&
    frame[224] == 0 && frame[225] == 0;
}

bool less_than_second(unsigned char* frame, size_t frame_size)
{
    return frame_size >= 530 &&
    frame[288] == 0 && frame[289] == 0 &&
    frame[348] == 0 && frame[349] == 0 &&
    frame[400] == 0 && frame[401] == 0 &&
    frame[444] == 0 && frame[445] == 0 &&
    frame[480] == 0 && frame[481] == 0 &&
    frame[508] == 0 && frame[509] == 0 &&
    frame[528] == 0 && frame[529] == 0;
}

bool is_less_than(unsigned char* frame, size_t frame_size)
{
    return less_than_first(frame, frame_size);
}





void decode_less_than(unsigned char* frame, size_t frame_size, colour* pal, std::vector<colour>& raw_image)
{
    int line;
    int i = 0;

    for(line = 0; line < 8; line++)
    {
        i += 2;

        int xdraw = line*4 +2;
        int xoffs = 32 - xdraw;

        std::cout << "\tdraw: " << xdraw << std::endl;

           
        fill_t(xoffs, raw_image);
        for(int px = xoffs; px < 32; px++)
        {
                std::cout << i << std::endl;
                raw_image.push_back(pal[frame[i]]);
                i++;
        }
        

        
        xdraw = line*4 +4;
        xoffs = 32 - xdraw;

        std::cout << "\tdraw: " << xdraw << std::endl;

        
        fill_t(xoffs, raw_image);
        for(int px = xoffs; px < 32; px++)
        {
                raw_image.push_back(pal[frame[i]]);
                i++;
        }
        
        
       
        std::cout << "len: " << raw_image.size() << std::endl;
    }
    
    if(less_than_second(frame, frame_size))
    {
        for(; line < 16; line++)
        {
            i += 2;

            int xdraw = (15-line)*4 +2;
            int xoffs = 32 - xdraw;

            std::cout << "\tdraw2: " << xdraw << std::endl;

            fill_t(xoffs, raw_image);
               
            for(int px = xoffs; px < 32; px++)
            {
                    raw_image.push_back(pal[frame[i]]);
                    i++;
            }
            

            
            xdraw = (15-line)*4;
            xoffs = 32 - xdraw;

            std::cout << "\tdraw2: " << xdraw << std::endl;

            
            fill_t(xoffs, raw_image);
            for(int px = xoffs; px < 32; px++)
            {
                    raw_image.push_back(pal[frame[i]]);
                    i++;
            }
            
            
           
            std::cout << "len2: " << raw_image.size() << std::endl;
        }
    }
    else
    {
        for(int i = 256; i < frame_size; i++)
            raw_image.push_back(pal[frame[i]]);
    }

}

size_t transparent_decode(unsigned char* frame, size_t frame_size, size_t width_override, bool from_header, uint16_t offset, colour* pal, std::vector<colour>& raw_image)
{

    bool first = true;
    
    //int x = 0, y = 0;

    int width_header, width_reg = 0, width = 99;
    int pixels = 0;

    bool reg_done = false;

    int tmp_pixels;


    for(int i = 0; i < frame_size; i++){

        tmp_pixels = 0;

        int temp = i;

        unsigned int c = frame[i];
 
        if(!reg_done)
        {
            if(128 <= c)
                width_reg += 256 - c;
            else
                width_reg += c;
            if(c != 127)
                reg_done = true;
        }
            
            
             
        // Regular command
        if((16 < c) && (c <= 127)){

            std::cout << i << " regular: " << c << std::endl;

            for(int j = 1; j < c+1; j++){
                //std::cout << "asasas" << std::endl;
                raw_image.push_back(pal[frame[i+j]]);
                /*raw_image.push_back(pal[frame[i+j]].r);
                raw_image.push_back(pal[frame[i+j]].g);
                raw_image.push_back(pal[frame[i+j]].b);*/
            }
            
            tmp_pixels += c;
            i+= c;
        }

        // Transparency command
        else if(128 <= c){

            std::cout << i << " transparency: " << c << " " << (256 - c) << std::endl;
            int run = 256 -c;
            for(int j = 0; j < run; j++){
                /*raw_image.push_back(255);
                raw_image.push_back(255);
                raw_image.push_back(255);*/
                raw_image.push_back(colour(255, 0, 255));
            }

            tmp_pixels += run;
        }
       
        // Block command 
        else if(c <= 16){

            std::cout << i << " block: " << c << std::endl;
            
            //if(c == 10 && i == 0){ i += 10; }
            if(first && from_header){
                std::cout << (int)frame[i+1] << " " << (int)frame[i+2] << std::endl;
                first = false;
                i += c;
                continue;
            }

            for(int j = 1; j < c+1; j++){
                raw_image.push_back(pal[frame[i+j]]);
                /*raw_image.push_back(pal[frame[i+j]].r);
                raw_image.push_back(pal[frame[i+j]].g);
                raw_image.push_back(pal[frame[i+j]].b);*/
            }
            
            tmp_pixels += c;
            i += c;
            
        }

        if(temp == offset && from_header)
            width_header = pixels / 31;
        else
            pixels += tmp_pixels;

        std::cout << "\t\tpixels: " << pixels << std::endl;


    }

    if(from_header)
        width = width_header;
    else
        width = width_reg;
    
    // should never be zero,  bigger than 640 unless we fail to detect width, so fallback to random value 
    if(width > 640) width = 32;
    if(width < 10) width = 32; 

    if(width_override)
        width = width_override;


    return width;
}

size_t decode_raw_32(unsigned char* frame, size_t frame_size, colour* pal, std::vector<colour>& raw_image)
{

    for(int i = 0; i < frame_size; i++)
    {
        raw_image.push_back(pal[frame[i]]);
    }

    return 32;
}

size_t get_frame(FILE* cel_file, colour* pal, uint32_t* frame_offsets, size_t frame_num, std::vector<colour>& raw_image, size_t width_override = 0)
{
    int frame_size = frame_offsets[frame_num+1] - frame_offsets[frame_num];
    std::cout << std::endl << "frame 0 size: " << frame_size << std::endl;


    fseek(cel_file, frame_offsets[frame_num], SEEK_SET);

    unsigned char frame[frame_size];
    /*fread(&frame[0], 1, 2, cel_file);

    uint16_t offset;
    
    bool from_header;

    // we have a header
    if(frame[0] == 10)
    {
        //fread(&offset, 2, 1, cel_file);
        //std::cout << offset << std::endl;
    
        //fread(&frame[4], 1, frame_size-4, cel_file);

        from_header = true;
    }
    else
    {
        //fread(&frame[2], 1, frame_size-2, cel_file);
        from_header = false;
    }*/

    fseek(cel_file, frame_offsets[frame_num], SEEK_SET);
    fread(&frame[0], 1, frame_size, cel_file);

  
     //std::cout << (int)(pal[255].r) << " " << (int)(pal[255].g) << " " << (int)(pal[255].b) << std::endl;
    
    //std::vector<colour> raw_image;
    
    // Make sure we're not concatenating onto some other image 
    raw_image.clear();
    

    print_cel(frame, frame_size);
    
    std::cout << std::endl;
   
    int width;
    
    if(is_less_than(frame, frame_size))
    {
        width = 32;
        decode_less_than(frame, frame_size, pal, raw_image);
    }
    else
    {
        uint16_t offset;
        bool from_header = false;
        
        // The frame has a header which we can use to determine width
        if(frame[0] == 10)
        {
            from_header = true;
            fseek(cel_file, frame_offsets[frame_num]+2, SEEK_SET);
            fread(&offset, 2, 1, cel_file);
        }

        width = transparent_decode(frame, frame_size, width_override, from_header, offset, pal, raw_image);
        
        
        if(raw_image.size() % width != 0) // It's a fully opaque raw frame, width 32, from a level tileset
        {
            raw_image.clear();
            width =  decode_raw_32(frame, frame_size, pal, raw_image);
        }
        

    }
    
    fix_image(raw_image, width);
  
    std::cout << "WIDTH used: " << width << std::endl;


    return width;
}


    
int main(int argc, char** argv){
    std::string pal_filename;
    size_t frame_num;
    if(argc >= 3)
        pal_filename = argv[2];
    else
        pal_filename = "diablo.pal";
    
    if(argc >= 4)
        frame_num = atoi(argv[3]);
    
    //size_t frame_num = 210;
    
    colour pal[256];
    get_pal(pal_filename, pal); 


    FILE * cel_file;
    cel_file = fopen(argv[1], "rb");

    size_t num_frames = get_num_frames(cel_file);
    
    uint32_t frame_offsets[num_frames+1];
    get_frame_offsets(cel_file, frame_offsets, num_frames);


    std::vector<colour> raw_image;
    size_t width = get_frame(cel_file, pal, frame_offsets, frame_num, raw_image);

    std::cout << width << std::endl; 






    SDL_Surface *screen;
    SDL_Event event;
  
    int h=0; 
  
    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) return 1;
   
    if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE)))
    {
        SDL_Quit();
        return 1;
    }


    
    bool quit = false;
    while(!quit)
    {
        while(SDL_PollEvent(&event)){
            switch (event.type) 
            {
                case SDL_QUIT:
	                quit = true;
	                break;

                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym){
                        case SDLK_UP:
                            if(frame_num == num_frames-1) break;
                            frame_num++;
                            break;
                        case SDLK_DOWN:
                            if(frame_num == 0) break;
                            frame_num--;
                            break;
                        case SDLK_LEFT:
                            width--;
                            break;
                        case SDLK_RIGHT:
                            width++;
                            break;
                    } 
                    get_frame(cel_file, pal, frame_offsets, frame_num, raw_image, width);
                    std::cout << "frame: " << frame_num << "/" << num_frames << std::endl;
                    std::cout << width << std::endl;
            }
        }

        
        if(SDL_MUSTLOCK(screen)) 
        {
            if(SDL_LockSurface(screen) < 0); continue;
        }

        SDL_FillRect(screen,NULL, SDL_MapRGB( screen->format, 0, 0, 255)); 
        
        int x = 0, y = 0;
        for(int i = 0; i < raw_image.size(); i++)
        {

            setpixel(screen, x, y, raw_image[i]);
            
            x++;
            if(x >= width){
                x = 0;
                y++;
            }
        }


        
        
        /*
        for(int j = 0; j < 256; j++)
        {
            setpixel(screen, j, 0, pal[j]);
        }
        */

        
        if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
      
        SDL_Flip(screen); 
        //while(1);
    }
}
