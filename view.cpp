#include <stdio.h>
#include <stdint.h>

#include <SDL.h>
#include <iostream>
#include <vector>
#include <algorithm>


#include "celfile.h"
#include "cel_frame.h"




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
    
int main(int argc, char** argv){
    std::string pal_filename;
    size_t frame_num;
    if(argc >= 3)
        pal_filename = argv[2];
    else
        pal_filename = "diablo.pal";
    
    if(argc >= 4)
        frame_num = atoi(argv[3]);
    /*
    //size_t frame_num = 210;
    
    colour pal[256];
    get_pal(pal_filename, pal); 


    FILE * cel_file;
    cel_file = fopen(argv[1], "rb");

    size_t num_frames = get_num_frames(cel_file);
    
    uint32_t frame_offsets[num_frames+1];
    get_frame_offsets(cel_file, frame_offsets, num_frames);



    std::string file_name = argv[1];
    bool tile_cel = is_tile_cel(file_name);
*/

    std::vector<colour> raw_image;
    //size_t width = get_frame(cel_file, pal, frame_offsets, frame_num, raw_image, tile_cel);

    //std::cout << width << std::endl; 

    size_t width; 

    Cel_file cel(argv[1]);
    Cel_frame frame = cel[frame_num];



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
                            if(frame_num == cel.num_frames()-1) break;
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
                    //width = cel.get_frame(frame_num, raw_image);
                    //width = get_frame(cel_file, pal, frame_offsets, frame_num, raw_image, tile_cel);
                    frame = cel[frame_num];
                    std::cout << "frame: " << frame_num << "/" << cel.num_frames() << std::endl;
                    std::cout << width << std::endl;
            }
        }

        
        if(SDL_MUSTLOCK(screen)) 
        {
            if(SDL_LockSurface(screen) < 0); continue;
        }

        SDL_FillRect(screen,NULL, SDL_MapRGB( screen->format, 0, 0, 255)); 
        
        /*int x = 0, y = 0;
        for(int i = 0; i < raw_image.size(); i++)
        {

            setpixel(screen, x, y, raw_image[i]);
            
            x++;
            if(x >= width){
                x = 0;
                y++;
            }
        }*/
        
        for(int x = 0; x < frame.width; x++)
        {
            for(int y = 0; y < frame.height; y++)
            {
                //if(frame[x][y].visible)
                    setpixel(screen, x, y, frame[x][y]);
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
