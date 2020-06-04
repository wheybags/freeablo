#include "item.h"
#include "itembase.h"

namespace FAWorld
{
    int32_t Item::getPrice() const { return getBase()->mPrice; }

    std::string Item::getFullDescription() const { return getBase()->mName; }

    std::vector<FAGui::MenuEntry> Item::descriptionForMerchants() const
    {
        std::vector<FAGui::MenuEntry> description;

        description.emplace_back(getBase()->mName, true);
        // {
        //     // first line - affixes + charges
        //     std::string str;
        //     if (mMaxCharges > 0)
        //         append(str, chargesStr());
        //     if (!str.empty())
        //         ret.emplace_back(std::move(str), FAGui::TextColor::white, false);
        // }
        // {
        //     std::string str;
        //     str += damageOrArmorStr();
        //     if (!str.empty())
        //         str += "  ";
        //     str += durabilityStr();
        //     auto reqs = requirementsStr();
        //     append(str, reqs.empty() ? "No Required Attributes" : reqs);
        //     if (!str.empty())
        //         ret.emplace_back(std::move(str), false);
        // }
        {
            description.emplace_back("Price: " + std::to_string(this->getPrice()), FAGui::TextColor::white, false);
        }
        while (description.size() < 4)
            description.emplace_back();

        return description;
    }

    const Render::TextureReference* Item::getInventoryIcon() const { return getBase()->mInventoryIcon; }

    const Render::Cursor* Item::getInventoryIconCursor() const { return getBase()->mInventoryIconCursor.get(); }
}