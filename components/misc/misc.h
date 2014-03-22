#ifndef FA_MISC_H
#define FA_MISC_H

#include <utility>
#include <stdint.h>
#include <stddef.h>

namespace Misc
{
    int32_t getVecDir(const std::pair<float, float>& vector);

    template <typename T>
    std::pair<float, float> getVec(const std::pair<T,T>& from, const std::pair<T,T>& to)
    {
        return std::make_pair(((float)to.first)-((float)from.first), ((float)to.second)-((float)from.second));
    }
}

#endif
