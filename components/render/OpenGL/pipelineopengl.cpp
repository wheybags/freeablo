#include <iostream>
#include <misc/stringops.h>
#include <render/OpenGL/pipelineopengl.h>
#include <render/OpenGL/renderinstanceopengl.h>

namespace Render
{
    PipelineOpenGL::PipelineOpenGL(RenderInstanceOpenGL& instance, PipelineSpec spec) : super(instance, std::move(spec))
    {
        auto compileShader = [](GLenum type, const filesystem::path& path) {
            std::string src = Misc::StringUtils::readAsString(path.str());
            const GLchar* srcPtr = src.c_str();

            GLuint shaderId = glCreateShader(type);
            glShaderSource(shaderId, 1, &srcPtr, nullptr);
            glCompileShader(shaderId);

            GLint isCompiled = 0;
            glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

                // The maxLength includes the NUL character
                std::vector<GLchar> errorLog(maxLength);
                glGetShaderInfoLog(shaderId, maxLength, &maxLength, errorLog.data());

                message_and_abort_fmt("Shader compile error in (%s): %s\n", path.str().c_str(), errorLog.data());
            }

            return shaderId;
        };

        mVertexShaderId = compileShader(GL_VERTEX_SHADER, mSpec.vertexShaderPath);
        mFragmentShaderId = compileShader(GL_FRAGMENT_SHADER, mSpec.fragmentShaderPath);

        mShaderProgramId = glCreateProgram();
        glAttachShader(mShaderProgramId, mVertexShaderId);
        glAttachShader(mShaderProgramId, mFragmentShaderId);
        glLinkProgram(mShaderProgramId);

        GLint status;
        glGetProgramiv(mShaderProgramId, GL_LINK_STATUS, &status);
        if (status != GL_TRUE)
        {
            GLint maxLength = 0;
            glGetProgramiv(mShaderProgramId, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NUL character
            std::vector<GLchar> errorLog(maxLength);
            glGetProgramInfoLog(mShaderProgramId, maxLength, &maxLength, errorLog.data());

            message_and_abort_fmt(
                "Shader link error(%s + %s): %s\n", mSpec.vertexShaderPath.str().c_str(), mSpec.fragmentShaderPath.str().c_str(), errorLog.data());
        }

        uint32_t textureIndex = 0;

        mUniformLocations.resize(mSpec.descriptorSetSpec.items.size());
        for (uint32_t bindingIndex = 0; bindingIndex < uint32_t(mSpec.descriptorSetSpec.items.size()); bindingIndex++)
        {
            const DescriptorSetSpec::Item& item = mSpec.descriptorSetSpec.items[bindingIndex];

            switch (item.type)
            {
                case DescriptorType::UniformBuffer:
                    mUniformLocations[bindingIndex] = glGetUniformBlockIndex(mShaderProgramId, item.glName.c_str());
                    break;
                case DescriptorType::Texture:
                {
                    ScopedBindGL thisBind(this);

                    GLint location = glGetUniformLocation(mShaderProgramId, item.glName.c_str());
                    glUniform1i(location, textureIndex);
                    mUniformLocations[bindingIndex] = textureIndex;
                    textureIndex++;
                    break;
                }
            }
        }
    }

    PipelineOpenGL::~PipelineOpenGL()
    {
        glDetachShader(mShaderProgramId, mVertexShaderId);
        glDetachShader(mShaderProgramId, mFragmentShaderId);
        glDeleteShader(mVertexShaderId);
        glDeleteShader(mFragmentShaderId);
        glDeleteProgram(mShaderProgramId);
    }

    void PipelineOpenGL::bind(std::optional<GLuint>, std::optional<GLuint>)
    {
        if (mSpec.scissor)
            glEnable(GL_SCISSOR_TEST);
        if (mSpec.depthTest)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
        }

        glUseProgram(mShaderProgramId);
    }
    void PipelineOpenGL::unbind(std::optional<GLuint>, std::optional<GLuint>)
    {
        glDisable(GL_SCISSOR_TEST);
        glDisable(GL_DEPTH_TEST);
        glUseProgram(0);
    }

    GLuint PipelineOpenGL::getUniformLocation(uint32_t bindingIndex) const
    {
        debug_assert(mUniformLocations[bindingIndex] != std::numeric_limits<GLuint>::max());
        return mUniformLocations[bindingIndex];
    }
}
