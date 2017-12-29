
#pragma once

#include <cel/celfile.h>
#include <cel/celframe.h>
#include <diabloexe/affix.h>
#include <diabloexe/baseitem.h>
#include <diabloexe/uniqueitem.h>
#include <tuple>
#include <vector>
#include <array>

namespace FARender
{
    class FASpriteGroup;
}

namespace FAWorld
{
    enum class ItemEquipType
    {
        none,
        oneHanded,
        twoHanded,
        chest,
        head,
        ring,
        amulet,
        unequipable,
        belt,
        invalid,
    };

    enum class ItemType
    {
        misc = 0,
        sword,
        axe,
        bow,
        mace,
        shield,
        lightArmor,
        helm,
        mediumArmor,
        heavyArmor,
        staff,
        gold,
        ring,
        amulet,
    };

    enum class ItemClass
    {
        none = 0,
        weapon,
        armor,
        jewelryAndConsumable,
        gold,
        quest,
    };

    // TODO: synchronize names with:
    // https://github.com/sanctuary/notes/blob/72a0772e0d187d29117c4ddd6e6265cafc774a50/enums.h#L128
    enum class ItemEffectType
    {
        IncPercentChanceToHit,
        DecPercentChanceToHit,
        IncPercentDamageDone,
        DecPercentDamageDone,
        IncPercentDamageDoneChanceToHit,
        DecPercentDamageDoneChanceToHit,
        IncPercentArmourClass,
        DecPercentArmourClass,
        IncPercentResistFire,
        IncPercentResistLightning,
        IncPercentResistMagic,
        IncPercentResistAll,
        Unknown6,
        Unknown7,
        ModSpellLevel,
        IncCharges,
        IncFireDamage,
        IncLightningDamage,
        Unknown8,
        IncStrength,
        DecStrength,
        IncMagic,
        DecMagic,
        IncDexterity,
        DecDexterity,
        IncVitality,
        DecVitality,
        IncAllBasicStats,
        DecAllBasicStats,
        IncDamageTaken,
        DecDamageTaken,
        IncHP,
        DecHP,
        IncMana,
        DecMana,
        IncPercentDurability,
        DecPercentDurability,
        Indestructible,
        IncLightRadius,
        DecLightRadius,
        Unknown0,
        MultipleArrows,
        IncPercentFireArrowDamage,
        IncPercentLightningArrowDamage,
        UniquePicture,
        Thorns,
        AllMana,
        PlayerNoHeal,
        Unknown1,
        Unknown2,
        Unknown3,
        Unknown4,
        HalfTrapDamage,
        Knockback,
        MonsterNoHeal,
        PercentManaSteal,
        PercentLifeSteal,
        ArmourPenetration,
        AttackSpeed0,
        HitRecovery,
        FastBlock,
        IncDamageDone,
        RandomArrowSpeed,
        UnusualDamage,
        AlteredDurability,
        NoStrengthRequirment,
        Spell,
        AttackSpeed1,
        OneHanded,
        AntiDemon,
        ZeroAllResist,
        Unknown5,
        ConstantLifeDrain,
        PercentFixedLifeSteal,
        Infravision,
        SpecifiedArmourClass,
        IncHPWithArmourClass,
        IncArmourClassWithMana,
        IncFireResistWithLevel,
        DecArmourClass
    };

    class ItemEffect
    {
    public:
        ItemEffectType type;
        int min;
        int max;
    };

    class Inventory;
    class ItemPosition;
    class Item
    {
        friend class FAWorld::Inventory;
        friend class ItemPosition;

    public:
        bool isEmpty() const;
        Item();
        ~Item();
        Item(DiabloExe::BaseItem item, uint32_t id, DiabloExe::Affix* affix = nullptr, bool isIdentified = true);
        Item(const DiabloExe::UniqueItem& item, uint32_t id);
        std::string getName() const;
        void setUniqueId(uint32_t mUniqueId);
        uint32_t getUniqueId() const;
        void setCount(uint32_t count);
        uint32_t getCount() const;
        uint8_t getBeltX() const;
        bool operator==(const Item rhs) const;
        bool isReal() const;
        std::pair<uint8_t, uint8_t> getInvSize() const;
        std::pair<uint8_t, uint8_t> getInvCoords() const;
        std::pair<uint8_t, uint8_t> getCornerCoords() const;
        std::string getFlipAnimationPath() const;
        std::string getFlipSoundPath() const;
        FARender::FASpriteGroup* getFlipSpriteGroup();
        bool isBeltEquippable() const;

        bool mIsReal;

        uint32_t getActiveTrigger() const;
        uint8_t getReqStr() const;
        uint8_t getReqMagic() const;
        uint8_t getReqDex() const;
        uint8_t getReqVit() const;
        uint32_t getSpecialEffect() const;
        uint32_t getMagicCode() const;
        uint32_t getSpellCode() const;
        uint32_t getUseOnce() const;
        uint32_t getBuyPrice() const;
        uint32_t getSellPrice() const;
        uint32_t getEffect0() const;
        uint32_t getMinRange0() const;
        uint32_t getMaxRange0() const;
        uint32_t getEffect1() const;
        uint32_t getMinRange1() const;
        uint32_t getMaxRange1() const;
        uint32_t getEffect2() const;
        uint32_t getMinRange2() const;
        uint32_t getMaxRange2() const;
        uint32_t getEffect3() const;
        uint32_t getMinRange3() const;
        uint32_t getMaxRange3() const;
        uint32_t getEffect4() const;
        uint32_t getMinRange4() const;
        uint32_t getMaxRange4() const;
        uint32_t getEffect5() const;
        uint32_t getMinRange5() const;
        uint32_t getMaxRange5() const;
        ItemType getType() const;
        ItemEquipType getEquipLoc() const;
        ItemClass getClass() const;
        uint32_t getGraphicValue() const;
        static Item empty;

    private:
        static Cel::CelFile* mObjcurs;
        DiabloExe::Affix mAffix;

        uint32_t mActiveTrigger;
        ItemClass mClass;
        ItemEquipType mEquipLoc;
        uint32_t mGraphicValue;

        ItemType mType;
        uint8_t mUniqCode;
        std::string mName;
        std::string mSecondName;
        uint32_t mQualityLevel;
        uint32_t mDurability;
        uint32_t mCurrentDurability;
        bool mIsIndestructible;

        uint32_t mMinAttackDamage;
        uint32_t mMaxAttackDamage;
        uint32_t mAttackDamage;

        uint32_t mMinArmourClass;
        uint32_t mMaxArmourClass;
        uint32_t mArmourClass;

        uint8_t mReqStr;
        uint8_t mReqMagic;
        uint8_t mReqDex;
        uint8_t mReqVit;

        uint32_t mSpecialEffect;
        uint32_t mMagicCode;
        uint32_t mSpellCode;
        uint32_t mUseOnce;
        uint32_t mBuyPrice;
        uint32_t mSellPrice;

        uint8_t mNumEffects;

        std::array<ItemEffect, 5> mEffects;

        uint8_t mBaseId;
        uint32_t mUniqueId;

        uint32_t mCount;
        uint32_t mMaxCount;

        uint8_t mCornerX;
        uint8_t mCornerY;

        bool mEmpty;
        bool mIsMagic;
        bool mIsIdentified;
        bool mIsUnique;

        uint8_t mInvY;
        uint8_t mInvX;

        uint8_t mBeltX;

        uint8_t mSizeX;
        uint8_t mSizeY;

        static bool mObjcursLoaded;
        std::string mDropItemGraphicsPath;
    };
}
