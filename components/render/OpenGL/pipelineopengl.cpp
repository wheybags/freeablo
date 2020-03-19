#include <iostream>
#include <misc/stringops.h>
#include <render/OpenGL/pipelineopengl.h>
#include <render/OpenGL/renderinstanceopengl.h>

namespace Render
{
    PipelineOpenGL::PipelineOpenGL(RenderInstanceOpenGL& instance, const PipelineSpec& spec) : super(instance, spec)
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
                glGetShaderInfoLog(shaderId, maxLength, &maxLength, &errorLog[0]);

                message_and_abort(errorLog.data());
            }

            return shaderId;
        };

        mVertexShaderId = compileShader(GL_VERTEX_SHADER, spec.vertexShaderPath);
        mFragmentShaderId = compileShader(GL_FRAGMENT_SHADER, spec.fragmentShaderPath);

        mShaderProgramId = glCreateProgram();
        glAttachShader(mShaderProgramId, mVertexShaderId);
        glAttachShader(mShaderProgramId, mFragmentShaderId);
        glLinkProgram(mShaderProgramId);

        GLint status;
        glGetProgramiv(mShaderProgramId, GL_LINK_STATUS, &status);
        release_assert(status == GL_TRUE);
    }

    PipelineOpenGL::~PipelineOpenGL()
    {
        glDetachShader(mShaderProgramId, mVertexShaderId);
        glDetachShader(mShaderProgramId, mFragmentShaderId);
        glDeleteShader(mVertexShaderId);
        glDeleteShader(mFragmentShaderId);
        glDeleteProgram(mShaderProgramId);

    }

    void PipelineOpenGL::bind(std::optional<GLenum>) { glUseProgram(mShaderProgramId); }
    void PipelineOpenGL::unbind(std::optional<GLenum>) { glUseProgram(0); }
}
