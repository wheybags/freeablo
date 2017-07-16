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
#ifndef RENDERINTERFACESDL2_H
#define RENDERINTERFACESDL2_H

#include <misc/disablewarn.h>
    #include <Rocket/Core/RenderInterface.h>
#include <misc/enablewarn.h>

#include <SDL.h>
//#include <SDL_opengl.h>

#include "../sdl_gl_funcs.h"

#include <functional>

#include <render/render.h>

#include "drawcommand.h"

#if !(SDL_VIDEO_RENDER_OGL)
    #error "Only the opengl sdl backend is supported. To add support for others, see http://mdqinc.com/blog/2013/01/integrating-librocket-with-sdl-2/"
#endif

class RocketSDL2Renderer : public Rocket::Core::RenderInterface
{
   
public:
	RocketSDL2Renderer(SDL_Renderer* renderer, SDL_Window* screen, 
        std::function<bool(Rocket::Core::TextureHandle&, Rocket::Core::Vector2i&, const Rocket::Core::String&)> loadTextureFunc,
        std::function<bool(Rocket::Core::TextureHandle&, const Rocket::Core::byte*, const Rocket::Core::Vector2i&)> generateTextureFunc,
        std::function<void(Rocket::Core::TextureHandle)> releaseTextureFunc);

    void drawBuffer(std::vector<DrawCommand>& buffer, Render::SpriteCacheBase* cache);

	/// Called by Rocket when it wants to render geometry that it does not wish to optimise.
	virtual void RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation);

	/// Called by Rocket when it wants to enable or disable scissoring to clip content.
	virtual void EnableScissorRegion(bool enable);
	/// Called by Rocket when it wants to change the scissor region.
	virtual void SetScissorRegion(int x, int y, int width, int height);

	/// Called by Rocket when a texture is required by the library.
	virtual bool LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source);

	/// Called by Rocket when a texture is required to be built from an internally-generated sequence of pixels.
	virtual bool GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions);

	/// Called by Rocket when a loaded texture is no longer required.
	virtual void ReleaseTexture(Rocket::Core::TextureHandle texture_handle);

    
    std::vector<DrawCommand>* mDrawBuffer;

private:
    void RenderGeometryImp(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation, Render::SpriteCacheBase* cache);
	void EnableScissorRegionImp(bool enable);
	void SetScissorRegionImp(int x, int y, int width, int height);

    std::function<bool(Rocket::Core::TextureHandle&, Rocket::Core::Vector2i&, const Rocket::Core::String&)> mLoadTextureFunc;
    std::function<bool(Rocket::Core::TextureHandle&, const Rocket::Core::byte*, const Rocket::Core::Vector2i&)> mGenerateTextureFunc;
    std::function<void(Rocket::Core::TextureHandle)> mReleaseTextureFunc;

    SDL_Renderer* mRenderer;
    SDL_Window* mScreen;
    PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB;


    void getGLFunc()
    {
        #ifdef WIN32
            glUseProgramObjectARB = static_cast<PFNGLUSEPROGRAMOBJECTARBPROC>(SDL_GL_GetProcAddress("glUseProgramObjectARB"));
        #else
            #pragma GCC system_header // this was the only way I could silence the warning :(
            glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)(SDL_GL_GetProcAddress("glUseProgramObjectARB"));
        #endif
    }
};

#endif
