#include <render/commandqueue.h>
#include <render/pipeline.h>
#include <render/vertexarrayobject.h>

namespace Render
{
    void Bindings::assertBindingsCorrect()
    {
#ifndef NDEBUG
        debug_assert(pipeline);
        debug_assert(vao);
        debug_assert(pipeline->mSpec.vertexLayouts == vao->getBindings());

        if (!pipeline->mSpec.descriptorSetSpec.items.empty())
        {
            debug_assert(descriptorSet);
            debug_assert(pipeline->mSpec.descriptorSetSpec == descriptorSet->getSpec());
        }
#endif
    }

    void CommandQueue::begin()
    {
        debug_assert(!mBegun);
        debug_assert(mSubmitted);
        mBegun = true;
        mSubmitted = false;
    }

    void CommandQueue::end()
    {
        debug_assert(mBegun);
        mBegun = false;
    }

    void CommandQueue::submit()
    {
        debug_assert(!mSubmitted);
        debug_assert(!mBegun);
        mSubmitted = true;
    }

    void CommandQueue::cmdDraw(size_t, size_t, Render::Bindings& bindings) { bindings.assertBindingsCorrect(); }

    void CommandQueue::cmdDrawIndexed(size_t, size_t, Render::Bindings& bindings)
    {
        bindings.assertBindingsCorrect();
        debug_assert(bindings.vao->getIndexBuffer());
    }

    void CommandQueue::cmdDrawInstances(size_t, size_t, size_t, Render::Bindings& bindings) { bindings.assertBindingsCorrect(); }
}
