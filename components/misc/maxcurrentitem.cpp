#include "maxcurrentitem.h"
#include "misc/assert.h"
#include <algorithm>
#include <serial/loader.h>

namespace Misc
{
    template <typename T> MaxCurrentItem<T>::MaxCurrentItem(Serial::Loader& loader)
    {
        max = loader.load<T>();
        current = loader.load<T>();
    }

    template <typename T> void MaxCurrentItem<T>::save(Serial::Saver& saver) const
    {
        saver.save(max);
        saver.save(current);
    }

    template <typename T> void MaxCurrentItem<T>::add(T delta) { current = std::max(std::min(current + delta, max), 0); }

    template <typename T> void MaxCurrentItem<T>::setMax(T _max)
    {
        release_assert(_max >= 0);
        max = _max;
        reclamp();
    }

    template <typename T> void MaxCurrentItem<T>::reclamp() { current = std::max(std::min(current, max), 0); }

    // explicit template instantiations, instead of putting all the implementations in the header
    template class MaxCurrentItem<int32_t>;
}
