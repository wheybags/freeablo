
#ifndef NK_SDL_GL3_H_
#define NK_SDL_GL3_H_

#include <vector>

#include "sdl_gl_funcs.h"

#include <fa_nuklear.h>


struct nk_gl_device 
{
    nk_buffer cmds;
    nk_draw_null_texture null;
    GLuint vbo, vao, ebo;
    GLuint prog;
    GLuint vert_shdr;
    GLuint frag_shdr;
    GLint attrib_pos;
    GLint attrib_uv;
    GLint attrib_col;
    GLint uniform_tex;
    GLint uniform_proj;
    GLuint font_tex;
};



class NuklearFrameDump
{
    public:
        NuklearFrameDump() = delete;
        NuklearFrameDump(const NuklearFrameDump&)  = delete;

        NuklearFrameDump(nk_gl_device& dev);
        ~NuklearFrameDump();
    
        void fill(nk_context* ctx);

        nk_buffer vbuf; // vertices
        nk_buffer ebuf; // indices

        nk_gl_device& dev;

        std::vector<nk_draw_command> drawCommands;

    private:
        nk_convert_config config;
        nk_buffer cmds; // draw commands temp storage
};

//void nk_sdl_init(nk_sdl& nkSdl, SDL_Window *win);
void nk_sdl_font_stash_begin(nk_font_atlas& atlas);
GLuint nk_sdl_font_stash_end(nk_context* ctx, nk_font_atlas& atlas, nk_draw_null_texture& nullTex);
//NK_API int                  nk_sdl_handle_event(SDL_Event *evt);
void nk_sdl_render_dump(const NuklearFrameDump& dump, SDL_Window* win);
//NK_API void                 nk_sdl_shutdown(void);
void nk_sdl_device_destroy(nk_gl_device& dev);
void nk_sdl_device_create(nk_gl_device& dev);

#endif