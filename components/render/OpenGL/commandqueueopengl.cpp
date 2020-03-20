#include <glad/glad.h>
#include <render/OpenGL/commandqueueopengl.h>
#include <render/OpenGL/descriptorsetopengl.h>
#include <render/OpenGL/pipelineopengl.h>
#include <render/OpenGL/textureopengl.h>
#include <render/OpenGL/vertexarrayobjectopengl.h>

namespace Render
{
    CommandQueueOpenGL::CommandQueueOpenGL(RenderInstanceOpenGL& instance) : super(instance) {}

    void CommandQueueOpenGL::cmdClearTexture(Texture& _texture, const Color& color)
    {
        TextureOpenGL& texture = safe_downcast<TextureOpenGL&>(_texture);

        ScopedBindGL textureBind(texture);

        GLuint fbo = 0;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

        glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.mId, 0);
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);
    }

    void CommandQueueOpenGL::cmdDraw(size_t firstVertex, size_t vertexCount, Bindings& bindings)
    {
        super::cmdDraw(firstVertex, vertexCount, bindings);

        ScopedBindGL vaoBind(safe_downcast<VertexArrayObjectOpenGL*>(bindings.vao));
        ScopedBindGL pipelineBind(safe_downcast<PipelineOpenGL*>(bindings.pipeline));
        glDrawArrays(GL_TRIANGLES, firstVertex, vertexCount);
    }

    void CommandQueueOpenGL::cmdDrawIndexed(size_t firstIndex, size_t vertexCount, Bindings& bindings)
    {
        super::cmdDrawIndexed(firstIndex, vertexCount, bindings);

        ScopedBindGL vaoBind(safe_downcast<VertexArrayObjectOpenGL*>(bindings.vao));
        ScopedBindGL pipelineBind(safe_downcast<PipelineOpenGL*>(bindings.pipeline));
        glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid*>(firstIndex));
    }

    void CommandQueueOpenGL::cmdDrawInstances(size_t firstVertex, size_t vertexCount, size_t instanceCount, Bindings& bindings)
    {
        super::cmdDrawInstances(firstVertex, vertexCount, instanceCount, bindings);

        auto pipeline = safe_downcast<PipelineOpenGL*>(bindings.pipeline);
        auto vao = safe_downcast<VertexArrayObjectOpenGL*>(bindings.vao);

        ScopedBindGL vaoBind(vao);
        ScopedBindGL pipelineBind(pipeline);

        if (bindings.descriptorSet)
        {
            debug_assert(pipeline->mSpec.descriptorSetSpec == bindings.descriptorSet->getSpec());

            for (uint32_t bindingIndex = 0; bindingIndex < bindings.descriptorSet->size(); bindingIndex++)
            {
                const DescriptorSet::Item& item = safe_downcast<DescriptorSetOpenGL*>(bindings.descriptorSet)->getItem(bindingIndex);

                switch (bindings.descriptorSet->getSpec().items[item.bindingIndex].type)
                {
                    case DescriptorType::Texture:
                    {
                        auto texture = safe_downcast<TextureOpenGL*>(std::get<Texture*>(item.item));
                        glActiveTexture(GL_TEXTURE0 + pipeline->getUniformLocation(bindingIndex));
                        texture->bind(std::nullopt);
                        break;
                    }
                    case DescriptorType::UniformBuffer:
                    {
                        auto bufferSlice = std::get<BufferSlice>(item.item);
                        auto buffer = safe_downcast<BufferOpenGL*>(bufferSlice.buffer);

                        GLuint uniformBlockIndex = pipeline->getUniformLocation(bindingIndex);
                        glUniformBlockBinding(pipeline->mShaderProgramId, uniformBlockIndex, bindingIndex);
                        glBindBufferRange(GL_UNIFORM_BUFFER, bindingIndex, buffer->getId(), bufferSlice.offset, bufferSlice.length);
                    }
                }
            }
        }

        glDrawArraysInstanced(GL_TRIANGLES, firstVertex, vertexCount, instanceCount);
    }

    void CommandQueueOpenGL::cmdPresent() { SDL_GL_SwapWindow(&getInstance().mWindow); }
}