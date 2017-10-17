#include "nuklear_sdl_gl3.h"
#include "sdl_gl_funcs.h"

#include <string.h>
#include <assert.h>
#include "render.h"
#include "../../apps/freeablo/fagui/guimanager.h"
#include <iostream>

struct nk_sdl_vertex
{
    float position[2];
    float uv[2];
    nk_byte col[4];
};

static const struct nk_draw_vertex_layout_element vertex_layout[] =
{
    { NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_sdl_vertex, position) },
    { NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_sdl_vertex, uv) },
    { NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(struct nk_sdl_vertex, col) },
    { NK_VERTEX_LAYOUT_END }
};


#ifdef __APPLE__
  #define NK_SHADER_VERSION "#version 150\n"
#else
  #define NK_SHADER_VERSION "#version 300 es\n"
#endif

void nk_sdl_device_create(nk_gl_device& dev)
{
    GLint status;
    static const GLchar *vertex_shader =
        NK_SHADER_VERSION
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 TexCoord;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main() {\n"
        "   Frag_UV = TexCoord;\n"
        "   Frag_Color = Color;\n"
        "   gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
        "}\n";
    static const GLchar *fragment_shader =
        NK_SHADER_VERSION
       R"(precision mediump float;
        uniform sampler2D Texture;
        in vec2 Frag_UV;
        in vec4 Frag_Color;
        out vec4 Out_Color;
        uniform float h_color_r;
        uniform float h_color_g;
        uniform float h_color_b;
        uniform float h_color_a;
        uniform float imgW;
        uniform float imgH;
        uniform int checkerboarded;
        void main(){
             vec4 c = Frag_Color * texture(Texture, Frag_UV.st);
             if (c.w == 0. && h_color_a > 0.)
                {
                  for (float i= -1.; i <= 1.; i++)
                    for (float j= -1.; j <= 1.; j++)
                        {
                          vec4 n = texture(Texture, vec2 (Frag_UV.st.x + i/imgW, Frag_UV.st.y + j/imgH));
                          if (n.w > 0. && (n.x > 0. || n.y > 0. || n.z > 0.))
                            c = vec4 (h_color_r, h_color_g, h_color_b, h_color_a);
                        }
                }
           if (checkerboarded != 0)
           {
             float vx = floor (Frag_UV.st.x * imgW);
             float vy = floor (Frag_UV.st.y * imgH);
             if (mod (vx + vy, 2.) == 1.)
               c.w = 0.;
           }
           Out_Color = c;
        }
       )";

    nk_buffer_init_default(&dev.cmds);
    dev.prog = glCreateProgram();
    dev.vert_shdr = glCreateShader(GL_VERTEX_SHADER);
    dev.frag_shdr = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(dev.vert_shdr, 1, &vertex_shader, 0);
    glShaderSource(dev.frag_shdr, 1, &fragment_shader, 0);
    glCompileShader(dev.vert_shdr);
    glCompileShader(dev.frag_shdr);
    glGetShaderiv(dev.vert_shdr, GL_COMPILE_STATUS, &status);
    assert(status == GL_TRUE);
    glGetShaderiv(dev.frag_shdr, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        int length = 1024;
        std::vector<GLchar> errorLog(length);
        glGetShaderInfoLog(dev.frag_shdr, length, &length, &errorLog[0]);
        std::cout << &errorLog[0] << std::endl;
    }
    assert(status == GL_TRUE);
    glAttachShader(dev.prog, dev.vert_shdr);
    glAttachShader(dev.prog, dev.frag_shdr);
    glLinkProgram(dev.prog);
    glGetProgramiv(dev.prog, GL_LINK_STATUS, &status);
    assert(status == GL_TRUE);

    dev.uniform_hcolor_r = glGetUniformLocation(dev.prog, "h_color_r");
    dev.uniform_hcolor_g = glGetUniformLocation(dev.prog, "h_color_g");
    dev.uniform_hcolor_b = glGetUniformLocation(dev.prog, "h_color_b");
    dev.uniform_hcolor_a = glGetUniformLocation(dev.prog, "h_color_a");
    dev.uniform_checkerboarded = glGetUniformLocation(dev.prog, "checkerboarded");
    dev.imgW = glGetUniformLocation(dev.prog, "imgW");
    dev.imgH = glGetUniformLocation(dev.prog, "imgH");
    dev.uniform_tex = glGetUniformLocation(dev.prog, "Texture");
    dev.uniform_proj = glGetUniformLocation(dev.prog, "ProjMtx");
    dev.attrib_pos = glGetAttribLocation(dev.prog, "Position");
    dev.attrib_uv = glGetAttribLocation(dev.prog, "TexCoord");
    dev.attrib_col = glGetAttribLocation(dev.prog, "Color");

    {
        // buffer setup
        GLsizei vs = sizeof(struct nk_sdl_vertex);
        size_t vp = offsetof(struct nk_sdl_vertex, position);
        size_t vt = offsetof(struct nk_sdl_vertex, uv);
        size_t vc = offsetof(struct nk_sdl_vertex, col);

        glGenBuffers(1, &dev.vbo);
        glGenBuffers(1, &dev.ebo);
        glGenVertexArrays(1, &dev.vao);

        glBindVertexArray(dev.vao);
        glBindBuffer(GL_ARRAY_BUFFER, dev.vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dev.ebo);

        glEnableVertexAttribArray((GLuint)dev.attrib_pos);
        glEnableVertexAttribArray((GLuint)dev.attrib_uv);
        glEnableVertexAttribArray((GLuint)dev.attrib_col);

        glVertexAttribPointer((GLuint)dev.attrib_pos, 2, GL_FLOAT, GL_FALSE, vs, (void*)vp);
        glVertexAttribPointer((GLuint)dev.attrib_uv, 2, GL_FLOAT, GL_FALSE, vs, (void*)vt);
        glVertexAttribPointer((GLuint)dev.attrib_col, 4, GL_UNSIGNED_BYTE, GL_TRUE, vs, (void*)vc);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

/*GLuint nk_sdl_device_upload_atlas(const void *image, int width, int height)
{
    GLuint font_tex;
    glGenTextures(1, &font_tex);
    glBindTexture(GL_TEXTURE_2D, font_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    return font_tex;
}*/

void nk_sdl_device_destroy(nk_gl_device& dev)
{
    glDetachShader(dev.prog, dev.vert_shdr);
    glDetachShader(dev.prog, dev.frag_shdr);
    glDeleteShader(dev.vert_shdr);
    glDeleteShader(dev.frag_shdr);
    glDeleteProgram(dev.prog);
    //glDeleteTextures(1, &dev.font_tex);
    glDeleteBuffers(1, &dev.vbo);
    glDeleteBuffers(1, &dev.ebo);
    nk_buffer_free(&dev.cmds);
}

void nk_sdl_render_dump(Render::SpriteCacheBase* cache, NuklearFrameDump& dump, SDL_Window* win)
{
    int width, height;
    int display_width, display_height;
    struct nk_vec2 scale;
    GLfloat ortho[4][4] = {
        { 2.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f,-2.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f,-1.0f, 0.0f },
        { -1.0f,1.0f, 0.0f, 1.0f },
    };
    SDL_GetWindowSize(win, &width, &height);
    SDL_GL_GetDrawableSize(win, &display_width, &display_height);
    ortho[0][0] /= (GLfloat)width;
    ortho[1][1] /= (GLfloat)height;

    scale.x = (float)display_width / (float)width;
    scale.y = (float)display_height / (float)height;

    // setup global state
    glViewport(0, 0, display_width, display_height);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);

    nk_gl_device& dev = dump.getDevice();

    // setup program
    glUseProgram(dev.prog);
    glUniform1i(dev.uniform_tex, 0);

    glUniformMatrix4fv(dev.uniform_proj, 1, GL_FALSE, &ortho[0][0]);
    {
        // convert from command queue into draw list and draw to screen
        void *vertices, *elements;
        const nk_draw_index *offset = NULL;

        // allocate vertex and element buffer
        glBindVertexArray(dev.vao);
        glBindBuffer(GL_ARRAY_BUFFER, dev.vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dev.ebo);

        glBufferData(GL_ARRAY_BUFFER, dump.vbuf.size, NULL, GL_STREAM_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, dump.ebuf.size, NULL, GL_STREAM_DRAW);

        // load vertices/elements directly into vertex/element buffer
        vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        elements = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
        {
            memcpy(vertices, dump.vbuf.memory.ptr, dump.vbuf.size);
            memcpy(elements, dump.ebuf.memory.ptr, dump.ebuf.size);
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

        // iterate over and execute each draw command
        for (size_t i = 0; i < dump.drawCommands.size(); i++)
        {
            const struct nk_draw_command& cmd = dump.drawCommands[i];
            if (!cmd.elem_count) continue;

            uint32_t cacheIndex = ((uint32_t*)cmd.texture.ptr)[0];
            uint32_t frameNum = ((uint32_t*)cmd.texture.ptr)[1];
            auto effect = static_cast<FAGui::EffectType> (cmd.userdata.id);

            Render::SpriteGroup* sprite = cache->get(cacheIndex);
            auto s = sprite->operator[](frameNum);
            glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)s);
            int32_t w, h;
            Render::spriteSize (s, w, h);
            int item_hl_color[] = {0xB9, 0xAA, 0x77};
            glUniform1f(dev.uniform_hcolor_r, item_hl_color[0] / 255.f);
            glUniform1f(dev.uniform_hcolor_g, item_hl_color[1] / 255.f);
            glUniform1f(dev.uniform_hcolor_b, item_hl_color[2] / 255.f);
            glUniform1f(dev.uniform_hcolor_a, effect == FAGui::EffectType::highlighted ? 1.0f : 0.0f);
            glUniform1i(dev.uniform_checkerboarded, effect == FAGui::EffectType::checkerboarded ? 1 : 0);
            glUniform1f(dev.imgW, w);
            glUniform1f(dev.imgH, h);

            glScissor((GLint)(cmd.clip_rect.x * scale.x),
                (GLint)((height - (GLint)(cmd.clip_rect.y + cmd.clip_rect.h)) * scale.y),
                (GLint)(cmd.clip_rect.w * scale.x),
                (GLint)(cmd.clip_rect.h * scale.y));
            glDrawElements(GL_TRIANGLES, (GLsizei)cmd.elem_count, GL_UNSIGNED_SHORT, offset);
            offset += cmd.elem_count;
        }
    }

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
}

/*
static void
nk_sdl_clipbard_paste(nk_handle usr, struct nk_text_edit *edit)
{
    const char *text = SDL_GetClipboardText();
    if (text) nk_textedit_paste(edit, text, nk_strlen(text));
    (void)usr;
}

static void
nk_sdl_clipbard_copy(nk_handle usr, const char *text, int len)
{
    char *str = 0;
    (void)usr;
    if (!len) return;
    str = (char*)malloc((size_t)len+1);
    if (!str) return;
    memcpy(str, text, (size_t)len);
    str[len] = '\0';
    SDL_SetClipboardText(str);
    free(str);
}
*/


/*void nk_sdl_init(nk_sdl& nkSdl, SDL_Window *win)
{
    sdl = nkSdl;

    nkSdl.win = win;
    nk_init_default(&nkSdl.ctx, 0);
    sdl.ctx.clip.copy = nullptr;// nk_sdl_clipbard_copy;
    sdl.ctx.clip.paste = nullptr;// nk_sdl_clipbard_paste;
    sdl.ctx.clip.userdata = nk_handle_ptr(0);
    nk_sdl_device_create();
}*/

/*void nk_sdl_font_stash_begin(nk_font_atlas& atlas)
{
    nk_font_atlas_init_default(&atlas);
    nk_font_atlas_begin(&atlas);
}

GLuint nk_sdl_font_stash_end(nk_context* ctx, nk_font_atlas& atlas, nk_draw_null_texture& nullTex)
{
    const void *image; int w, h;
    image = nk_font_atlas_bake(&atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
    GLuint font_tex = nk_sdl_device_upload_atlas(image, w, h);
    nk_font_atlas_end(&atlas, nk_handle_id((int)font_tex), &nullTex);
    if (atlas.default_font)
        nk_style_set_font(ctx, &atlas.default_font->handle);

    return font_tex;
}*/

#if 0
NK_API int
nk_sdl_handle_event(SDL_Event *evt)
{
    struct nk_context *ctx = &sdl.ctx;
    if (evt->type == SDL_KEYUP || evt->type == SDL_KEYDOWN) {
        /* key events */
        int down = evt->type == SDL_KEYDOWN;
        const Uint8* state = SDL_GetKeyboardState(0);
        SDL_Keycode sym = evt->key.keysym.sym;
        if (sym == SDLK_RSHIFT || sym == SDLK_LSHIFT)
            nk_input_key(ctx, NK_KEY_SHIFT, down);
        else if (sym == SDLK_DELETE)
            nk_input_key(ctx, NK_KEY_DEL, down);
        else if (sym == SDLK_RETURN)
            nk_input_key(ctx, NK_KEY_ENTER, down);
        else if (sym == SDLK_TAB)
            nk_input_key(ctx, NK_KEY_TAB, down);
        else if (sym == SDLK_BACKSPACE)
            nk_input_key(ctx, NK_KEY_BACKSPACE, down);
        else if (sym == SDLK_HOME) {
            nk_input_key(ctx, NK_KEY_TEXT_START, down);
            nk_input_key(ctx, NK_KEY_SCROLL_START, down);
        } else if (sym == SDLK_END) {
            nk_input_key(ctx, NK_KEY_TEXT_END, down);
            nk_input_key(ctx, NK_KEY_SCROLL_END, down);
        } else if (sym == SDLK_PAGEDOWN) {
            nk_input_key(ctx, NK_KEY_SCROLL_DOWN, down);
        } else if (sym == SDLK_PAGEUP) {
            nk_input_key(ctx, NK_KEY_SCROLL_UP, down);
        } else if (sym == SDLK_z)
            nk_input_key(ctx, NK_KEY_TEXT_UNDO, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_r)
            nk_input_key(ctx, NK_KEY_TEXT_REDO, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_c)
            nk_input_key(ctx, NK_KEY_COPY, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_v)
            nk_input_key(ctx, NK_KEY_PASTE, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_x)
            nk_input_key(ctx, NK_KEY_CUT, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_b)
            nk_input_key(ctx, NK_KEY_TEXT_LINE_START, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_e)
            nk_input_key(ctx, NK_KEY_TEXT_LINE_END, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_UP)
            nk_input_key(ctx, NK_KEY_UP, down);
        else if (sym == SDLK_DOWN)
            nk_input_key(ctx, NK_KEY_DOWN, down);
        else if (sym == SDLK_LEFT) {
            if (state[SDL_SCANCODE_LCTRL])
                nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, down);
            else nk_input_key(ctx, NK_KEY_LEFT, down);
        } else if (sym == SDLK_RIGHT) {
            if (state[SDL_SCANCODE_LCTRL])
                nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, down);
            else nk_input_key(ctx, NK_KEY_RIGHT, down);
        } else return 0;
        return 1;
    } else if (evt->type == SDL_MOUSEBUTTONDOWN || evt->type == SDL_MOUSEBUTTONUP) {
        /* mouse button */
        int down = evt->type == SDL_MOUSEBUTTONDOWN;
        const int x = evt->button.x, y = evt->button.y;
        if (evt->button.button == SDL_BUTTON_LEFT) {
            if (evt->button.clicks > 1)
                nk_input_button(ctx, NK_BUTTON_DOUBLE, x, y, down);
            nk_input_button(ctx, NK_BUTTON_LEFT, x, y, down);
        } else if (evt->button.button == SDL_BUTTON_MIDDLE)
            nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y, down);
        else if (evt->button.button == SDL_BUTTON_RIGHT)
            nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, down);
        return 1;
    } else if (evt->type == SDL_MOUSEMOTION) {
        /* mouse motion */
        if (ctx->input.mouse.grabbed) {
            int x = (int)ctx->input.mouse.prev.x, y = (int)ctx->input.mouse.prev.y;
            nk_input_motion(ctx, x + evt->motion.xrel, y + evt->motion.yrel);
        } else nk_input_motion(ctx, evt->motion.x, evt->motion.y);
        return 1;
    } else if (evt->type == SDL_TEXTINPUT) {
        /* text input */
        nk_glyph glyph;
        memcpy(glyph, evt->text.text, NK_UTF_SIZE);
        nk_input_glyph(ctx, glyph);
        return 1;
    } else if (evt->type == SDL_MOUSEWHEEL) {
        /* mouse wheel */
        nk_input_scroll(ctx,nk_vec2((float)evt->wheel.x,(float)evt->wheel.y));
        return 1;
    }
    return 0;
}
#endif

/*void nk_sdl_shutdown(void)
{
    nk_font_atlas_clear(&sdl.atlas);
    nk_free(&sdl.ctx);
    nk_sdl_device_destroy();
    memset(&sdl, 0, sizeof(sdl));
}*/

NuklearFrameDump::NuklearFrameDump(nk_gl_device& dev)
{
    init(dev);
}

void NuklearFrameDump::init(nk_gl_device& dev)
{
    this->dev = &dev;
    nk_buffer_init_default(&vbuf);
    nk_buffer_init_default(&ebuf);
    nk_buffer_init_default(&cmds);

    memset(&config, 0, sizeof(config));
    config.vertex_layout = vertex_layout;
    config.vertex_size = sizeof(struct nk_sdl_vertex);
    config.vertex_alignment = NK_ALIGNOF(struct nk_sdl_vertex);
    config.null = dev.null;
    config.circle_segment_count = 22;
    config.curve_segment_count = 22;
    config.arc_segment_count = 22;
    config.global_alpha = 1.0f;
    config.shape_AA = nk_anti_aliasing::NK_ANTI_ALIASING_ON;
    config.line_AA = nk_anti_aliasing::NK_ANTI_ALIASING_ON;
}

NuklearFrameDump::~NuklearFrameDump()
{
    assert(dev);
    nk_buffer_free(&vbuf);
    nk_buffer_free(&ebuf);
    nk_buffer_free(&cmds);
}

void NuklearFrameDump::fill(nk_context* ctx)
{
    assert(dev);
    nk_convert(ctx, &cmds, &vbuf, &ebuf, &config);

    drawCommands.clear();

    const nk_draw_command *cmd;
    nk_draw_foreach(cmd, ctx, &cmds)
    {
        drawCommands.push_back(*cmd);
    }
}
