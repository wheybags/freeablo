
#pragma once

#include <string>
#include <vector>

#include <stdint.h>

namespace Level
{
    class Sol
    {
    public:
        Sol(const std::string& path);
        Sol() {}

        bool passable(size_t index) const;

        size_t size() const;

    private:
        std::vector<uint8_t> mData;
    };
}

