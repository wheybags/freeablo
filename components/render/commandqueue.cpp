#include <render/commandqueue.h>

namespace Render
{
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

    void CommandQueue::cmdDraw(size_t, size_t, Render::Bindings& bindings) { bindings.assertFilled(); }

    void CommandQueue::cmdDrawIndexed(size_t, size_t, Render::Bindings& bindings)
    {
        bindings.assertFilled();
        debug_assert(bindings.vao->getIndexBuffer());
    }

    void CommandQueue::cmdDrawInstances(size_t, size_t, size_t, Render::Bindings& bindings) { bindings.assertFilled(); }
}
