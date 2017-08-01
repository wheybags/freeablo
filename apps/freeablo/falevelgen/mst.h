#ifndef MST_H
#define MST_H

#include <vector>
#include <cstddef>
#include <stdint.h>

namespace FALevelGen
{
    void minimumSpanningTree(const std::vector<std::vector<int32_t> >& graph, std::vector<int32_t>& parent);
}

#endif
