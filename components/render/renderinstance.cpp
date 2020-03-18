#include "renderinstance.h"
#include <misc/misc.h>
#include <render/OpenGL/renderinstanceopengl.h>

namespace Render
{
    Render::RenderInstance* RenderInstance::createRenderInstance(Type type, SDL_Window& window)
    {
        switch (type)
        {
            case Type::OpenGL:
                return new RenderInstanceOpenGL(window);
        }

        invalid_enum(Type, type);
    }
}
