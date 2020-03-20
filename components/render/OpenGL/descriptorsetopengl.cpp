#include <misc/assert.h>
#include <render/OpenGL/descriptorsetopengl.h>

namespace Render
{
    DescriptorSetOpenGL::DescriptorSetOpenGL(DescriptorSetSpec spec) : DescriptorSet(std::move(spec)) { mCurrentItems.resize(mSourceSpec.items.size()); }

    void DescriptorSetOpenGL::updateItems(const std::vector<Item>& items)
    {
        super::updateItems(items);

        for (const auto& item : items)
            mCurrentItems[item.bindingIndex] = item;
    }

    const DescriptorSet::Item& DescriptorSetOpenGL::getItem(uint32_t bindingIndex) const
    {
        debug_assert(mCurrentItems[bindingIndex].has_value());
        return *mCurrentItems[bindingIndex];
    }
}