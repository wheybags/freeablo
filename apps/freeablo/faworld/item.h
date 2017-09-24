#ifndef ITEM_H
#define ITEM_H
#include <diabloexe/baseitem.h>
#include <diabloexe/uniqueitem.h>
#include <diabloexe/affix.h>
#include <cel/celfile.h>
#include <cel/celframe.h>
#include <vector>
#include <tuple>

namespace FARender
{
    class FASpriteGroup;
}

namespace FAWorld
{
class Inventory;
class ItemPosition;
class Item
{
    friend class FAWorld::Inventory;
    friend class ItemPosition;
public:
    typedef enum
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
    }ItemEffect;

    bool isEmpty() const;
    Item();
    ~Item();
    Item(DiabloExe::BaseItem item, uint32_t id, DiabloExe::Affix *affix=NULL, bool isIdentified=true);
    Item(const DiabloExe::UniqueItem &item, uint32_t id);
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
    FARender::FASpriteGroup *getFlipSpriteGroup();
    bool isBeltEquippable() const;


    bool mIsReal;
    typedef enum{
        eqINV,
        eqONEHAND,
        eqTWOHAND,
        eqBODY,
        eqHEAD,
        eqRING,
        eqAMULET,
        eqUNEQUIP,
        eqBELT,
        eqFLOOR,
        eqCURSOR,
        eqRIGHTHAND,
        eqLEFTHAND,
        eqRIGHTRING,
        eqLEFTRING} equipLoc;

    typedef enum{itWEAPON=1,
                 itARMOUR=2,
                 itPOT=3,
                 itGOLD=4,
                 itNOVELTY=5}itemType;

    typedef enum{
        icOther,
        icSword,
        icAxe,
        icBow,
        icBlunt,
        icShield,
        icLightArmour,
        icHelm,
        icMidArmour,
        icHeavyArmour,
        icStave,
        icGold,
        icRing,
        icAmulet
    }itemCode;

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
    itemCode getCode() const;
    equipLoc getEquipLoc() const;
    itemType getType() const;
    uint32_t getGraphicValue() const;
    static Item empty;

private:
    static Cel::CelFile * mObjcurs;
    std::vector<std::tuple<ItemEffect, uint32_t, uint32_t, uint32_t>> mEffects;
    DiabloExe::Affix mAffix;

    uint32_t mActiveTrigger;
    itemType mType;
    equipLoc mEquipLoc;
    uint32_t mGraphicValue;

    itemCode mCode;
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

    uint8_t  mReqStr;
    uint8_t  mReqMagic;
    uint8_t  mReqDex;
    uint8_t  mReqVit;

    uint32_t mSpecialEffect;
    uint32_t mMagicCode;
    uint32_t mSpellCode;
    uint32_t mUseOnce;
    uint32_t mBuyPrice;
    uint32_t mSellPrice;

    uint8_t mNumEffects;

    uint32_t mEffect0;
    uint32_t mMinRange0;
    uint32_t mMaxRange0;
    uint32_t mRange0;

    uint32_t mEffect1;
    uint32_t mMinRange1;
    uint32_t mMaxRange1;
    uint32_t mRange1;

    uint32_t mEffect2;
    uint32_t mMinRange2;
    uint32_t mMaxRange2;
    uint32_t mRange2;

    uint32_t mEffect3;
    uint32_t mMinRange3;
    uint32_t mMaxRange3;
    uint32_t mRange3;

    uint32_t mEffect4;
    uint32_t mMinRange4;
    uint32_t mMaxRange4;
    uint32_t mRange4;

    uint32_t mEffect5;
    uint32_t mMinRange5;
    uint32_t mMaxRange5;
    uint32_t mRange5;



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

};
}
#endif // ITEM_H
