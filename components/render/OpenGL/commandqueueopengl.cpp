#include <glad/glad.h>
#include <render/OpenGL/commandqueueopengl.h>
#include <render/OpenGL/vertexarrayobjectopengl.h>

namespace Render
{
    CommandQueueOpenGL::CommandQueueOpenGL(RenderInstanceOpenGL& instance) : super(instance) {}

    void CommandQueueOpenGL::cmdDraw(size_t firstVertex, size_t vertexCount, Bindings& bindings)
    {
        super::cmdDraw(firstVertex, vertexCount, bindings);

        ScopedBindGL vaoBind(static_cast<VertexArrayObjectOpenGL*>(bindings.vao));
        glDrawArrays(GL_TRIANGLES, firstVertex, vertexCount);
    }

    void CommandQueueOpenGL::cmdDrawIndexed(size_t firstIndex, size_t vertexCount, Bindings& bindings)
    {
        super::cmdDrawIndexed(firstIndex, vertexCount, bindings);

        ScopedBindGL vaoBind(static_cast<VertexArrayObjectOpenGL*>(bindings.vao));
        glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid*>(firstIndex));
    }

    void CommandQueueOpenGL::cmdDrawInstances(size_t firstVertex, size_t vertexCount, size_t instanceCount, Bindings& bindings)
    {
        super::cmdDrawInstances(firstVertex, vertexCount, instanceCount, bindings);

        ScopedBindGL vaoBind(static_cast<VertexArrayObjectOpenGL*>(bindings.vao));
        glDrawArraysInstanced(GL_TRIANGLES, firstVertex, vertexCount, instanceCount);
    }

    void CommandQueueOpenGL::cmdPresent() { SDL_GL_SwapWindow(&getInstance().mWindow); }
}