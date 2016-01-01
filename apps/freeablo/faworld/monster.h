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
        STATIC_HANDLE_NET_OBJECT_IN_CLASS()

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
