#pragma once
#include <glad/glad.h>
#include <render/OpenGL/scopedbindgl.h>
#include <render/framebuffer.h>

namespace Render
{
    class FramebufferOpenGL final : public Framebuffer, public BindableGL
    {
    public:
        FramebufferOpenGL(const FramebufferInfo& info);
        virtual ~FramebufferOpenGL();

        void bind(std::optional<GLuint> extra1, std::optional<GLuint> extra2) override;
        void unbind(std::optional<GLuint> extra1, std::optional<GLuint> extra2) override;

    private:
        GLuint mId = 0;
    };
}
