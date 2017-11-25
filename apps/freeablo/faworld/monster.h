#ifndef W_MONSTER_H
#define W_MONSTER_H

#include "actor.h"

namespace DiabloExe
{
    class  Monster;
}

namespace FAWorld
{
    class AI;
    class NullAI;

    class Monster: public Actor
    {
        STATIC_HANDLE_NET_OBJECT_IN_CLASS()

        public:
            static const std::string typeId;
            const std::string& getTypeId() override { return typeId; }

            Monster();
            Monster(const DiabloExe::Monster& monster);
            Monster(FASaveGame::GameLoader& loader);
            void save(FASaveGame::GameSaver& saver);

            ~Monster() {};
            void init();

            // TODO: these should be virtual methods on Actor
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
