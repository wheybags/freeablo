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

    void Target::load(FASaveGame::GameLoader& loader)
    {
        mType = Type(loader.load<uint8_t>());

        switch (mType)
        {
            case Type::Actor:
            {
                int32_t targetId = loader.load<int32_t>();

                loader.addFunctionToRunAtEnd([this, targetId]() {
                    mData.actor = World::get()->getActorById(targetId);
                    release_assert(mData.actor);
                });
                break;
            }

            case Type::Item:
            {
                mData.item.action = ItemTarget::ActionType(loader.load<uint8_t>());
                Tile itemLocation(loader);
                GameLevel* level = loader.currentlyLoadingLevel;
                release_assert(level);

                loader.addFunctionToRunAtEnd([this, itemLocation, level]() {
                    mData.item.item = level->getItemMap().getItemAt(itemLocation);

                    // TODO: uncomment this assert when we have items on the ground
                    // actually being saved. Atm, the above call will always return null
                    // because there's never anything on the ground after loading a save.
                    // release_assert(mData.item.item);
                });

                break;
            }

            case Type::None:
                break;
        }
    }

    void Target::save(FASaveGame::GameSaver& saver)
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
                target.item->getTile().save(saver);
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

    void Target::clear() { mType = Type::None; }
}
