
#pragma once

#include "actor.h"
#include "inventory.h"
#include <boost/format.hpp>

#include <boost/signals2/signal.hpp>

namespace FAWorld
{
    class Player : public Actor
    {
    public:
        static const std::string typeId;
        const std::string& getTypeId() override { return typeId; }

        Player();
        Player(const std::string& className, const DiabloExe::CharacterStats& charStats);
        void initCommon();
        Player(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver);

        virtual ~Player();
        void setSpriteClass(std::string className);
        bool talk(Actor* actor);
        void updateSprites();
        void pickupItem(ItemTarget target) override;
        bool dropItem(const FAWorld::Tile& clickedTile);

        virtual size_t getBasePriority() { return 10; }

        const Inventory& getInventory() const { return mInventory; }
        Inventory& getInventory() { return mInventory; }

    private:
        void init(const std::string& className, const DiabloExe::CharacterStats& charStats);

        std::string mClassName;
        Inventory mInventory; // TODO: this should be a member of Actor, not Player

        friend class Inventory;

    public:
        boost::signals2::signal<void(Actor*)> talkRequested;
    };
}
