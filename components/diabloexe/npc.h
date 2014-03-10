#ifndef EXE_NPC_H
#define EXE_NPC_H

#include <faio/faio.h>

namespace DiabloExe
{
    class Npc
    {
        public:
            std::string name;
            std::string celPath;
            uint8_t x;
            uint8_t y;

            Npc() {}

            std::string dump() const;

       private:
            Npc(FAIO::FAFile* exe, size_t nameAdr, size_t celAdr, size_t xAdr, size_t yAdr);

            friend class DiabloExe;
    };
}

#endif
