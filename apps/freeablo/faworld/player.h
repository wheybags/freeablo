#ifndef PLAYER_H
#define PLAYER_H
#include <boost/format.hpp>
#include "actor.h"
#include "inventory.h"

#include <boost/signals2/signal.hpp>

namespace FAWorld
{
    class Player: public Actor
    {
        STATIC_HANDLE_NET_OBJECT_IN_CLASS()

        public:
            static const std::string typeId;
            const std::string& getTypeId() override { return typeId; }

            Player();
            Player(const std::string& className, const DiabloExe::CharacterStats& charStats);
            Player(FASaveGame::GameLoader& loader);
            void save(FASaveGame::GameSaver& saver);

            virtual ~Player();
            void setSpriteClass(std::string className);
            bool talk(Actor * actor);
            void updateSprites();
            void pickupItem(ItemTarget target) override;
            bool dropItem(const FAWorld::Tile& clickedTile);

            virtual size_t getBasePriority() { return 10; }

            const Inventory &getInventory () const { return mInventory; }
            Inventory &getInventory () { return mInventory; }

        private:
            void init(const std::string& className, const DiabloExe::CharacterStats& charStats);

            std::string mClassName;
            Inventory mInventory; //TODO: this should be a member of Actor, not Player

            friend class Inventory;

            template <class Stream>
            Serial::Error::Error faSerial(Stream& stream)
            {
                serialise_as_parent_class(Actor);
                return Serial::Error::Success;
            }

    public:
            boost::signals2::signal<void (Actor *)> talkRequested;
    private:
            friend class Serial::WriteBitStream;
            friend class Serial::ReadBitStream;
    };
}
#endif
