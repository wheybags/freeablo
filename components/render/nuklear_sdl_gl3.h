
#ifndef NK_SDL_GL3_H_
#define NK_SDL_GL3_H_

#include <vector>

#include "sdl_gl_funcs.h"

#include <fa_nuklear.h>


struct nk_sdl_device {
    struct nk_buffer cmds;
    struct nk_draw_null_texture null;
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

struct nk_sdl_vertex {
    float position[2];
    float uv[2];
    nk_byte col[4];
};

struct nk_sdl {
    SDL_Window *win;
    struct nk_sdl_device ogl;
    struct nk_context ctx;
    struct nk_font_atlas atlas;
};

extern struct nk_sdl sdl;


class NuklearFrameDump
{
    public:
        NuklearFrameDump(const nk_draw_null_texture& nullTex);
    
        void fill(nk_context* ctx, nk_buffer* cmds);

        nk_buffer vbuf; // vertices
        nk_buffer ebuf; // indices

        std::vector<nk_draw_command> drawCommands;

    private:
        nk_convert_config config;
};


NK_API struct nk_context*   nk_sdl_init(SDL_Window *win);
NK_API void                 nk_sdl_font_stash_begin(struct nk_font_atlas **atlas);
NK_API void                 nk_sdl_font_stash_end(void);
NK_API int                  nk_sdl_handle_event(SDL_Event *evt);
NK_API void                 nk_sdl_render_dump(const NuklearFrameDump& dump);
NK_API void                 nk_sdl_shutdown(void);
NK_API void                 nk_sdl_device_destroy(void);
NK_API void                 nk_sdl_device_create(void);

#endif