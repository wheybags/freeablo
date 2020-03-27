#pragma once
#include <optional>
#include <render/descriptorset.h>

namespace Render
{
    class DescriptorSetOpenGL final : public DescriptorSet
    {
        using super = DescriptorSet;

    public:
        explicit DescriptorSetOpenGL(DescriptorSetSpec spec);
        ~DescriptorSetOpenGL() override = default;

        void updateItems(const std::vector<Item>& items) override;

    public:
        const Item& getItem(uint32_t bindingIndex) const;

    private:
        std::vector<std::optional<Item>> mCurrentItems;
    };
}
