#ifndef PLAYER_H
#define PLAYER_H
#include <boost/format.hpp>
#include "actor.h"
#include "monster.h"
#include "inventory.h"
#include "characterstats.h"
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
            FARender::FASpriteGroup getCurrentAnim();
            void updateSprite();
            CharacterStatsBase * mStats=nullptr;
            bool mInDungeon = false;
            void setStats(CharacterStatsBase * stats){mStats = stats;}
            void takeDamage(double amount);
            std::string getClassName();
        private:
            std::string mClassName;
            std::string mClassCode;
            std::string mArmourCode;
            std::string mWeaponCode;
        friend class Inventory;
    };
}
#endif
