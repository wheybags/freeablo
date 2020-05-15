#include <render/color.h>
#include <render/commandqueue.h>
#include <render/debugrenderer.h>
#include <render/pipeline.h>
#include <render/render.h>
#include <render/renderinstance.h>
#include <render/vertexarrayobject.h>
#include <render/vertextypes.h>

#define _USE_MATH_DEFINES
#include "math.h"

namespace Render
{
    DebugRenderer::DebugRenderer(RenderInstance& renderInstance) : mInstance(renderInstance)
    {
        Render::PipelineSpec debugPipelineSpec;
        debugPipelineSpec.vertexLayouts = {Render::DebugVertex::layout()};
        debugPipelineSpec.vertexShaderPath = Misc::getResourcesPath().str() + "/shaders/debug.vert";
        debugPipelineSpec.fragmentShaderPath = Misc::getResourcesPath().str() + "/shaders/debug.frag";
        debugPipelineSpec.descriptorSetSpec = {};

        mPipeline = mInstance.createPipeline(debugPipelineSpec);
        mVao = mInstance.createVertexArrayObject({0}, debugPipelineSpec.vertexLayouts, 0);
    }

    DebugRenderer::~DebugRenderer() = default;

    void
    DebugRenderer::drawRectangle(CommandQueue& commandQueue, Framebuffer* nonDefaultFramebuffer, const Color& color, int32_t x, int32_t y, int32_t w, int32_t h)
    {
        float screenW = float(WIDTH);
        float screenH = float(HEIGHT);

        // clang-format off
        DebugVertex topLeft =     {{x     / screenW, y     / screenH}, {color.r, color.g, color.b, color.a}};
        DebugVertex topRight =    {{(x+w) / screenW, y     / screenH}, {color.r, color.g, color.b, color.a}};
        DebugVertex bottomLeft =  {{x     / screenW, (y+h) / screenH}, {color.r, color.g, color.b, color.a}};
        DebugVertex bottomRight = {{(x+w) / screenW, (y+h) / screenH}, {color.r, color.g, color.b, color.a}};

        DebugVertex vertices[]
        {
            topLeft, topRight, bottomLeft,
            topRight, bottomRight, bottomLeft
        };
        // clang-format on

        mVao->getVertexBuffer(0)->setData(vertices, sizeof(vertices));

        Bindings bindings;
        bindings.pipeline = mPipeline.get();
        bindings.vao = mVao.get();
        bindings.nonDefaultFramebuffer = nonDefaultFramebuffer;

        commandQueue.cmdDraw(0, sizeof(vertices) / sizeof(*vertices), bindings);
    }

    void DebugRenderer::drawQuad(CommandQueue& commandQueue, Framebuffer* nonDefaultFramebuffer, const Color& color, Vec2f* quad)
    {
        float screenW = float(WIDTH);
        float screenH = float(HEIGHT);

        // clang-format off
        DebugVertex a = {{quad[0].x / screenW, quad[0].y / screenH}, {color.r, color.g, color.b, color.a}};
        DebugVertex b = {{quad[1].x / screenW, quad[1].y / screenH}, {color.r, color.g, color.b, color.a}};
        DebugVertex c = {{quad[2].x / screenW, quad[2].y / screenH}, {color.r, color.g, color.b, color.a}};
        DebugVertex d = {{quad[3].x / screenW, quad[3].y / screenH}, {color.r, color.g, color.b, color.a}};

        DebugVertex vertices[]
        {
            a, b, c,
            c, d, a
        };
        // clang-format on

        mVao->getVertexBuffer(0)->setData(vertices, sizeof(vertices));

        Bindings bindings;
        bindings.pipeline = mPipeline.get();
        bindings.vao = mVao.get();
        bindings.nonDefaultFramebuffer = nonDefaultFramebuffer;

        commandQueue.cmdDraw(0, sizeof(vertices) / sizeof(*vertices), bindings);
    }

    void DebugRenderer::drawLine(CommandQueue& commandQueue, Framebuffer* nonDefaultFramebuffer, const Color& color, Vec2f a, Vec2f b, float thickness)
    {
        float screenW = float(WIDTH);
        float screenH = float(HEIGHT);

        // clang-format off
        Vec2f aToB = b - a;
        float angleRadians = atan2(aToB.y, aToB.x);
        float rotated = angleRadians + 0.5f * M_PI;

        Vec2f off = Vec2f(cosf(rotated), sinf(rotated)) * thickness / 2.0f;
        Vec2f offNeg = Vec2f(-off.x, -off.y);

        DebugVertex topLeft =     {{(b.x+offNeg.x) / screenW, (b.y+offNeg.y) / screenH}, {color.r, color.g, color.b, color.a}};
        DebugVertex topRight =    {{(b.x+off.x)    / screenW, (b.y+off.y)    / screenH}, {color.r, color.g, color.b, color.a}};
        DebugVertex bottomLeft =  {{(a.x+offNeg.x) / screenW, (a.y+offNeg.y) / screenH}, {color.r, color.g, color.b, color.a}};
        DebugVertex bottomRight = {{(a.x+off.x)    / screenW, (a.y+off.y)    / screenH}, {color.r, color.g, color.b, color.a}};

        DebugVertex vertices[]
        {
            topLeft, topRight, bottomLeft,
            topRight, bottomRight, bottomLeft
        };
        // clang-format on

        mVao->getVertexBuffer(0)->setData(vertices, sizeof(vertices));

        Bindings bindings;
        bindings.pipeline = mPipeline.get();
        bindings.vao = mVao.get();
        bindings.nonDefaultFramebuffer = nonDefaultFramebuffer;

        commandQueue.cmdDraw(0, sizeof(vertices) / sizeof(*vertices), bindings);
    }
}
