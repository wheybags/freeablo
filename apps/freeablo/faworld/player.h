#ifndef PLAYER_H
#define PLAYER_H
#include <boost/format.hpp>
#include "actor.h"
#include "inventory.h"
namespace FAWorld
{
    class Player: public Actor
    {
        public:
            Player();
            Inventory mInventory;
            void setSpriteClass(std::string className);
            bool attack(Actor * enemy);
            bool attack(Player * enemy);
            FARender::FASpriteGroup* getCurrentAnim();
            void updateSpriteFormatVars();

            virtual void setLevel(GameLevel* level);

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
