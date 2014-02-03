#ifndef MST_H
#define MST_H

#include <vector>
#include <cstddef>

namespace FALevelGen
{
    void minimumSpanningTree(const std::vector<std::vector<size_t> >& graph, std::vector<size_t>& parent);
}

#endif
