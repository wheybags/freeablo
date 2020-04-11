#include "nuklear_sdl_gl3.h"
#include "../../apps/freeablo/fagui/guimanager.h"
#include <cstring>
#include <misc/assert.h>
#include <misc/misc.h>
#include <render/OpenGL/pipelineopengl.h>
#include <render/OpenGL/textureopengl.h>
#include <render/commandqueue.h>
#include <render/renderinstance.h>
#include <render/texture.h>
#include <render/vertexarrayobject.h>
#include <render/vertextypes.h>

static const struct nk_draw_vertex_layout_element vertex_layout[] = {{NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct Render::NuklearVertex, position)},
                                                                     {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct Render::NuklearVertex, uv)},
                                                                     {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(struct Render::NuklearVertex, color)},
                                                                     {NK_VERTEX_LAYOUT_END}};

void nk_sdl_device_create(nk_gl_device& dev, Render::RenderInstance& renderInstance)
{
    nk_buffer_init_default(&dev.cmds);

    Render::PipelineSpec pipelineSpec;
    pipelineSpec.vertexLayouts = {Render::NuklearVertex::layout()};
    pipelineSpec.vertexShaderPath = Misc::getResourcesPath().str() + "/shaders/gui.vert";
    pipelineSpec.fragmentShaderPath = Misc::getResourcesPath().str() + "/shaders/gui.frag";
    pipelineSpec.descriptorSetSpec = {{
        {Render::DescriptorType::UniformBuffer, "vertexUniforms"},
        {Render::DescriptorType::UniformBuffer, "fragmentUniforms"},
        {Render::DescriptorType::Texture, "Texture"},
    }};
    pipelineSpec.scissor = true;

    dev.pipeline = renderInstance.createPipeline(pipelineSpec).release();
    dev.vertexArrayObject = renderInstance.createVertexArrayObject({0}, pipelineSpec.vertexLayouts, 1).release();
    dev.descriptorSet = renderInstance.createDescriptorSet(pipelineSpec.descriptorSetSpec).release();

    dev.uniformCpuBuffer = new GuiUniforms::CpuBufferType(renderInstance.getUniformBufferOffsetAlignment());
    dev.uniformBuffer = renderInstance.createBuffer(dev.uniformCpuBuffer->getSizeInBytes()).release();
}

void nk_sdl_device_destroy(nk_gl_device& dev)
{
    delete dev.descriptorSet;
    delete dev.uniformBuffer;
    delete dev.pipeline;
    delete dev.vertexArrayObject;
    delete dev.uniformCpuBuffer;
    nk_buffer_free(&dev.cmds);
}

void nk_sdl_render_dump(NuklearFrameDump& dump, SDL_Window* win, Render::AtlasTexture& atlasTexture, Render::CommandQueue& commandQueue)
{
    int width, height;
    int display_width, display_height;
    struct nk_vec2 scale;
    float ortho[4][4] = {
        {2.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, -2.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, -1.0f, 0.0f},
        {-1.0f, 1.0f, 0.0f, 1.0f},
    };
    SDL_GetWindowSize(win, &width, &height);

    // Note: if SDL_GL_GetDrawableSize() is used SDL_WINDOW_ALLOW_HIGHDPI option must
    // also be enabled. However this seems to cause an issue where black lines appear
    // between each tile on high-DPI displays so is currently disabled.
    // SDL_GL_GetDrawableSize(win, &display_width, &display_height);
    display_width = width;
    display_height = height;

    ortho[0][0] /= (GLfloat)width;
    ortho[1][1] /= (GLfloat)height;

    scale.x = (float)display_width / (float)width;
    scale.y = (float)display_height / (float)height;

    nk_gl_device& dev = dump.getDevice();

    const Render::AtlasTextureLookupMap& atlasLookupMap = atlasTexture.getLookupMap();

    // convert from command queue into draw list and draw to screen
    size_t offset = 0;

    dev.vertexArrayObject->getVertexBuffer(0)->setData(dump.vbuf.memory.ptr, dump.vbuf.size);
    dev.vertexArrayObject->getIndexBuffer()->setData(dump.ebuf.memory.ptr, dump.ebuf.size);

    dev.descriptorSet->updateItems({
        {0, Render::BufferSlice{dev.uniformBuffer, dev.uniformCpuBuffer->getMemberOffset<GuiUniforms::Vertex>(), sizeof(GuiUniforms::Vertex)}},
        {1, Render::BufferSlice{dev.uniformBuffer, dev.uniformCpuBuffer->getMemberOffset<GuiUniforms::Fragment>(), sizeof(GuiUniforms::Fragment)}},
    });

    // iterate over and execute each draw command
    for (const auto& cmd : dump.drawCommands)
    {
        if (!cmd.elem_count)
            continue;

        auto* nuklearTextureHandle = reinterpret_cast<FANuklearTextureHandle*>(cmd.texture.ptr);

        const Render::AtlasTextureEntry* atlasEntry = nullptr;
        if (nuklearTextureHandle->texture == nullptr)
        {
            uint32_t frameNum = nuklearTextureHandle->frameNumber;

            Render::SpriteGroup* spriteGroup = nuklearTextureHandle->spriteGroup;
            Render::Sprite sprite = spriteGroup->operator[](frameNum);
            atlasEntry = &atlasLookupMap.at((GLuint)(intptr_t)sprite);
        }

        auto effect = static_cast<FAGui::EffectType>(cmd.userdata.id);

        int item_hl_color[] = {0xB9, 0xAA, 0x77};

        GuiUniforms::Vertex* vertex = dev.uniformCpuBuffer->getMemberPointer<GuiUniforms::Vertex>();
        memcpy(vertex->ProjMtx, ortho, sizeof(ortho));

        GuiUniforms::Fragment* fragment = dev.uniformCpuBuffer->getMemberPointer<GuiUniforms::Fragment>();
        fragment->hoverColor[0] = float(item_hl_color[0]) / 255.0f;
        fragment->hoverColor[1] = float(item_hl_color[1]) / 255.0f;
        fragment->hoverColor[2] = float(item_hl_color[2]) / 255.0f;
        fragment->hoverColor[3] = effect == FAGui::EffectType::highlighted ? 1.0f : 0.0f;
        fragment->checkerboarded = effect == FAGui::EffectType::checkerboarded ? 1.0f : 0.0f;

        if (atlasEntry)
        {
            dev.descriptorSet->updateItems({
                {2, atlasEntry->mTexture},
            });

            fragment->imageSize[0] = atlasEntry->mWidth;
            fragment->imageSize[1] = atlasEntry->mHeight;
            fragment->atlasSize[0] = atlasEntry->mTexture->width();
            fragment->atlasSize[1] = atlasEntry->mTexture->height();
            fragment->atlasOffset[0] = atlasEntry->mX;
            fragment->atlasOffset[1] = atlasEntry->mY;
            fragment->atlasOffset[2] = atlasEntry->mLayer;
        }
        else
        {
            dev.descriptorSet->updateItems({
                {2, nuklearTextureHandle->texture},
            });

            fragment->imageSize[0] = nuklearTextureHandle->texture->width();
            fragment->imageSize[1] = nuklearTextureHandle->texture->height();
            fragment->atlasSize[0] = nuklearTextureHandle->texture->width();
            fragment->atlasSize[1] = nuklearTextureHandle->texture->height();
            fragment->atlasOffset[0] = 0;
            fragment->atlasOffset[1] = 0;
            fragment->atlasOffset[2] = 0;
        }

        dev.uniformBuffer->setData(dev.uniformCpuBuffer->data(), dev.uniformCpuBuffer->getSizeInBytes());

        Render::ScissorRect scissor = {};
        scissor.x = int32_t(cmd.clip_rect.x * scale.x);
        scissor.y = int32_t((float(height) - (cmd.clip_rect.y + cmd.clip_rect.h)) * scale.y);
        scissor.w = int32_t(cmd.clip_rect.w * scale.x);
        scissor.h = int32_t(cmd.clip_rect.h * scale.y);

        commandQueue.cmdScissor(scissor);

        Render::Bindings bindings;
        bindings.vao = dev.vertexArrayObject;
        bindings.pipeline = dev.pipeline;
        bindings.descriptorSet = dev.descriptorSet;

        commandQueue.cmdDrawIndexed(size_t(offset), cmd.elem_count, bindings);

        // Useful if we ever need to debug the generated vertices:

        // std::vector<nk_draw_index> indices;
        // indices.resize(cmd.elem_count);
        // memcpy(indices.data(), ((char*)dump.ebuf.memory.ptr) + offset, cmd.elem_count);
        //
        // auto* allVertices = (Render::NuklearVertex*)dump.vbuf.memory.ptr;
        // std::vector<Render::NuklearVertex> vertices;
        // vertices.resize(indices.size());
        // for (int32_t i = 0; i < int32_t(vertices.size()); i++)
        //     vertices[i] = allVertices[indices[i]];

        offset += cmd.elem_count * sizeof(nk_draw_index);
    }
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

NuklearFrameDump::NuklearFrameDump(nk_gl_device& dev) { init(dev); }

void NuklearFrameDump::init(nk_gl_device& dev)
{
    this->dev = &dev;
    nk_buffer_init_default(&vbuf);
    nk_buffer_init_default(&ebuf);
    nk_buffer_init_default(&cmds);

    memset(&config, 0, sizeof(config));
    config.vertex_layout = vertex_layout;
    config.vertex_size = sizeof(Render::NuklearVertex);
    config.vertex_alignment = NK_ALIGNOF(Render::NuklearVertex);
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
    debug_assert(dev);
    nk_buffer_free(&vbuf);
    nk_buffer_free(&ebuf);
    nk_buffer_free(&cmds);
}

void NuklearFrameDump::fill(nk_context* ctx)
{
    debug_assert(dev);

    nk_buffer_clear(&vbuf);
    nk_buffer_clear(&ebuf);
    nk_buffer_clear(&cmds);
    nk_convert(ctx, &cmds, &vbuf, &ebuf, &config);

    drawCommands.clear();

    const nk_draw_command* cmd;
    nk_draw_foreach(cmd, ctx, &cmds) { drawCommands.push_back(*cmd); }
}

nk_gl_device& NuklearFrameDump::getDevice()
{
    debug_assert(dev);
    return *dev;
}
