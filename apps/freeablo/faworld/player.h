#ifndef PLAYER_H
#define PLAYER_H
#include <boost/format.hpp>
#include "actor.h"
#include "inventory.h"
namespace FAWorld
{
    class Player: public Actor
    {
        STATIC_HANDLE_NET_OBJECT_IN_CLASS()

        public:
            Player();
            Player(const std::string& className, const DiabloExe::CharacterStats& charStats);
            virtual ~Player();
            void setSpriteClass(std::string className);
            bool talk(Actor * actor);
            void updateSprites();
            void pickupItem(ItemTarget target) override;
        bool dropItem(const FAWorld::Tile& clickedTile);

        virtual size_t getBasePriority()
            {
                return 10;
            }

            const Inventory &getInventory () const { return mInventory; }
            Inventory &getInventory () { return mInventory; }

        private:
            void init(const std::string& className, const DiabloExe::CharacterStats& charStats);

            std::string mClassName;
            Inventory mInventory;

            friend class Inventory;

            template <class Stream>
            Serial::Error::Error faSerial(Stream& stream)
            {
                serialise_as_parent_class(Actor);
                return Serial::Error::Success;
            }

            friend class Serial::WriteBitStream;
            friend class Serial::ReadBitStream;
    };
}
#endif
