#include "mst.h"
#include <limits>

namespace FALevelGen
{
    // A utility function to find the vertex with minimum key value, from
    // the set of vertices not yet included in MST
    int32_t minKey(const std::vector<int32_t>& key, const std::vector<bool>& mstSet)
    {
        int32_t min = std::numeric_limits<int32_t>::max(), min_index = 0;

        for (int32_t v = 0; v < (int32_t)key.size(); v++)
            if (mstSet[v] == false && key[v] < min)
                min = key[v], min_index = v;

        return min_index;
    }

    // Function to construct Minimum Spanning Tree for a graph, using Primms algorithm
    // (http://en.wikipedia.org/wiki/Prim%27s_algorithm).
    // Taken from http://www.geeksforgeeks.org/greedy-algorithms-set-5-prims-minimum-spanning-tree-mst-2/
    void minimumSpanningTree(const std::vector<std::vector<int32_t> >& graph, std::vector<int32_t>& parent)
    {
        parent.resize(graph[0].size());

        std::vector<int32_t> key(graph[0].size());
        std::vector<bool> mstSet(graph[0].size());

        // Initialize all keys as INFINITE
        for (int32_t i = 0; i < (int32_t)key.size(); i++)
            key[i] = std::numeric_limits<int32_t>::max(), mstSet[i] = false;

        // Always include first 1st vertex in MST.
        key[0] = 0;     // Make key 0 so that this vertex is picked as first vertex
        parent[0] = static_cast<unsigned int> (-1); // First node is always root of MST

        for(int32_t count = 0; count < (int32_t)mstSet.size()-1; count++)
        {
            int32_t u = minKey(key, mstSet);
            mstSet[u] = true;

            // Update key value and parent index of the adjacent vertices of
            // the picked vertex. Consider only those vertices which are not yet
            // included in MST
            for (int32_t v = 0; v < (int32_t)key.size(); v++)
                if (graph[u][v] && mstSet[v] == false && graph[u][v] <  key[v])
                    parent[v]  = u, key[v] = graph[u][v];
         }
    }
}
