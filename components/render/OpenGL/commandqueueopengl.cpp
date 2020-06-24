#include <glad/glad.h>
#include <render/OpenGL/commandqueueopengl.h>
#include <render/OpenGL/descriptorsetopengl.h>
#include <render/OpenGL/framebufferopengl.h>
#include <render/OpenGL/pipelineopengl.h>
#include <render/OpenGL/textureopengl.h>
#include <render/OpenGL/vertexarrayobjectopengl.h>

namespace Render
{
    CommandQueueOpenGL::DrawScopedBinderGL::DrawScopedBinderGL(Bindings bindings)
    {
        auto pipeline = safe_downcast<PipelineOpenGL*>(bindings.pipeline);
        auto vao = safe_downcast<VertexArrayObjectOpenGL*>(bindings.vao);
        auto descriptorSet = safe_downcast<DescriptorSetOpenGL*>(bindings.descriptorSet);
        auto nonDefaultFramebuffer = safe_downcast<FramebufferOpenGL*>(bindings.nonDefaultFramebuffer);

        mBinders.emplace_back(vao);
        mBinders.emplace_back(pipeline);

        if (nonDefaultFramebuffer)
            mBinders.emplace_back(nonDefaultFramebuffer);

        if (!bindings.descriptorSet)
            return;

        for (uint32_t bindingIndex = 0; bindingIndex < bindings.descriptorSet->size(); bindingIndex++)
        {
            const DescriptorSet::Item& item = descriptorSet->getItem(bindingIndex);

            switch (bindings.descriptorSet->getSpec().items[item.bindingIndex].type)
            {
                case DescriptorType::Texture:
                {
                    auto texture = safe_downcast<TextureOpenGL*>(std::get<Texture*>(item.item));
                    mBinders.emplace_back(texture, GL_TEXTURE0 + pipeline->getUniformLocation(bindingIndex));
                    break;
                }
                case DescriptorType::UniformBuffer:
                {
                    auto bufferSlice = std::get<BufferSlice>(item.item);

                    GLuint uniformBlockIndex = pipeline->getUniformLocation(bindingIndex);
                    glUniformBlockBinding(pipeline->mShaderProgramId, uniformBlockIndex, bindingIndex);

                    auto slice = new BufferSliceOpenGL(bufferSlice);
                    mTempBindables.emplace_back(slice);
                    mBinders.emplace_back(slice, GL_UNIFORM_BUFFER, bindingIndex);
                }
            }
        }
    }

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

        std::unique_ptr<DrawScopedBinderGL> binder = this->setupState(bindings);
        glDrawArrays(GL_TRIANGLES, firstVertex, vertexCount);
    }

    void CommandQueueOpenGL::cmdDrawIndexed(size_t firstIndex, size_t vertexCount, Bindings& bindings)
    {
        super::cmdDrawIndexed(firstIndex, vertexCount, bindings);

        std::unique_ptr<DrawScopedBinderGL> binder = this->setupState(bindings);
        glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid*>(firstIndex));
    }

    void CommandQueueOpenGL::cmdDrawInstances(size_t firstVertex, size_t vertexCount, size_t instanceCount, Bindings& bindings)
    {
        super::cmdDrawInstances(firstVertex, vertexCount, instanceCount, bindings);

        std::unique_ptr<DrawScopedBinderGL> binder = this->setupState(bindings);
        glDrawArraysInstanced(GL_TRIANGLES, firstVertex, vertexCount, instanceCount);
    }

    void CommandQueueOpenGL::cmdClearFramebuffer(std::optional<Color> color, bool clearDepth, Framebuffer* nonDefaultFramebuffer)
    {
        ScopedBindGL framebufferBind;

        if (nonDefaultFramebuffer)
            framebufferBind = ScopedBindGL(safe_downcast<FramebufferOpenGL*>(nonDefaultFramebuffer));

        if (color)
            glClearColor(color->r, color->g, color->b, color->a);

        GLbitfield clearFlags = 0;

        if (clearDepth)
            clearFlags |= GL_DEPTH_BUFFER_BIT;
        if (color)
            clearFlags |= GL_COLOR_BUFFER_BIT;

        glClear(clearFlags);
    }

    void CommandQueueOpenGL::cmdPresent() { SDL_GL_SwapWindow(&getInstance().mWindow); }

    std::unique_ptr<CommandQueueOpenGL::DrawScopedBinderGL> CommandQueueOpenGL::setupState(Bindings& bindings)
    {
        auto binder = std::make_unique<DrawScopedBinderGL>(bindings);

        if (bindings.pipeline->mSpec.scissor)
            glScissor(mScissor.x, mScissor.y, uint32_t(mScissor.w), uint32_t(mScissor.h));

        return binder;
    }
}