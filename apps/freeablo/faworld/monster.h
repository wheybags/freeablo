#ifndef W_MONSTER_H
#define W_MONSTER_H

#include "actor.h"

namespace DiabloExe
{
    struct Monster;
}

namespace FAWorld
{
    class Monster: public Actor
    {
        STATIC_NET_OBJECT_SET_CLASS_ID(1)

        public:
            Monster();
            Monster(const DiabloExe::Monster& monster, Position pos, ActorStats * stats=nullptr);
            void init();
            std::string getDieWav();
            std::string getHitWav();

        private:
            std::string mAnimPath;
            std::string mSoundPath;
    };
}

#endif     
