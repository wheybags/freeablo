/*
 * This source file is part of libRocket, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://www.librocket.com
 *
 * Copyright (c) 2008-2010 Nuno Silva
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <misc/disablewarn.h>
    #include <Rocket/Core/Core.h>
#include <misc/enablewarn.h>

#include <SDL_image.h>

#include <misc/stringops.h>
#include <cel/celfile.h>
#include <cel/celframe.h>

#include "RenderInterfaceSDL2.h"

#include <sstream>

#if !(SDL_VIDEO_RENDER_OGL)
    #error "Only the opengl sdl backend is supported. To add support for others, see http://mdqinc.com/blog/2013/01/integrating-librocket-with-sdl-2/"
#endif

RocketSDL2Renderer::RocketSDL2Renderer(SDL_Renderer* renderer, SDL_Window* screen, 
        std::function<bool(Rocket::Core::TextureHandle&, Rocket::Core::Vector2i&, const Rocket::Core::String&)> loadTextureFunc,
        std::function<bool(Rocket::Core::TextureHandle&, const Rocket::Core::byte*, const Rocket::Core::Vector2i&)> generateTextureFunc,
        std::function<void(Rocket::Core::TextureHandle)> releaseTextureFunc) :
            mLoadTextureFunc(loadTextureFunc), mGenerateTextureFunc(generateTextureFunc), mReleaseTextureFunc(releaseTextureFunc)
{
    mRenderer = renderer;
    mScreen = screen;

    getGLFunc();
    
    ROCKET_ASSERT(glUseProgramObjectARB != NULL);
}

void RocketSDL2Renderer::RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, const Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation)
{
    if(mDrawBuffer)
    {
        DrawCommand tmp;

        for(int i = 0; i < num_vertices; i++)
            tmp.draw.vertices.push_back(vertices[i]);

        for(int i = 0; i < num_indices; i++)
            tmp.draw.indices.push_back(indices[i]);

        tmp.draw.texture = texture;
        tmp.draw.translation = translation;

        tmp.mode = DrawCommand::Draw;

        mDrawBuffer->push_back(tmp);
    }
}

void RocketSDL2Renderer::drawBuffer(std::vector<DrawCommand>& buffer, Render::SpriteCacheBase* cache)
{
    for(size_t i = 0; i < buffer.size(); i++)
    {
        switch(buffer[i].mode)
        {
            case DrawCommand::Draw:
                RenderGeometryImp(&(buffer[i].draw.vertices[0]), buffer[i].draw.vertices.size(), &(buffer[i].draw.indices[0]), buffer[i].draw.indices.size(), buffer[i].draw.texture, buffer[i].draw.translation, cache);
                break;
            case DrawCommand::EnableScissor:
                EnableScissorRegionImp(buffer[i].enableScissor);
                break;
            case DrawCommand::SetScissor:
	            SetScissorRegionImp(buffer[i].setScissor.x, buffer[i].setScissor.y, buffer[i].setScissor.width, buffer[i].setScissor.height);
                break;
        }
    }
}


// Called by Rocket when it wants to render geometry that it does not wish to optimise.
void RocketSDL2Renderer::RenderGeometryImp(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, const Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation, Render::SpriteCacheBase* cache)
{
    // SDL uses shaders that we need to disable here  
    glUseProgramObjectARB(0);
    glPushMatrix();
    glTranslatef(translation.x, translation.y, 0);
 
    std::vector<Rocket::Core::Vector2f> Positions(num_vertices);
    std::vector<Rocket::Core::Colourb> Colors(num_vertices);
    std::vector<Rocket::Core::Vector2f> TexCoords(num_vertices);
    float texw, texh;
 
    SDL_Texture* sdl_texture = NULL;
    if(texture)
    {
        Render::RocketFATex* rtex = (Render::RocketFATex*) texture;

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        sdl_texture = (SDL_Texture *) cache->get(rtex->spriteIndex)->operator[](rtex->index);
        SDL_GL_BindTexture(sdl_texture, &texw, &texh);
    }
 
    for(int  i = 0; i < num_vertices; i++) {
        Positions[i] = vertices[i].position;
        Colors[i] = vertices[i].colour;
        if (sdl_texture) {
            TexCoords[i].x = vertices[i].tex_coord.x * texw;
            TexCoords[i].y = vertices[i].tex_coord.y * texh;
        }
        else TexCoords[i] = vertices[i].tex_coord;
    };
 
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, &Positions[0]);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, &Colors[0]);
    glTexCoordPointer(2, GL_FLOAT, 0, &TexCoords[0]);
 
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, indices);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
 
    if (sdl_texture) {
        SDL_GL_UnbindTexture(sdl_texture);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
 
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    /* Reset blending and draw a fake point just outside the screen to let SDL know that it needs to reset its state in case it wants to render a texture */
    glDisable(GL_BLEND);
    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_NONE);
    SDL_RenderDrawPoint(mRenderer, -1, -1);
}


// Called by Rocket when it wants to enable or disable scissoring to clip content.		
void RocketSDL2Renderer::EnableScissorRegion(bool enable)
{
    if(mDrawBuffer)
    {
        DrawCommand tmp;
        tmp.mode = DrawCommand::EnableScissor;
        tmp.enableScissor = enable;

        mDrawBuffer->push_back(tmp);
    }
}

void RocketSDL2Renderer::EnableScissorRegionImp(bool enable)
{
    if (enable)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);
}

// Called by Rocket when it wants to change the scissor region.		
void RocketSDL2Renderer::SetScissorRegion(int x, int y, int width, int height)
{
    if(mDrawBuffer)
    {
        DrawCommand tmp;
        tmp.mode = DrawCommand::SetScissor;

        tmp.setScissor.x = x;
        tmp.setScissor.y = y;
        tmp.setScissor.width = width;
        tmp.setScissor.height = height;

        mDrawBuffer->push_back(tmp);
    }
}

void RocketSDL2Renderer::SetScissorRegionImp(int x, int y, int width, int height)
{
    int w_width, w_height;
    SDL_GetWindowSize(mScreen, &w_width, &w_height);
    glScissor(x, w_height - (y + height), width, height);
}

bool RocketSDL2Renderer::LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source)
{
    return mLoadTextureFunc(texture_handle, texture_dimensions, source);
}

bool RocketSDL2Renderer::GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions)
{
    return mGenerateTextureFunc(texture_handle, source, source_dimensions);
}

void RocketSDL2Renderer::ReleaseTexture(Rocket::Core::TextureHandle texture_handle)
{
    mReleaseTextureFunc(texture_handle);
}
