#include "target.h"
#include "../fasavegame/gameloader.h"
#include "actor.h"
#include <misc/assert.h>

namespace FAWorld
{
    Target::Target(Actor* target)
    {
        mType = Type::Actor;
        mData.actor = target;
    }

    Target::Target(const ItemTarget& target)
    {
        mType = Type::Item;
        mData.item = target;
    }

    Target::Target(const Misc::Point& target)
    {
        mType = Type::Door;
        mData.doorPosition = target;
    }

    void Target::load(FASaveGame::GameLoader& loader)
    {
        mType = Type(loader.load<uint8_t>());

        switch (mType)
        {
            case Type::Actor:
            {
                int32_t targetId = loader.load<int32_t>();

                World* world = loader.currentlyLoadingWorld;
                loader.addFunctionToRunAtEnd([this, targetId, world]() {
                    mData.actor = world->getActorById(targetId);
                    release_assert(mData.actor);
                });
                break;
            }

            case Type::Item:
            {
                mData.item.action = ItemTarget::ActionType(loader.load<uint8_t>());
                mData.item.itemLocation = Misc::Point(loader);
                break;
            }

            case Type::Door:
            {
                mData.doorPosition = Misc::Point(loader);
                break;
            }

            case Type::None:
                break;
        }
    }

    void Target::save(FASaveGame::GameSaver& saver) const
    {
        saver.save(uint8_t(mType));

        switch (mType)
        {
            case Type::Actor:
            {
                saver.save(get<Actor*>()->getId());
                break;
            }

            case Type::Item:
            {
                ItemTarget target = get<ItemTarget>();
                saver.save(uint8_t(target.action));
                target.itemLocation.save(saver);
                break;
            }

            case Type::Door:
            {
                get<Misc::Point>().save(saver);
                break;
            }

            case Type::None:
                break;
        }
    }

    template <> Actor* Target::get<Actor*>() const
    {
        debug_assert(mType == Type::Actor);
        return mData.actor;
    }

    template <> Target::ItemTarget Target::get<Target::ItemTarget>() const
    {
        debug_assert(mType == Type::Item);
        return mData.item;
    }

    template <> Misc::Point Target::get<Misc::Point>() const
    {
        debug_assert(mType == Type::Door);
        return mData.doorPosition;
    }

    void Target::clear() { mType = Type::None; }
}
