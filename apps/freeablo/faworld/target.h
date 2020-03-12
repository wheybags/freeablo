#pragma once
#include <cstdint>
#include <misc/simplevec2.h>

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
            Door,
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
            Misc::Point itemLocation;
        };

        Target(const ItemTarget& target);
        Target(Actor* target);
        Target(const Misc::Point& target);
        Target() = default;

        void load(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver) const;

        void clear();

        Type getType() const { return mType; }
        template <typename T> T get() const;

    private:
        union Data
        {
            Actor* actor;
            ItemTarget item;
            Misc::Point doorPosition;

            Data() {}
        } mData;

        Type mType = Type::None;
    };

    template <> Actor* Target::get<Actor*>() const;
    template <> Target::ItemTarget Target::get<Target::ItemTarget>() const;
    template <> Misc::Point Target::get<Misc::Point>() const;
}
