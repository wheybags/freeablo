
#include <stdio.h>
#include <stdint.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <iostream>
#include <vector>
#include <algorithm>


#include "celfile.h"


#define WIDTH 1280
#define HEIGHT 960
#define BPP 4
#define DEPTH 32




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

void draw_at(SDL_Surface* screen, int start_x, int y, const std::vector<colour>& raw_image, size_t width)
{
        int x = 0;
        for(int i = 0; i < raw_image.size(); i++)
        {
            
            if(raw_image[i].visible)
                setpixel(screen, start_x+x, y, raw_image[i]);
            
            x++;
            if(x >= width){
                x = 0;
                y++;
            }
        }
}

void draw_tile(int lindex, int rindex, int x, int y)
{
    //draw_at(screen, 10, 10, raw_image, width);
    //draw_at(screen, 42, 11, frame2, width);
    //draw_at(screen, 42, 26, raw_image, width);
}




int main(int argc, char** argv)
{

    FILE * f;

    f = fopen(argv[1], "rb");
    

    uint16_t width;
    fread(&width, 2, 1, f);
    uint16_t height;
    fread(&height, 2, 1, f);
    
    std::cerr << width*height << std::endl;
    uint16_t tiles[width*height];
    fread(&tiles[0], 2, width*height, f);

    for(size_t i = 0; i < width*height; i++)
    {
        std::cout << i << " " << tiles[i] << std::endl;
    }
    
    fclose(f);

    /*colour pal[256];
    get_pal("../../diabdat.mpq/levels/towndata/town.pal", pal);
    
    FILE * cel_file;
    cel_file = fopen("../../diabdat.mpq/levels/towndata/town.cel", "rb");
    size_t num_frames = get_num_frames(cel_file);
    uint32_t frame_offsets[num_frames+1];
    get_frame_offsets(cel_file, frame_offsets, num_frames);
    */
    
    std::vector<colour> raw_image;
    //bool tile_cel = true;
    //size_t frame_num = 0;


    
    
    
    
    SDL_Surface *screen;
    SDL_Event event;
  
    int h=0; 
  
    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) return 1;
   
    if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE)))
    {
        SDL_Quit();
        return 1;
    }
    
    width = 32;

    std::vector<colour> frame2;
    Cel_file town("../../diabdat.mpq/levels/towndata/town.cel");

    town.get_frame(124, raw_image); 
    
    town.get_frame(125, frame2); 

    SDL_FillRect(screen,NULL, SDL_MapRGB( screen->format, 0, 0, 255)); 
    draw_at(screen, 10, 10, raw_image, width);
    draw_at(screen, 42, 11, frame2, width);
    draw_at(screen, 42, 26, raw_image, width);

    
    bool quit = false;
    while(!quit)
    {
        while(SDL_PollEvent(&event)){
            switch (event.type) 
            {
                case SDL_QUIT:
	                quit = true;
	                break;

            }
           //width = get_frame(cel_file, pal, frame_offsets, frame_num, raw_image, tile_cel);
        }

        
        if(SDL_MUSTLOCK(screen)) 
        {
            if(SDL_LockSurface(screen) < 0); continue;
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
