#pragma once
#include <glad/glad.h>
#include <render/OpenGL/scopedbindgl.h>
#include <render/texture.h>

namespace Render
{
    class RenderInstanceOpenGL;

    class TextureOpenGL final : public Texture, public BindableGL
    {
        using super = Texture;

    public:
        TextureOpenGL(RenderInstanceOpenGL& instance, const BaseTextureInfo& info);
        ~TextureOpenGL() override;

        void bind(std::optional<GLenum> binding) override;
        void unbind(std::optional<GLenum> binding) override;

    private:
        GLenum getBindPoint() const;

    private:
        GLuint mId = 0;

        friend class CommandQueueOpenGL;
    };
}
