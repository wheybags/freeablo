#include <glad/glad.h>
#include <render/OpenGL/commandqueueopengl.h>
#include <render/OpenGL/descriptorsetopengl.h>
#include <render/OpenGL/framebufferopengl.h>
#include <render/OpenGL/pipelineopengl.h>
#include <render/OpenGL/renderinstanceopengl.h>
#include <render/OpenGL/textureopengl.h>
#include <render/OpenGL/vertexarrayobjectopengl.h>

namespace Render
{
#ifdef DEBUG_GRAPHICS
    void gladDebugPostCallCallback(const char* name, void*, int, ...)
    {
        GLenum error_code;
        error_code = glad_glGetError();

        if (error_code != GL_NO_ERROR)
        {
            fprintf(stderr, "ERROR %d in %s\n", error_code, name);
            DEBUG_BREAK;
        }
    }
#endif

    RenderInstanceOpenGL::RenderInstanceOpenGL(SDL_Window& window) : super(window)
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        mGlContext = SDL_GL_CreateContext(&mWindow);

        if (!gladLoadGL())
            message_and_abort("gladLoadGL failed");

#ifdef DEBUG_GRAPHICS
        glad_set_post_callback(gladDebugPostCallCallback);

        // Ensure VSYNC is disabled to get actual FPS.
        SDL_GL_SetSwapInterval(0);
#endif

        const GLubyte* glVersion(glGetString(GL_VERSION));
        int major = glVersion[0] - '0';
        int minor = glVersion[2] - '0';
        if (major < 3 || (major == 3 && minor < 3))
            message_and_abort_fmt("ERROR: Minimum OpenGL version is 3.3. Your current version is %d.%d\n", major, minor);

        // setup render capabilities struct
        {
            GLint uniformBufferOffsetAlignment = 0;
            glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferOffsetAlignment);

            GLint maxTextureSize = 0;
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

            mRenderCapabilities = {maxTextureSize, uniformBufferOffsetAlignment};
        }

        setupGlobalState();
    }

    void RenderInstanceOpenGL::setupGlobalState()
    {
        glDisable(GL_CULL_FACE);

        // For now, we will just force all pixel transfers to be tightly packed
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Use normal alpha blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    RenderInstanceOpenGL::~RenderInstanceOpenGL() { SDL_GL_DeleteContext(mGlContext); }

    std::unique_ptr<DescriptorSet> RenderInstanceOpenGL::createDescriptorSet(DescriptorSetSpec spec)
    {
        return std::unique_ptr<DescriptorSet>(new DescriptorSetOpenGL(std::move(spec)));
    }

    std::unique_ptr<Pipeline> RenderInstanceOpenGL::createPipeline(const PipelineSpec& spec)
    {
        return std::unique_ptr<Pipeline>(new PipelineOpenGL(*this, spec));
    }

    std::unique_ptr<Texture> RenderInstanceOpenGL::createTexture(const BaseTextureInfo& info)
    {
        return std::unique_ptr<Texture>(new TextureOpenGL(*this, info));
    }

    std::unique_ptr<Framebuffer> RenderInstanceOpenGL::createFramebuffer(const FramebufferInfo& info)
    {
        return std::unique_ptr<Framebuffer>(new FramebufferOpenGL(info));
    }

    std::unique_ptr<Buffer> RenderInstanceOpenGL::createBuffer(size_t sizeInBytes) { return std::unique_ptr<Buffer>(new BufferOpenGL(sizeInBytes)); }

    std::unique_ptr<VertexArrayObject> RenderInstanceOpenGL::createVertexArrayObject(std::vector<size_t> bufferSizeCounts,
                                                                                     std::vector<NonNullConstPtr<VertexLayout>> bindings,
                                                                                     size_t indexBufferSizeInElements)
    {
        return std::unique_ptr<VertexArrayObject>(
            new VertexArrayObjectOpenGL(*this, std::move(bufferSizeCounts), std::move(bindings), indexBufferSizeInElements));
    }

    std::unique_ptr<CommandQueue> RenderInstanceOpenGL::createCommandQueue() { return std::unique_ptr<CommandQueue>(new CommandQueueOpenGL(*this)); }

    void RenderInstanceOpenGL::onWindowResized(int32_t width, int32_t height) { glViewport(0, 0, width, height); }
}
