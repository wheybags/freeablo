#pragma once
#include <glad/glad.h>
#include <render/OpenGL/scopedbindgl.h>
#include <render/pipeline.h>
#include <unordered_map>

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
        void bind(std::optional<GLuint> extra1, std::optional<GLuint> extra2) override;
        void unbind(std::optional<GLuint> extra1, std::optional<GLuint> extra2) override;

        GLuint getUniformLocation(uint32_t bindingIndex) const;

    private:
        GLuint mVertexShaderId = 0;
        GLuint mFragmentShaderId = 0;
        std::vector<GLuint> mUniformLocations;

    public:
        GLuint mShaderProgramId = 0;
    };
}