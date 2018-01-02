
#pragma once

#include <cstdint>

namespace FASaveGame
{
    class GameSaver;
    class GameLoader;
}

namespace FAWorld
{
    class Actor;
    class PlacedItemData;

    class Target
    {
    public:
        enum class Type : uint8_t
        {
            None,
            Actor,
            Item,
        };

        class ItemTarget
        {
        public:
            enum class ActionType : uint8_t
            {
                autoEquip,
                toCursor,
            };
            ActionType action;
            PlacedItemData* item;
        };

        Target(const ItemTarget& target);
        Target(Actor* target);
        Target() = default;

        void load(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver);

        void clear();

        Type getType() const { return mType; }
        template <typename T> T get() const;

    private:
        union
        {
            Actor* actor;
            ItemTarget item;
        } mData;

        Type mType = Type::None;
    };

    template <> Actor* Target::get<Actor*>() const;
    template <> Target::ItemTarget Target::get<Target::ItemTarget>() const;
}
