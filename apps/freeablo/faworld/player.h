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
            virtual ~Player();
            Inventory mInventory;
            void setSpriteClass(std::string className);
            bool attack(Actor * enemy);
            bool attack(Player * enemy);
            bool talk(Actor * actor);
            FARender::FASpriteGroup* getCurrentAnim();
            void updateSpriteFormatVars();

            virtual void setLevel(GameLevel* level);

            virtual size_t getBasePriority()
            {
                return 10;
            }

        private:
            // these "Fmt" vars are just used by getCurrentAnim
            std::string mFmtClassName;
            std::string mFmtClassCode;
            std::string mFmtArmourCode;
            std::string mFmtWeaponCode;
            bool mFmtInDungeon = false;

        friend class Inventory;
    };
}
#endif
