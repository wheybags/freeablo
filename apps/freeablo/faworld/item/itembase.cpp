#include "itembase.h"

namespace FAWorld
{
    ItemBase::ItemBase(const DiabloExe::ExeItem& exeItem)
        : mId(exeItem.idName), mType(exeItem.type), mName(exeItem.name), mShortName(exeItem.shortName), mSize(exeItem.invSizeX, exeItem.invSizeY),
          mDropItemGraphicsPath(exeItem.dropItemGraphicsPath), mInventoryGraphicsId(exeItem.invGraphicsId), mDropItemSoundPath(exeItem.dropItemSoundPath),
          mInventoryPlaceItemSoundPath(exeItem.invPlaceItemSoundPath)
    {}
}