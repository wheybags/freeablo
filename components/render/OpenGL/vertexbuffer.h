#pragma once
#include <cstddef>
#include <cstdint>
#include <glad/glad.h>
#include <memory>
#include <misc/assert.h>
#include <misc/misc.h>
#include <render/renderinstance.h>
#include <render/vertexlayout.h>

namespace Render
{
    struct SpriteVertexMain
    {
        float vertex_position[3];
        float v_uv[2];

        static const VertexLayout& layout()
        {
            static VertexLayout layout{{
                                           Format::RGB32F,
                                           Format::RG32F,
                                       },
                                       VertexInputRate::ByVertex};

            debug_assert(layout.getSizeInBytes() == sizeof(SpriteVertexMain));

            return layout;
        }
    };

    struct SpriteVertexPerInstance
    {
        uint16_t v_imageSize[2];
        int16_t v_imageOffset[2];
        uint8_t v_hoverColor[4];
        uint16_t v_atlasOffset[3];

        static const VertexLayout& layout()
        {
            static VertexLayout layout{{
                                           Format::RG16U,
                                           Format::RG16I,
                                           Format::RGBA8UNorm,
                                           Format::RGB16U,
                                       },
                                       VertexInputRate::ByInstance};

            debug_assert(layout.getSizeInBytes() == sizeof(SpriteVertexPerInstance));

            return layout;
        }
    };

    struct NuklearVertex
    {
        float position[2];
        float uv[2];
        uint8_t color[4];

        static const VertexLayout& layout()
        {
            static VertexLayout layout{{
                                           Format::RG32F,
                                           Format::RG32F,
                                           Format::RGBA8UNorm,
                                       },
                                       VertexInputRate::ByVertex};

            debug_assert(layout.getSizeInBytes() == sizeof(NuklearVertex));

            return layout;
        }
    };

    class BindableGL
    {
    public:
        virtual void bind() = 0;
        virtual void unbind() = 0;
    };

    class ScopedBindGL
    {
    public:
        ScopedBindGL(BindableGL& resource) : mResource(resource) { mResource.bind(); }

        ScopedBindGL(BindableGL* resource) : ScopedBindGL(*resource) {}

        ~ScopedBindGL() { mResource.unbind(); }

    private:
        BindableGL& mResource;
    };

    class BufferOpenGL final : public Buffer
    {
        using super = Buffer;

    public:
        BufferOpenGL(size_t sizeInBytes);
        ~BufferOpenGL();

        GLuint getId() { return mId; }

        virtual void setData(void* data, size_t dataSizeInBytes) override;

    private:
        GLuint mId = 0;
    };

    class VertexArrayObjectOpenGL final : public BindableGL, public VertexArrayObject
    {
        using super = VertexArrayObject;

    public:
        VertexArrayObjectOpenGL(std::vector<size_t> bufferSizeCounts, std::vector<NonNullConstPtr<VertexLayout>> bindings, size_t indexBufferSizeInElements);
        virtual ~VertexArrayObjectOpenGL();

        virtual Buffer* getVertexBuffer(size_t index) override { return mBuffers[index].get(); }
        virtual Buffer* getIndexBuffer() override { return mIndexBuffer.get(); }

        virtual void bind() override;
        virtual void unbind() override;

    private:
        static GLint setupAttributes(GLint locationIndex, BufferOpenGL& buffer, const VertexLayout& layout);

    private:
        std::vector<std::unique_ptr<BufferOpenGL>> mBuffers;
        std::unique_ptr<BufferOpenGL> mIndexBuffer;
        GLuint mVaoId;
    };
}
