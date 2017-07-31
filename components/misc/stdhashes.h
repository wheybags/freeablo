#pragma once

// hash functions for types that are missing them, so they can be sued as keys in std::unordered_map

namespace std
{
    // std::pair
    template<typename a, typename b>
    struct hash< std::pair<a, b> > {
    private:
        const hash<a> ah;
        const hash<b> bh;
    public:
        hash() : ah(), bh() {}
        size_t operator()(const std::pair<a, b> &p) const {
            return ah(p.first) * 1812433253 + bh(p.second);
        }
    };
}

struct EnumClassHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};
