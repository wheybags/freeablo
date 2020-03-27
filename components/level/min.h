#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace Level
{
    class Min
    {
    public:
        explicit Min(const std::string& filename);
        Min() = default;

        const std::vector<int16_t>& operator[](size_t index) const;
        size_t size() const;

    private:
        std::vector<std::vector<int16_t>> mPillars;
    };
}
