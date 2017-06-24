#ifndef W_MONSTER_H
#define W_MONSTER_H

#include "actor.h"
#include "ai.h"

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
            Monster(const DiabloExe::Monster& monster, Position pos);
            ~Monster();
            void init();
            std::string getDieWav();
            std::string getHitWav();
            void attachAI(AI* ai) {
              mAi = ai;
            };

            bool attack(Actor * enemy);
            void update(bool noclip, size_t ticksPassed);
            bool canIAttack(Actor * actor);

        private:
            AI * mAi;
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
