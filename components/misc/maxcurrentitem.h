#pragma once
namespace Serial
{
    class Loader;
    class Saver;
}

namespace Misc
{
    template <typename T> class MaxCurrentItem
    {
    public:
        T max = 0;
        T current = 0;

        MaxCurrentItem() = default;
        explicit MaxCurrentItem(T max) : max(max), current(max) {}
        explicit MaxCurrentItem(Serial::Loader& loader);
        void save(Serial::Saver& saver) const;

        void add(T delta);
        void setMax(T max);
        void reclamp(); ///< make sure the value is clamped into its range
    };
}
