#pragma once
#include <cstdint>
#include <misc/assert.h>
#include <vector>

namespace Misc
{
    template <typename T> class Array2D
    {
    public:
        typedef typename std::vector<T>::iterator iterator;
        typedef typename std::vector<T>::const_iterator const_iterator;

        Array2D()
        {
            mWidth = 0;
            mHeight = 0;
            mData.resize(0);
        }

        Array2D(int32_t width, int32_t height)
        {
            mWidth = width;
            mHeight = height;

            mData.resize(width * height);
        }

        Array2D(int32_t width, int32_t height, std::vector<T>&& data)
        {
            mWidth = width;
            mHeight = height;
            debug_assert(int32_t(data.size()) == mWidth * mHeight);
            mData = std::move(data);
        }

        Array2D(const Array2D&) = delete;

        Array2D(Array2D&&) = default;
        Array2D& operator=(Array2D&&) = default;

        bool pointIsValid(int32_t x, int32_t y) const { return (x >= 0 && x < mWidth && y >= 0 && y < mHeight); }

        const T& get(int32_t x, int32_t y) const
        {
            debug_assert(pointIsValid(x, y));
            return mData[x + y * mWidth];
        }

        T& get(int32_t x, int32_t y)
        {
            debug_assert(pointIsValid(x, y));
            return mData[x + y * mWidth];
        }

        iterator begin() { return mData.begin(); }
        const_iterator begin() const { return mData.begin(); }
        iterator end() { return mData.end(); }
        const_iterator end() const { return mData.end(); }

        int32_t width() const { return mWidth; }
        int32_t height() const { return mHeight; }

        void resize(int32_t newWidth, int32_t newHeight)
        {
            mWidth = newWidth;
            mHeight = newHeight;

            mData.resize(mWidth * mHeight);
        }

        T* data() { return mData.data(); }
        std::vector<T>& getFlatVector() { return mData; }
        const std::vector<T>& getFlatVector() const { return mData; }

    private:
        std::vector<T> mData;
        int32_t mWidth;
        int32_t mHeight;
    };
}
