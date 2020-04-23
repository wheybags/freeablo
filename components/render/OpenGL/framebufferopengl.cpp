#include <misc/misc.h>
#include <render/OpenGL/framebufferopengl.h>
#include <render/OpenGL/textureopengl.h>

namespace Render
{
    FramebufferOpenGL::FramebufferOpenGL(const FramebufferInfo& info) : Framebuffer(info)
    {
        glGenFramebuffers(1, &mId);

        ScopedBindGL thisBind(this);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, safe_downcast<TextureOpenGL*>(mInfo.colorBuffer)->mId, 0);

        if (mInfo.depthStencilBuffer)
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, safe_downcast<TextureOpenGL*>(mInfo.depthStencilBuffer)->mId, 0);

        release_assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    }

    FramebufferOpenGL::~FramebufferOpenGL() { glDeleteFramebuffers(1, &mId); }

    void FramebufferOpenGL::bind(std::optional<GLuint>, std::optional<GLuint>) { glBindFramebuffer(GL_FRAMEBUFFER, mId); }

    void FramebufferOpenGL::unbind(std::optional<GLuint>, std::optional<GLuint>) { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

}
