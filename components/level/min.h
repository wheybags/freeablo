#pragma once
#include <stdint.h>
#include <string>
#include <vector>

namespace Level
{
    class Min
    {
    public:
        Min(const std::string&);
        Min() {}

        const std::vector<int16_t>& operator[](size_t index) const;
        size_t size() const;

    private:
        std::vector<std::vector<int16_t>> mPillars;
    };
}
