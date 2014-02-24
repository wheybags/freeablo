#ifndef MIN_H
#define MIN_H

#include <string>
#include <stdint.h>
#include <vector>

namespace Level
{
    class MinPillar
    {
        public:
            size_t size() const;
            int16_t operator[] (size_t index) const;
            bool passable() const; ///< Checks if any above ground blocks are non-empty
        
        private:
            MinPillar(std::vector<int16_t> data);
            std::vector<int16_t> mData;

            friend class Min;
    };

    class Min
    {
        public:
            Min(const std::string&);
            const MinPillar& operator[] (size_t index) const;
            size_t size();

        private:
            std::vector<MinPillar> mPillars;
    };
}

#endif
