#include <stdio.h>

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
    unsigned char r;
    unsigned char g;
    unsigned char b;

    colour(unsigned char _r, unsigned char _g, unsigned char _b)
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

int main(int argc, char** argv){
    std::string pal_filename;
    if(argc == 3)
        pal_filename = argv[2];
    else
        pal_filename = "diablo.pal";


    FILE * pFile;

    pFile = fopen(argv[1], "rb");

    unsigned int num_frames;
    num_frames = 99999;

    fread(&num_frames, 4, 1, pFile);
    std::cout << ftell(pFile) << ": Num frames: " << num_frames << std::endl;
    
    unsigned int frame_offsets[num_frames+1];

    for(int i = 0; i < num_frames; i++){
            fread(&frame_offsets[i], 4, 1, pFile);
            std::cout << ftell(pFile) << ": offset " << i << ": " << frame_offsets[i] << std::endl;
    }

    unsigned int end_offset;
    fread(&end_offset, 4, 1, pFile);
    std::cout << ftell(pFile) << ": end offset: " << end_offset << std::endl;
    frame_offsets[num_frames] = end_offset;


    int frame0_size = frame_offsets[1] - frame_offsets[0];
    std::cout << std::endl << "frame 0 size: " << frame0_size << std::endl;

    unsigned char frame0[frame0_size];
    fread(&frame0[0], 1, 2, pFile);

    Uint16 offset;
    
    bool from_header;

    // we have a header
    if(frame0[0] == 10)
    {
        fread(&offset, 2, 1, pFile);
        std::cout << offset << std::endl;
    
        fread(&frame0[4], 1, frame0_size-4, pFile);

        from_header = true;
    }
    else
    {
        fread(&frame0[2], 1, frame0_size-2, pFile);
        from_header = false;
    }

    //int offset = frame0[2] + (frame0[1] << 8);
    //std::cout << (int)frame0[2] << " " << (int)frame0[3] << " " << offset <<  std::endl;


    /*unsigned int pixels = 0;


    for(int i = 0; i < frame0_size; i++){
        unsigned int c = frame0[i];
        
        // Regular command
        if((16 < c) && (c <= 127)){
            std::cout << i << ": regular: " << c << std::endl;;
            i+= c-1;
            pixels += c;
        }

        // Transparency command
        else if(128 <= c){
            std::cout << i << ": transparency: " << c << " " << (256 - c) << std::endl;
            pixels += 256 - c;
        }
       
        // Block command 
        else if(c <= 16){
            
            unsigned int x;
            //x = frame0[i+1] << 8;
            //x |=frame0[i+2];
            x = frame0[i+1];
            std::cout << i << ": block: " << c  << " " << x  <<" "<< (unsigned int) frame0[i+2]<< std::endl;;
            if(c == 0){
                //std::cout << "ERROR" << std::endl;
                //return 1;
                continue;
            }
            
            i += c;
        }
        else{
            std::cout << "NOTHING" << std::endl;
        }
    }

    std::cout << "pixels: " << ((float)pixels) / 96.0 << std::endl;
*/

    FILE * pal_file;

    pal_file = fopen(pal_filename.c_str(), "rb");
    
    colour pal[256];

    for(int i = 0; i < 256; i++)
    {
            fread(&pal[i].r, 1, 1, pal_file);
            fread(&pal[i].g, 1, 1, pal_file);
            fread(&pal[i].b, 1, 1, pal_file);
    }

    //std::cout << (int)(pal[255].r) << " " << (int)(pal[255].g) << " " << (int)(pal[255].b) << std::endl;
    
    std::vector<colour> raw_image;

    bool first = true;
    
    //int x = 0, y = 0;

    int width_header, width_reg = 0, width = 99;
    int pixels = 0;

    bool reg_done = false;

    int tmp_pixels;
    
    for(int i = 0; i < frame0_size; i++){

        tmp_pixels = 0;

        int temp = i;

        unsigned int c = frame0[i];
 
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
                raw_image.push_back(pal[frame0[i+j]]);
                /*raw_image.push_back(pal[frame0[i+j]].r);
                raw_image.push_back(pal[frame0[i+j]].g);
                raw_image.push_back(pal[frame0[i+j]].b);*/
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
                raw_image.push_back(colour(255, 255, 255));
            }

            tmp_pixels += run;
        }
       
        // Block command 
        else if(c <= 16){

            std::cout << i << " block: " << c << std::endl;
            
            if(first){
                //std::cout << (int)frame0[i+1] << " " << (int)frame0[i+2] << std::endl;
                first = false;
                i += c;
                continue;
            }

            for(int j = 1; j < c+1; j++){
                raw_image.push_back(pal[frame0[i+j]]);
                /*raw_image.push_back(pal[frame0[i+j]].r);
                raw_image.push_back(pal[frame0[i+j]].g);
                raw_image.push_back(pal[frame0[i+j]].b);*/
            }
            
            tmp_pixels += c;
            i += c;
            
        }

        if(temp == offset)
            width_header = pixels / 31;
        else
            pixels += tmp_pixels;

        std::cout << "\t\tpixels: " << pixels << std::endl;


    }
    
    if(from_header)
        width = width_header;
    else
        width = width_reg;
    
    if(width > 640) width = 99; // should never be bigger than 640 unless we fail to detect width, so fallback to random value 
   
    std::cout << "WIDTH_reg: " << width_reg << std::endl;


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

    //std::reverse(raw_image.begin(), raw_image.end());
/*
    int x = 0, y = 0;
    for(int i = 0; i < raw_image.size(); i += 1)
    {
        //char tmp = raw_image[raw_image.size()-i];
        //raw_image[raw_image.size()-i] = raw_image[i];
        //raw_image[i] = tmp;
                
        //for(int j = 0; j < 3; j++)
        {
            // x,y = width -x, (raw_image.size() / width) -y;
            

            //char tmp = raw_image[(((y*width)+x)*3)+j];
            //raw_image[((y*width)+x)*3] = raw_image[(raw_image.size()-(((y*width)+x)*3))+j];
            //raw_image[(raw_image.size()-(((y*width)+x)*3))+j] = tmp;
        }

        x++;
        if(x >= width){
            x = 0;
            y++;
        }
    }
*/
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
                        case SDLK_LEFT:
                            width--;
                            break;
                        case SDLK_RIGHT:
                            width++;
                            break;
                    } 
                    std::cout << width << std::endl;
            }
        }

        
        if(SDL_MUSTLOCK(screen)) 
        {
            if(SDL_LockSurface(screen) < 0); continue;
        }

        SDL_FillRect(screen,NULL, SDL_MapRGB( screen->format, 0, 0, 0)); 
        
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


        
        

        for(int j = 0; j < 256; j++)
        {
            setpixel(screen, j, 0, pal[j]);
        }

        
        if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
      
        SDL_Flip(screen); 
        //while(1);
    }
}
