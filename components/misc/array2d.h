#pragma once
#include <cstdint>
#include <cstring>
#include <memory>
#include <misc/assert.h>
#include <vector>

enum class PointerDataType
{
    Copy,
    NonOwningReference,
    OwningReference
};

namespace Misc
{
    template <typename T> class Array2D
    {
    public:
        typedef T* iterator;
        typedef const T* const_iterator;

        Array2D() = default;
        Array2D(const Array2D&) = delete;
        Array2D(Array2D&& other) noexcept { *this = std::move(other); }

        Array2D(int32_t width, int32_t height, std::vector<T>&& data) : mWidth(width), mHeight(height)
        {
            debug_assert(int32_t(data.size()) == mWidth * mHeight);

            mDataVector = std::make_unique<std::vector<T>>(std::move(data));
            mData = mDataVector->data();
        }

        enum class InitType
        {
            Uninitialised,
            DefaultConstructed
        };
        Array2D(int32_t width, int32_t height, InitType initType = InitType::DefaultConstructed) : mWidth(width), mHeight(height)
        {
            if (initType == InitType::Uninitialised)
            {
                mData = reinterpret_cast<T*>(malloc(sizeof(T) * width * height));
                mUseDelete = false;
            }
            else
            {
                mDataVector = std::make_unique<std::vector<T>>(width * height);
                mData = mDataVector->data();
            }
        }

        Array2D(int32_t width, int32_t height, T* data, PointerDataType dataOwnership) : mWidth(width), mHeight(height)
        {
            switch (dataOwnership)
            {
                case PointerDataType::Copy:
                {
                    mDataVector = std::make_unique<std::vector<T>>(data, data + width * height);
                    mData = mDataVector->data();
                    break;
                }

                case PointerDataType::NonOwningReference:
                {
                    mData = data;
                    mOwnsExternalPointer = false;
                    break;
                }

                case PointerDataType::OwningReference:
                {
                    mData = data;
                    mOwnsExternalPointer = true;
                    break;
                }
            }
        }

        ~Array2D()
        {
            if (mOwnsExternalPointer)
            {
                debug_assert(!mDataVector);

                if (mUseDelete)
                    delete[] mData;
                else
                    free(mData);
            }
        }

        Array2D& operator=(Array2D&& other) noexcept
        {
            mDataVector = std::move(other.mDataVector);
            mData = other.mData;
            mOwnsExternalPointer = other.mOwnsExternalPointer;
            mWidth = other.mWidth;
            mHeight = other.mHeight;

            other.mData = nullptr;
            other.mOwnsExternalPointer = false;
            other.mWidth = 0;
            other.mHeight = 0;

            return *this;
        }

        void memcpyTo(Array2D& other) const
        {
            static_assert(std::is_trivially_constructible<T>::value, "probably not a good idea to memcpy complex types");
            other.resize(width(), height());

            memcpy(other.data(), data(), mWidth * mHeight);
        }

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

        iterator begin() { return mData; }
        const_iterator begin() const { return mData; }
        iterator end() { return mData + width() * height(); }
        const_iterator end() const { return mData + width() * height(); }

        int32_t width() const { return mWidth; }
        int32_t height() const { return mHeight; }

        bool empty() const { return mData == nullptr && mWidth == 0 && mHeight == 0; }

        bool ownsData() const { return mDataVector || mOwnsExternalPointer || empty(); }

        void resize(int32_t newWidth, int32_t newHeight)
        {
            release_assert(mDataVector || empty());

            mWidth = newWidth;
            mHeight = newHeight;

            if (!mDataVector)
                mDataVector = std::make_unique<std::vector<T>>(mWidth * mHeight);
            else
                mDataVector->resize(mWidth * mHeight);

            mData = mDataVector->data();
        }

        T* data() { return mData; }
        const T* data() const { return mData; }

    private:
        int32_t mWidth = 0;
        int32_t mHeight = 0;

        std::unique_ptr<std::vector<T>> mDataVector;

        T* mData = nullptr;
        bool mUseDelete = false;
        bool mOwnsExternalPointer = false;
    };
}
