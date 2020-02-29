#define KEEP_CALL_ON_ALL_PLAYER_INPUTS
#include "playerinput.h"
#undef KEEP_CALL_ON_ALL_PLAYER_INPUTS
#include <misc/assert.h>
#include <serial/loader.h>
#include <serial/streaminterface.h>
#include <set>

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

    void PlayerInput::save(Serial::Saver& saver) const
    {
        size_t start = saver.getStream().getCurrentSize();

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

        size_t inputSize = saver.getStream().getCurrentSize() - start;
        if (inputSize > MAX_SERIALISED_INPUT_SIZE)
            message_and_abort_fmt("Found input of type %d whose size (%d) exceeds MAX_SERIALISED_INPUT_SIZE", int(mType), int(inputSize));
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

    void PlayerInput::TargetTileData::save(Serial::Saver& saver) const
    {
        saver.save(x);
        saver.save(y);
    }

    void PlayerInput::TargetTileData::load(Serial::Loader& loader)
    {
        x = loader.load<int32_t>();
        y = loader.load<int32_t>();
    }

    void PlayerInput::DragOverTileData::save(Serial::Saver& saver) const
    {
        saver.save(x);
        saver.save(y);
    }

    void PlayerInput::DragOverTileData::load(Serial::Loader& loader)
    {
        x = loader.load<int32_t>();
        y = loader.load<int32_t>();
    }

    void PlayerInput::TargetActorData::save(Serial::Saver& saver) const { saver.save(actorId); }

    void PlayerInput::TargetActorData::load(Serial::Loader& loader) { actorId = loader.load<int32_t>(); }

    void PlayerInput::TargetItemOnFloorData::save(Serial::Saver& saver) const
    {
        position.save(saver);
        saver.save(uint8_t(type));
    }

    void PlayerInput::TargetItemOnFloorData::load(Serial::Loader& loader)
    {
        position = Misc::Point(loader);
        type = Target::ItemTarget::ActionType(loader.load<uint8_t>());
    }

    void PlayerInput::ForceAttackData::save(Serial::Saver& saver) const { pos.save(saver); }

    void PlayerInput::ForceAttackData::load(Serial::Loader& loader) { pos = Misc::Point(loader); }

    void PlayerInput::CastSpellData::save(Serial::Saver& saver) const
    {
        saver.save(x);
        saver.save(y);
    }

    void PlayerInput::CastSpellData::load(Serial::Loader& loader)
    {
        x = loader.load<int32_t>();
        y = loader.load<int32_t>();
    }

    void PlayerInput::ChangeLevelData::save(Serial::Saver& saver) const { saver.save(uint8_t(direction)); }

    void PlayerInput::ChangeLevelData::load(Serial::Loader& loader) { direction = Direction(loader.load<uint8_t>()); }

    void PlayerInput::InventorySlotClickedData::save(Serial::Saver& saver) const { slot.save(saver); }

    void PlayerInput::InventorySlotClickedData::load(Serial::Loader& loader) { slot.load(loader); }

    void PlayerInput::SetActiveSpellData::save(Serial::Saver& saver) const { saver.save((int32_t)spell); }

    void PlayerInput::SetActiveSpellData::load(Serial::Loader& loader) { spell = (SpellId)loader.load<int32_t>(); }

    void PlayerInput::ConfigureSpellHotkeyData::save(Serial::Saver& saver) const
    {
        saver.save(hotkey);
        saver.save((int32_t)spell);
    }

    void PlayerInput::ConfigureSpellHotkeyData::load(Serial::Loader& loader)
    {
        hotkey = loader.load<int32_t>();
        spell = (SpellId)loader.load<int32_t>();
    }

    void PlayerInput::SpellHotkeyData::save(Serial::Saver& saver) const { saver.save(hotkey); }

    void PlayerInput::SpellHotkeyData::load(Serial::Loader& loader) { hotkey = loader.load<int32_t>(); }

    void PlayerInput::SplitGoldStackIntoCursorData::save(Serial::Saver& saver) const
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

    void PlayerInput::PlayerJoinedData::save(Serial::Saver& saver) const { saver.save(peerId); }

    void PlayerInput::PlayerJoinedData::load(Serial::Loader& loader) { peerId = loader.load<uint32_t>(); }

    void PlayerInput::BuyItemData::save(Serial::Saver& saver) const
    {
        saver.save(itemId);
        saver.save(shopkeeperId);
    }

    void PlayerInput::BuyItemData::load(Serial::Loader& loader)
    {
        itemId = loader.load<uint32_t>();
        shopkeeperId = loader.load<int32_t>();
    }

    void PlayerInput::SellItemData::save(Serial::Saver& saver) const
    {
        itemLocation.save(saver);
        saver.save(shopkeeperId);
    }

    void PlayerInput::SellItemData::load(Serial::Loader& loader)
    {
        itemLocation.load(loader);
        shopkeeperId = loader.load<int32_t>();
    }

    void PlayerInput::removeUnnecessaryInputs(std::vector<PlayerInput>& inputs)
    {
        // This should remove all but the last of each input type, per player.
        // The idea is that if we are spam clicking on some spot on the ground, then we don't need to
        // process all those click events - just the last one.

        std::set<std::pair<int32_t, Type>> seenInputs;

        for (int32_t i = int32_t(inputs.size()) - 1; i >= 0; i--)
        {
            auto& input = inputs[i];

            if (seenInputs.count({input.mActorId, input.mType}))
                inputs.erase(inputs.begin() + i);

            seenInputs.insert({input.mActorId, input.mType});
        }
    }
}
