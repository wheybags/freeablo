#ifndef PLAYER_H
#define PLAYER_H
#include <boost/format.hpp>
#include "actor.h"
namespace FAWorld
{
    class Player: public Actor
    {
        public:
            Player();
            void setSpriteClass(std::string className);
            bool attack(Actor * enemy);
            bool attack(Player * enemy);
            FARender::FASpriteGroup getCurrentAnim();
        protected:
            std::string mAnimPathFormat = "plrgfx/%s";
            bool mIsAttacking = false;
        friend class Inventory;
    };
}
#endif
