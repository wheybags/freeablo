#ifndef W_MONSTER_H
#define W_MONSTER_H

#include "actor.h"

namespace DiabloExe
{
    struct Monster;
}

namespace FAWorld
{
    class AI;
    class NullAI;

    class Monster: public Actor
    {
        STATIC_HANDLE_NET_OBJECT_IN_CLASS()

        public:
            Monster();
            Monster(const DiabloExe::Monster& monster);
            ~Monster() {};
            void init();
            std::string getDieWav();
            std::string getHitWav();

        private:
            std::string mSoundPath;

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
