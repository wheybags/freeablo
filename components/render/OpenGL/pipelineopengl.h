#pragma once
#include <glad/glad.h>
#include <render/OpenGL/scopedbindgl.h>
#include <render/pipeline.h>

namespace Render
{
    class RenderInstanceOpenGL;

    class PipelineOpenGL final : public Pipeline, public BindableGL
    {
        using super = Pipeline;

    public:
        PipelineOpenGL(RenderInstanceOpenGL& instance, PipelineSpec spec);
        ~PipelineOpenGL() override;

    public:
        void bind(std::optional<GLenum> binding) override;
        void unbind(std::optional<GLenum> binding) override;

    private:
        GLuint mVertexShaderId = 0;
        GLuint mFragmentShaderId = 0;

    public:
        GLuint mShaderProgramId = 0;
    };
}