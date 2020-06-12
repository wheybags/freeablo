#include "simplebuffdebuffeffect.h"
#include "simplebuffdebuffeffectbase.h"
#include <fasavegame/gameloader.h>
#include <faworld/actorstats.h>
#include <fmt/format.h>
#include <misc/misc.h>

namespace FAWorld
{
    SimpleBuffDebuffEffect::SimpleBuffDebuffEffect(const SimpleBuffDebuffEffectBase* base) : super(base) {}

    void SimpleBuffDebuffEffect::init()
    {
        mValue = getBase()->mParameter2; // TODO: this should be randomised
    }

    void SimpleBuffDebuffEffect::save(FASaveGame::GameSaver& saver) const
    {
        super::save(saver);
        saver.save(mValue);
    }

    void SimpleBuffDebuffEffect::load(FASaveGame::GameLoader& loader)
    {
        super::load(loader);
        mValue = loader.load<int32_t>();
    }

    void SimpleBuffDebuffEffect::apply(MagicStatModifiers& modifiers) const
    {
        switch (getBase()->mAttribute)
        {
            case SimpleBuffDebuffEffectBase::Attribute::Strength:
                modifiers.baseStats.strength += mValue;
                break;
            case SimpleBuffDebuffEffectBase::Attribute::Magic:
                modifiers.baseStats.magic += mValue;
                break;
            case SimpleBuffDebuffEffectBase::Attribute::Dexterity:
                modifiers.baseStats.dexterity += mValue;
                break;
            case SimpleBuffDebuffEffectBase::Attribute::Vitality:
                modifiers.baseStats.vitality += mValue;
                break;
            case SimpleBuffDebuffEffectBase::Attribute::ToHit:
                modifiers.toHit += mValue;
                break;
            case SimpleBuffDebuffEffectBase::Attribute::Life:
                modifiers.maxLife += mValue;
                break;
            case SimpleBuffDebuffEffectBase::Attribute::Mana:
                modifiers.maxMana += mValue;
                break;
            case SimpleBuffDebuffEffectBase::Attribute::Damage:
                modifiers.meleeDamageBonus += mValue;
                modifiers.rangedDamageBonus += mValue;
                break;
            case SimpleBuffDebuffEffectBase::Attribute::Armor:
                modifiers.armorClass += mValue;
                break;
        }
    }

    const SimpleBuffDebuffEffectBase* SimpleBuffDebuffEffect::getBase() const { return safe_downcast<const SimpleBuffDebuffEffectBase*>(mBase); }

    std::string SimpleBuffDebuffEffect::getFullDescription() const { return fmt::format(getBase()->mDescriptionFormatString, mValue); }
}