#define KEEP_CALL_ON_ALL_PLAYER_INPUTS
#include "playerinput.h"
#undef KEEP_CALL_ON_ALL_PLAYER_INPUTS
#include <misc/assert.h>
#include <serial/loader.h>

namespace FAWorld
{
#define GENERATE_CONSTRUCTOR_BODY(X)                                                                                                                           \
    PlayerInput::PlayerInput(const X##Data& data, int32_t actorId)                                                                                             \
    {                                                                                                                                                          \
        mData.data##X = data;                                                                                                                                  \
        mType = Type::X;                                                                                                                                       \
        mActorId = actorId;                                                                                                                                    \
    }

    CALL_ON_ALL_PLAYER_INPUTS(GENERATE_CONSTRUCTOR_BODY)

#undef GENERATE_CONSTRUCTOR_BODY

    void PlayerInput::save(Serial::Saver& saver)
    {
        saver.save(uint8_t(mType));
        saver.save(mActorId);

        if (false)
        {
        }
#define GENERATE_SAVE_BODY(X)                                                                                                                                  \
    else if (mType == Type::X) { mData.data##X.save(saver); }
        CALL_ON_ALL_PLAYER_INPUTS(GENERATE_SAVE_BODY)
#undef GENERATE_SAVE_BODY
        else invalid_enum(Type, mType);
    }

    void PlayerInput::load(Serial::Loader& loader)
    {
        mType = Type(loader.load<uint8_t>());
        mActorId = loader.load<int32_t>();

        if (false)
        {
        }
#define GENERATE_LOAD_BODY(X)                                                                                                                                  \
    else if (mType == Type::X) { mData.data##X.load(loader); }
        CALL_ON_ALL_PLAYER_INPUTS(GENERATE_LOAD_BODY)
#undef GENERATE_LOAD_BODY
        else invalid_enum(Type, mType);
    }

#undef CALL_ON_ALL_PLAYER_INPUTS

    void PlayerInput::TargetTileData::save(Serial::Saver& saver)
    {
        saver.save(x);
        saver.save(y);
    }

    void PlayerInput::TargetTileData::load(Serial::Loader& loader)
    {
        x = loader.load<int32_t>();
        y = loader.load<int32_t>();
    }

    void PlayerInput::DragOverTileData::save(Serial::Saver& saver)
    {
        saver.save(x);
        saver.save(y);
    }

    void PlayerInput::DragOverTileData::load(Serial::Loader& loader)
    {
        x = loader.load<int32_t>();
        y = loader.load<int32_t>();
    }

    void PlayerInput::TargetActorData::save(Serial::Saver& saver) { saver.save(actorId); }

    void PlayerInput::TargetActorData::load(Serial::Loader& loader) { actorId = loader.load<int32_t>(); }

    void PlayerInput::TargetItemOnFloorData::save(Serial::Saver& saver)
    {
        position.save(saver);
        saver.save(uint8_t(type));
    }

    void PlayerInput::TargetItemOnFloorData::load(Serial::Loader& loader)
    {
        position = Misc::Point(loader);
        type = Target::ItemTarget::ActionType(loader.load<uint8_t>());
    }

    void PlayerInput::AttackDirectionData::save(Serial::Saver& saver) { saver.save(uint8_t(direction)); }

    void PlayerInput::AttackDirectionData::load(Serial::Loader& loader) { direction = Misc::Direction(loader.load<uint8_t>()); }

    void PlayerInput::ChangeLevelData::save(Serial::Saver& saver) { saver.save(uint8_t(direction)); }

    void PlayerInput::ChangeLevelData::load(Serial::Loader& loader) { direction = Direction(loader.load<uint8_t>()); }

    void PlayerInput::InventorySlotClickedData::save(Serial::Saver& saver) { slot.save(saver); }

    void PlayerInput::InventorySlotClickedData::load(Serial::Loader& loader) { slot.load(loader); }

    void PlayerInput::SplitGoldStackIntoCursorData::save(Serial::Saver& saver)
    {
        saver.save(invX);
        saver.save(invY);
        saver.save(splitCount);
    }

    void PlayerInput::SplitGoldStackIntoCursorData::load(Serial::Loader& loader)
    {
        invX = loader.load<int32_t>();
        invY = loader.load<int32_t>();
        splitCount = loader.load<int32_t>();
    }

    void PlayerInput::PlayerJoinedData::save(Serial::Saver& saver) { saver.save(peerId); }

    void PlayerInput::PlayerJoinedData::load(Serial::Loader& loader) { peerId = loader.load<uint32_t>(); }

    void PlayerInput::BuyItemData::save(Serial::Saver& saver)
    {
        saver.save(itemId);
        saver.save(shopkeeperId);
    }

    void PlayerInput::BuyItemData::load(Serial::Loader& loader)
    {
        itemId = loader.load<uint32_t>();
        shopkeeperId = loader.load<int32_t>();
    }
}
