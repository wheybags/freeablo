#include "item.h"
#include "../engine/enginemain.h"
#include "../fagui/textcolor.h"
#include "../farender/renderer.h"
#include "../fasavegame/gameloader.h"
#include "itemenums.h"
#include "itemfactory.h"
#include <fmt/format.h>
#include <iostream>
#include <random/random.h>

namespace FAWorld
{
    void Item::save(FASaveGame::GameSaver& saver) const
    {
        saver.save(mIsReal);
        saver.save(mUniqueId);
        saver.save(mCount);

        saver.save(mArmorClass);
        saver.save(mCurrentDurability);
        saver.save(mMaxDurability);
        saver.save(mCurrentCharges);
        saver.save(mMaxCharges);

        saver.save(static_cast<int32_t>(mBaseId));
        saver.save(mEmpty);

        saver.save(mInvY);
        saver.save(mInvX);

        saver.save(mCornerX);
        saver.save(mCornerY);
    }

    std::string Item::damageStr() const { return fmt::format("damage: {} - {}", getMinAttackDamage(), getMaxAttackDamage()); }

    std::string Item::armorStr() const { return fmt::format("armor: {}", mArmorClass); }

    std::string Item::damageOrArmorStr() const
    {
        if (getClass() == ItemClass::armor)
            return armorStr();

        if (getClass() == ItemClass::weapon)
            return damageStr();
        return {};
    }

    std::string Item::durabilityStr() const
    {
        if (getClass() != ItemClass::armor && getClass() != ItemClass::weapon)
            return {};

        if (mMaxDurability == indestructibleItemDurability)
            return "Indestructible";
        else
            return fmt::format("Dur: {}/{}", mCurrentDurability, mMaxDurability);
    }

    std::string Item::requirementsStr() const
    {
        if (getRequiredStrength() + getRequiredDexterity() + getRequiredMagic() == 0)
            return {};
        std::string str = "Required:";
        if (getRequiredStrength() > 0)
            str += fmt::format(" {} Str", getRequiredStrength());
        if (getRequiredMagic() > 0)
            str += fmt::format(" {} Mag", getRequiredMagic());
        if (getRequiredDexterity() > 0)
            str += fmt::format(" {} Dex", getRequiredDexterity());
        return str;
    }

    const DiabloExe::BaseItem& Item::base() const { return Engine::EngineMain::get()->exe().getBaseItems()[static_cast<int32_t>(mBaseId)]; }

    bool Item::isItemAMeleeWeapon(ItemType type)
    {
        switch (type)
        {
            case ItemType::sword:
            case ItemType::axe:
            case ItemType::mace:
            case ItemType::staff:
                return true;
            case ItemType::misc:
            case ItemType::bow:
            case ItemType::shield:
            case ItemType::lightArmor:
            case ItemType::helm:
            case ItemType::mediumArmor:
            case ItemType::heavyArmor:
            case ItemType::gold:
            case ItemType::ring:
            case ItemType::amulet:
            case ItemType::none:
                return false;
        }

        invalid_enum(ItemType, type);
    }

    bool Item::isItemARangedWeapon(ItemType type)
    {
        switch (type)
        {
            case ItemType::bow:
                return true;
            case ItemType::sword:
            case ItemType::axe:
            case ItemType::mace:
            case ItemType::staff:
            case ItemType::misc:
            case ItemType::shield:
            case ItemType::lightArmor:
            case ItemType::helm:
            case ItemType::mediumArmor:
            case ItemType::heavyArmor:
            case ItemType::gold:
            case ItemType::ring:
            case ItemType::amulet:
            case ItemType::none:
                return false;
        }

        invalid_enum(ItemType, type);
    }

    std::string Item::getFullDescription() const
    {
        auto description = getName();
        auto dmrOrArmor = damageOrArmorStr();
        if (!dmrOrArmor.empty())
            description += '\n' + dmrOrArmor;

        auto dur = durabilityStr();
        if (!dur.empty())
            description += " " + dur;

        description += chargesStr();
        if (getQuality() == ItemQuality::unique)
            description += "\nUnique Item";
        // TODO: affix/prefix description
        auto reqs = requirementsStr();
        if (!reqs.empty())
            description += '\n' + reqs;

        return description;
    }

    std::vector<FAGui::MenuEntry> Item::descriptionForMerchants() const
    {
        std::vector<FAGui::MenuEntry> ret;
        auto append = [](std::string& target, const std::string& new_part) {
            if (!target.empty())
                target += ",  ";
            target += new_part;
        };
        ret.push_back({getName(), true});
        {
            // first line - affixes + charges
            std::string str;
            if (mMaxCharges > 0)
                append(str, chargesStr());
            if (!str.empty())
                ret.push_back({std::move(str), FAGui::TextColor::white, false});
        }
        {
            std::string str;
            str += damageOrArmorStr();
            if (!str.empty())
                str += "  ";
            str += durabilityStr();
            auto reqs = requirementsStr();
            append(str, reqs.empty() ? "No Required Attributes" : reqs);
            if (!str.empty())
                ret.push_back({std::move(str), false});
        }
        {
            ret.push_back({"Price: " + std::to_string(this->getPrice()), FAGui::TextColor::white, false});
        }
        while (ret.size() < 4)
            ret.emplace_back();
        return ret;
    }

    std::array<int32_t, 2> Item::getInvSize() const
    {
        if (isEmpty())
            return {0, 0};
        return {base().invSizeX, base().invSizeY};
    }
    std::pair<uint8_t, uint8_t> Item::getInvCoords() const { return {mInvX, mInvY}; }
    std::pair<uint8_t, uint8_t> Item::getCornerCoords() const { return {mCornerX, mCornerY}; }
    int32_t Item::getInvVolume() const
    {
        if (isEmpty())
            return 0;

        return base().invSizeX * base().invSizeY;
    }

    void Item::load(FASaveGame::GameLoader& loader)
    {
        mIsReal = loader.load<bool>();
        mUniqueId = loader.load<int32_t>();
        mCount = loader.load<int32_t>();

        mArmorClass = loader.load<int32_t>();
        mCurrentDurability = loader.load<int32_t>();
        mMaxDurability = loader.load<int32_t>();
        mCurrentCharges = loader.load<int32_t>();
        mMaxCharges = loader.load<int32_t>();

        mBaseId = static_cast<ItemId>(loader.load<int32_t>());
        mEmpty = loader.load<bool>();

        mInvY = loader.load<uint8_t>();
        mInvX = loader.load<uint8_t>();

        mCornerX = loader.load<int32_t>();
        mCornerY = loader.load<int32_t>();
    }

    std::string Item::getName() const
    {
        if (getType() == ItemType::gold)
            return fmt::format("{} gold {}", mCount, (mCount > 1 ? "pieces" : "piece"));
        return base().name;
    }

    ItemQuality Item::getQuality() const
    {
        // TODO: support other kinds of items
        return ItemQuality::normal;
    }

    Item::~Item() {}

    std::string Item::getFlipSoundPath() const { return base().dropItemSoundPath; }

    std::string Item::getInvPlaceSoundPath() const { return base().invPlaceItemSoundPath; }

    FARender::FASpriteGroup* Item::getFlipSpriteGroup() { return FARender::Renderer::get()->loadImage(base().dropItemGraphicsPath); }

    bool Item::isBeltEquippable() const { return getInvSize() == std::array<int32_t, 2>{1, 1} && isUsable() && getType() != ItemType::gold; }

    int32_t Item::getMaxCount() const
    {
        if (getType() == ItemType::gold)
            return 5000;
        return 1;
    }

    int32_t Item::getRequiredStrength() const { return base().requiredStrength; }
    int32_t Item::getRequiredMagic() const { return base().requiredMagic; }
    int32_t Item::getRequiredDexterity() const { return base().requiredDexterity; }

    uint32_t Item::getSpecialEffect() const { return base().specialEffectFlags; }

    ItemMiscId Item::getMiscId() const { return static_cast<ItemMiscId>(base().miscId); }

    uint32_t Item::getSpellCode() const { return base().spellId; }

    bool Item::isUsable() const { return base().isUsable; }

    int32_t Item::getPrice() const { return static_cast<int32_t>(base().price); }

    ItemType Item::getType() const
    {
        if (isEmpty())
            return ItemType::none;
        return static_cast<ItemType>(base().type);
    }

    ItemEquipType Item::getEquipLoc() const
    {
        if (isEmpty())
            return ItemEquipType::none;
        return static_cast<ItemEquipType>(base().equipType);
    }

    ItemClass Item::getClass() const
    {
        if (isEmpty())
            return ItemClass::none;
        return static_cast<ItemClass>(base().itemClass);
    }
    uint32_t Item::getGraphicValue() const
    {
        if (isEmpty())
            return 0;

        if (getType() == ItemType::gold)
        {
            if (mCount <= 1000)
                return 15;
            if (mCount <= 2500)
                return 16;

            return 17;
        }
        return base().invGraphicsId + 11;
    }

    int32_t Item::getMinAttackDamage() const { return base().minAttackDamage; }

    int32_t Item::getMaxAttackDamage() const { return base().maxAttackDamage; }

    std::string Item::chargesStr() const
    {
        if (getMiscId() == ItemMiscId::staff && mMaxCharges > 0)
            return fmt::format("\nCharges: {}/{}", mCurrentCharges, mMaxCharges);

        return {};
    }

    void Item::setUniqueId(uint32_t mUniqueId) { this->mUniqueId = mUniqueId; }

    uint32_t Item::getUniqueId() const { return this->mUniqueId; }

    bool Item::operator==(const Item rhs) const { return this->mUniqueId == rhs.mUniqueId; }
}
