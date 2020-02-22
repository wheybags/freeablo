#pragma once
#include <cstddef>
#include <stdint.h>
#include <vector>

namespace FALevelGen
{
    void minimumSpanningTree(const std::vector<std::vector<int32_t>>& graph, std::vector<int32_t>& parent);
}
