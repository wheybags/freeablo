#include <misc/misc.h>
#include <render/descriptorset.h>

namespace Render
{
    void DescriptorSet::updateItems(const std::vector<Item>& items)
    {
#ifndef DEBUG
        for (const auto& item : items)
        {
            debug_assert(item.bindingIndex < mSourceSpec.items.size());

            switch (mSourceSpec.items[item.bindingIndex].type)
            {
                case DescriptorType::Texture:
                    debug_assert(std::holds_alternative<Texture*>(item.item));
                    break;
                case DescriptorType::UniformBuffer:
                    debug_assert(std::holds_alternative<BufferSlice>(item.item));
                    break;
            }
        }
#endif
    }
}