#include "maxcurrentitem.h"
#include <algorithm>
#include <serial/loader.h>

namespace Misc
{
    template<typename T>
    MaxCurrentItem<T>::MaxCurrentItem(Serial::Loader& loader)
    {
        max = loader.load<T>();
        current = loader.load<T>();
    }

    template<typename T>
    void MaxCurrentItem<T>::save(Serial::Saver& saver)
    {
        saver.save(max);
        saver.save(current);
    }

    template<typename T>
    bool MaxCurrentItem<T>::change(T delta, bool allowClamp)
    {
        T next = std::max(std::min(current + delta, max), 0);
        bool clamped = next != current + delta;

        if (!clamped || allowClamp)
            current = next;

        return clamped;
    }

    template<typename T>
    void MaxCurrentItem<T>::reclamp()
    {
        change(0, true);
    }

    // explicit template instantiations, instead of putting all the implementations in the header
    template class MaxCurrentItem<int32_t>;
}
