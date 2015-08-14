#ifndef ITEM_H
#define ITEM_H
#include <diabloexe/baseitem.h>
#include <diabloexe/prefix.h>
#include <cel/celfile.h>
#include <cel/celframe.h>


namespace FAWorld
{
class Inventory;
}

namespace Level
{

class ItemPosition;
class Item
{
    friend class FAWorld::Inventory;
    friend class ItemPosition;
public:
    bool inline isEmpty(){return mEmpty;}
    Item();
    ~Item();
    Item(DiabloExe::BaseItem item, size_t id);
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
    DiabloExe::BaseItem mItem;


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


private:
    static Cel::CelFile * mObjcurs;
    DiabloExe::Prefix mPrefix;
    uint8_t mBaseId;
    uint32_t mUniqueId;
    uint32_t mCount;
    uint32_t mMaxCount;
    uint8_t mCornerX;
    uint8_t mCornerY;
    bool mEmpty;
    uint8_t mInvY;
    uint8_t mInvX;
    uint8_t mBeltX;
    uint8_t mSizeX;
    uint8_t mSizeY;
    static bool mObjcursLoaded;

};
}
#endif // ITEM_H
