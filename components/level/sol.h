#ifndef SOL_H
#define SOL_H

#include <vector>
#include <string>

#include <stdint.h>

namespace Level
{
    class Sol
    {
        public:
            Sol(const std::string& path);
            
            bool passable(size_t index) const;
        
        private:
            std::vector<uint8_t> mData;
    };
}

#endif
