
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
        T max;
        T current;

        MaxCurrentItem() = default;
        MaxCurrentItem(T max) : max(max), current(max) {}
        MaxCurrentItem(Serial::Loader& loader);
        void save(Serial::Saver& saver);

        void add(T delta);
        void setMax(T max);
        void reclamp(); ///< make sure the value is clamped into its range
    };
}
