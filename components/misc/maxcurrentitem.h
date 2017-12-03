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

        MaxCurrentItem(T max) : max(max), current(max) {}
        MaxCurrentItem(Serial::Loader& loader);
        void save(Serial::Saver& saver);

        bool change(T delta, bool allowClamp = true);
        void reclamp(); ///< make sure the value is clamped into its range
    };
}
