#include "usableitembase.h"
#include <diabloexe/baseitem.h>
#include <faworld/item/usableitem.h>
#include <faworld/potion.h>

namespace FAWorld
{
    UsableItemBase::UsableItemBase(const DiabloExe::ExeItem& exeItem) : super(exeItem)
    {
        switch (exeItem.miscId)
        {
            case ItemMiscId::potionOfHealing:
            {
                mEffect = Potion::restoreHp;
                mUseSoundPath = "sfx/items/invpot.wav";
                break;
            }
            case ItemMiscId::potionOfFullHealing:
            {
                mEffect = Potion::restoreHpFull;
                mUseSoundPath = "sfx/items/invpot.wav";
                break;
            }
            case ItemMiscId::potionOfMana:
            {
                mEffect = Potion::restoreMana;
                mUseSoundPath = "sfx/items/invpot.wav";
                break;
            }
            case ItemMiscId::potionOfFullMana:
            {
                mEffect = Potion::restoreManaFull;
                mUseSoundPath = "sfx/items/invpot.wav";
                break;
            }
            case ItemMiscId::potionOfRejuvenation:
            {
                mEffect = [](Player& player) {
                    Potion::restoreHp(player);
                    Potion::restoreMana(player);
                };
                mUseSoundPath = "sfx/items/invpot.wav";
                break;
            }
            case ItemMiscId::potionOfFullRejuvenation:
            {
                mEffect = [](Player& player) {
                    Potion::restoreHpFull(player);
                    Potion::restoreManaFull(player);
                };
                mUseSoundPath = "sfx/items/invpot.wav";
                break;
            }
            case ItemMiscId::elixirOfDexterity:
            {
                mEffect = [](Player& player) { Potion::increaseDexterity(player, 1); };
                mUseSoundPath = "sfx/items/invpot.wav";
                break;
            }
            case ItemMiscId::elixirOfMagic:
            {
                mEffect = [](Player& player) { Potion::increaseMagic(player, 1); };
                mUseSoundPath = "sfx/items/invpot.wav";
                break;
            }
            case ItemMiscId::elixirOfVitality:
            {
                mEffect = [](Player& player) { Potion::increaseVitality(player, 1); };
                mUseSoundPath = "sfx/items/invpot.wav";
                break;
            }
            case ItemMiscId::elixirOfStrength:
            {
                mEffect = [](Player& player) { Potion::increaseStrength(player, 1); };
                mUseSoundPath = "sfx/items/invpot.wav";
                break;
            }
            case ItemMiscId::spectralElixir:
            {
                mEffect = [](Player& player) {
                    Potion::increaseStrength(player, 3);
                    Potion::increaseVitality(player, 3);
                    Potion::increaseMagic(player, 3);
                    Potion::increaseDexterity(player, 3);
                };
                mUseSoundPath = "sfx/items/invpot.wav";
                break;
            }
            default:
                mEffect = [](Player&) {};
                break;
        }
    }

    bool UsableItemBase::isBeltEquippable() const { return mSize == Vec2i(1, 1); }

    std::unique_ptr<Item> UsableItemBase::createItem() const { return std::unique_ptr<Item>(new UsableItem(this)); }
}