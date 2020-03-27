#pragma once
#include <cstdint>
#include <misc/misc.h>
#include <vector>

namespace Render
{
    class AlignedCpuBuffer
    {
    public:
        struct Member
        {
            size_t size;
            size_t alignment;
        };

        explicit AlignedCpuBuffer(const std::vector<Member>& members);
        uint8_t* getMemberPointer(size_t index) { return &mData[mMemberOffsets[index]]; }
        size_t getMemberOffset(size_t index) { return mMemberOffsets[index]; }
        size_t getSizeInBytes() { return mData.size(); }
        uint8_t* data() { return mData.data(); }

    private:
        std::vector<size_t> mMemberOffsets;
        std::vector<uint8_t> mData;
    };

    template <typename... Types> class TypedAlignedCpuBuffer
    {
    public:
        explicit TypedAlignedCpuBuffer(size_t alignment) : mBuffer({{sizeof(Types), alignment}...}) {}

        template <typename T> constexpr size_t getIndexOfType() { return getTypeIndexInTemplateList<T, Types...>(); }

        template <typename T> size_t getMemberOffset() { return mBuffer.getMemberOffset(getIndexOfType<T>()); }

        template <typename T> T* getMemberPointer() { return reinterpret_cast<T*>(mBuffer.getMemberPointer(getIndexOfType<T>())); }

        size_t getSizeInBytes() { return mBuffer.getSizeInBytes(); }
        uint8_t* data() { return mBuffer.data(); }

        AlignedCpuBuffer mBuffer;
    };
}
