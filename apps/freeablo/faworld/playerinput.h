#pragma once
#include "../faworld/equiptarget.h"
#include "../faworld/spellenums.h"
#include "target.h"
#include <cstdint>
#include <misc/direction.h>

#define CALL_ON_ALL_PLAYER_INPUTS(MACRO)                                                                                                                       \
    MACRO(TargetTile)                                                                                                                                          \
    MACRO(DragOverTile)                                                                                                                                        \
    MACRO(TargetActor)                                                                                                                                         \
    MACRO(TargetItemOnFloor)                                                                                                                                   \
    MACRO(ForceAttack)                                                                                                                                         \
    MACRO(CastSpell)                                                                                                                                           \
    MACRO(ChangeLevel)                                                                                                                                         \
    MACRO(InventorySlotClicked)                                                                                                                                \
    MACRO(SetActiveSpell)                                                                                                                                      \
    MACRO(ConfigureSpellHotkey)                                                                                                                                \
    MACRO(SpellHotkey)                                                                                                                                         \
    MACRO(SplitGoldStackIntoCursor)                                                                                                                            \
    MACRO(PlayerJoined)                                                                                                                                        \
    MACRO(PlayerLeft)                                                                                                                                          \
    MACRO(BuyItem)                                                                                                                                             \
    MACRO(SellItem)

namespace Serial
{
    class Loader;
    class Saver;
}

namespace FAWorld
{
    class PlayerInput
    {
    public:
        struct TargetTileData
        {
            int32_t x, y;

            void save(Serial::Saver& saver) const;
            void load(Serial::Loader& loader);
        };
        struct DragOverTileData
        {
            int32_t x, y;

            void save(Serial::Saver& saver) const;
            void load(Serial::Loader& loader);
        };
        struct TargetActorData
        {
            int32_t actorId;

            void save(Serial::Saver& saver) const;
            void load(Serial::Loader& loader);
        };
        struct TargetItemOnFloorData
        {
            Misc::Point position;
            Target::ItemTarget::ActionType type;

            void save(Serial::Saver& saver) const;
            void load(Serial::Loader& loader);
        };
        struct ForceAttackData
        {
            Misc::Point pos;

            void save(Serial::Saver& saver) const;
            void load(Serial::Loader& loader);
        };
        struct CastSpellData
        {
            int32_t x, y;

            void save(Serial::Saver& saver) const;
            void load(Serial::Loader& loader);
        };
        struct ChangeLevelData
        {
            enum class Direction : uint8_t
            {
                Up,
                Down
            };

            Direction direction;

            void save(Serial::Saver& saver) const;
            void load(Serial::Loader& loader);
        };
        struct InventorySlotClickedData
        {
            EquipTarget slot;

            void save(Serial::Saver& saver) const;
            void load(Serial::Loader& loader);
        };
        struct SetActiveSpellData
        {
            SpellId spell;

            void save(Serial::Saver& saver) const;
            void load(Serial::Loader& loader);
        };
        struct ConfigureSpellHotkeyData
        {
            int32_t hotkey;
            SpellId spell;

            void save(Serial::Saver& saver) const;
            void load(Serial::Loader& loader);
        };
        struct SpellHotkeyData
        {
            int32_t hotkey;

            void save(Serial::Saver& saver) const;
            void load(Serial::Loader& loader);
        };
        struct SplitGoldStackIntoCursorData
        {
            int32_t invX, invY;
            int32_t splitCount;

            void save(Serial::Saver& saver) const;
            void load(Serial::Loader& loader);
        };
        struct PlayerJoinedData
        {
            uint32_t peerId;

            void save(Serial::Saver& saver) const;
            void load(Serial::Loader& loader);
        };
        struct PlayerLeftData
        {
            void save(Serial::Saver&) const {}
            void load(Serial::Loader&) {}
        };
        struct BuyItemData
        {
            uint32_t itemId;
            int32_t shopkeeperId;

            void save(Serial::Saver& saver) const;
            void load(Serial::Loader& loader);
        };
        struct SellItemData
        {
            FAWorld::EquipTarget itemLocation;
            int32_t shopkeeperId;

            void save(Serial::Saver& saver) const;
            void load(Serial::Loader& loader);
        };

        // All this macro mess takes care of generating boilerplate code to wrap up the above structs into a union with a type enum, and
        // save/load functions. So, eg, if mType == TargetTile, then you can access mData.dataTargetType. You can also call .save() and .load()
        // and it will delegate to the appropriate save and load funcs from the structs above

#define GENERATE_ENUM_VALS(X) X,
        enum class Type : uint8_t
        {
            None,
            CALL_ON_ALL_PLAYER_INPUTS(GENERATE_ENUM_VALS)
        };
#undef GENERATE_ENUM_VALS

#define GENERATE_CONSTRUCTOR(X) PlayerInput(const X##Data& data, int32_t actorId);
        CALL_ON_ALL_PLAYER_INPUTS(GENERATE_CONSTRUCTOR)
#undef GENERATE_CONSTRUCTOR

        PlayerInput() = default;

        union DataUnionType
        {
#define GENERATE_UNION_MEMBER(X) X##Data data##X;
            CALL_ON_ALL_PLAYER_INPUTS(GENERATE_UNION_MEMBER)
#undef GENERATE_UNION_MEMBER

            DataUnionType() {}
        } mData;

        void save(Serial::Saver& saver) const;
        void load(Serial::Loader& loader);

        Type mType = Type::None;
        int32_t mActorId = -1;

        static void removeUnnecessaryInputs(std::vector<PlayerInput>& inputs);

        static constexpr int32_t MAX_SERIALISED_INPUT_SIZE = 50;
    };
}

#ifndef KEEP_CALL_ON_ALL_PLAYER_INPUTS
#undef CALL_ON_ALL_PLAYER_INPUTS
#endif
