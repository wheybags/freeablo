#include <memory>
#include <render/alignedcpubuffer.h>

namespace Render
{
    AlignedCpuBuffer::AlignedCpuBuffer(const std::vector<Member>& members)
    {
        void* current = nullptr;
        for (const auto& member : members)
        {
            size_t ignoreSpaceLimit = std::numeric_limits<size_t>::max();
            void* thisOffset = std::align(member.alignment, member.size, current, ignoreSpaceLimit);
            current = reinterpret_cast<char*>(current) + member.size;

            mMemberOffsets.push_back(reinterpret_cast<size_t>(thisOffset));
        }

        mData.resize(reinterpret_cast<size_t>(current));
    }
}
