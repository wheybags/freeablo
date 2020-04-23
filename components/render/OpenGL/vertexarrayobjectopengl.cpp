#include "vertexarrayobjectopengl.h"

namespace Render
{
    VertexArrayObjectOpenGL::VertexArrayObjectOpenGL(RenderInstanceOpenGL& renderInstance,
                                                     std::vector<size_t> bufferSizeCounts,
                                                     std::vector<NonNullConstPtr<VertexLayout>> bindings,
                                                     size_t indexBufferSizeInElements)
        : super(bindings)
    {
        debug_assert(bufferSizeCounts.size() == bindings.size());

        glGenVertexArrays(1, &mVaoId);
        ScopedBindGL binder(this);

        GLint locationIndex = 0;
        for (size_t bindingIndex = 0; bindingIndex < mBindings.size(); bindingIndex++)
        {
            std::unique_ptr<BufferOpenGL> buffer(safe_downcast<BufferOpenGL*>(renderInstance.createBuffer(bufferSizeCounts[bindingIndex]).release()));
            locationIndex = VertexArrayObjectOpenGL::setupAttributes(locationIndex, *buffer, *bindings[bindingIndex]);
            mBuffers.emplace_back(buffer.release());
        }

        if (indexBufferSizeInElements > 0)
        {
            mIndexBuffer.reset(safe_downcast<BufferOpenGL*>(renderInstance.createBuffer(indexBufferSizeInElements).release()));
            mIndexBuffer->bind(GL_ELEMENT_ARRAY_BUFFER, std::nullopt); // This binding point is not global, it's stored in the vao, so we don't ever unbind
        }
    }

    VertexArrayObjectOpenGL::~VertexArrayObjectOpenGL() { glDeleteVertexArrays(1, &mVaoId); }

    void VertexArrayObjectOpenGL::bind(std::optional<GLuint>, std::optional<GLuint>) { glBindVertexArray(mVaoId); }

    void VertexArrayObjectOpenGL::unbind(std::optional<GLuint>, std::optional<GLuint>) { glBindVertexArray(0); }

    GLint VertexArrayObjectOpenGL::setupAttributes(GLint locationIndex, BufferOpenGL& buffer, const VertexLayout& layout)
    {
        ScopedBindGL bufferBind(buffer, GL_ARRAY_BUFFER);

        size_t offset = 0;
        for (Format element : layout.getElements())
        {
            GLenum type = 0;
            GLint size = 0;
            GLboolean normalized = GL_FALSE;

            switch (element)
            {
                case Format::RGBA8UNorm:
                    type = GL_UNSIGNED_BYTE;
                    normalized = GL_TRUE;
                    size = 4;
                    break;
                case Format::RGBA32F:
                    type = GL_FLOAT;
                    normalized = GL_FALSE;
                    size = 4;
                    break;
                case Format::RGB32F:
                    type = GL_FLOAT;
                    normalized = GL_FALSE;
                    size = 3;
                    break;
                case Format::RG32F:
                    type = GL_FLOAT;
                    normalized = GL_FALSE;
                    size = 2;
                    break;
                case Format::R32F:
                    type = GL_FLOAT;
                    normalized = GL_FALSE;
                    size = 1;
                    break;
                case Format::RGBA16U:
                    type = GL_UNSIGNED_SHORT;
                    normalized = GL_FALSE;
                    size = 4;
                    break;
                case Format::RGB16U:
                    type = GL_UNSIGNED_SHORT;
                    normalized = GL_FALSE;
                    size = 3;
                    break;
                case Format::RG16U:
                    type = GL_UNSIGNED_SHORT;
                    normalized = GL_FALSE;
                    size = 2;
                    break;
                case Format::RGBA16I:
                    type = GL_SHORT;
                    normalized = GL_FALSE;
                    size = 4;
                    break;
                case Format::RGB16I:
                    type = GL_SHORT;
                    normalized = GL_FALSE;
                    size = 3;
                    break;
                case Format::RG16I:
                    type = GL_SHORT;
                    normalized = GL_FALSE;
                    size = 2;
                    break;
                case Format::Depth24Stencil8:
                    invalid_enum(Format, element);
            }

            glVertexAttribPointer(locationIndex, size, type, normalized, layout.getSizeInBytes(), reinterpret_cast<void*>(offset));
            glVertexAttribDivisor(locationIndex, layout.getRate() == VertexInputRate::ByVertex ? 0 : 1);
            glEnableVertexAttribArray(locationIndex);

            offset += formatSize(element);
            locationIndex++;
        }

        return locationIndex;
    }
}
