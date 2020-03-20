#pragma once
#include <render/buffer.h>
#include <string>
#include <variant>
#include <vector>

namespace Render
{
    class Texture;

    enum class DescriptorType
    {
        Texture,
        UniformBuffer,
    };

    class DescriptorSetSpec
    {
    public:
        struct Item
        {
            DescriptorType type;
            std::string glName;

            bool operator==(const Item& other) const { return type == other.type && glName == other.glName; }
        };

        std::vector<Item> items;

        bool operator==(const DescriptorSetSpec& other) const { return items == other.items; }
    };

    class DescriptorSet
    {
    public:
        DescriptorSet(DescriptorSet&) = delete;
        explicit DescriptorSet(DescriptorSetSpec spec) : mSourceSpec(std::move(spec)) {}

        struct Item
        {
            uint32_t bindingIndex;
            std::variant<BufferSlice, Texture*> item;
        };

        virtual void updateItems(const std::vector<Item>& items);
        uint32_t size() const { return uint32_t(mSourceSpec.items.size()); }
        const DescriptorSetSpec& getSpec() const { return mSourceSpec; };

        virtual ~DescriptorSet() = default;

    protected:
        DescriptorSetSpec mSourceSpec;
    };
}