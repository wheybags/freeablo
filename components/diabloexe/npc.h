#ifndef EXE_NPC_H
#define EXE_NPC_H

#include <boost/optional/optional.hpp>
#include <faio/fafileobject.h>

#include <unordered_map>

namespace DiabloExe
{
    class Npc
    {
    public:
        std::string id;
        std::string name;
        std::string celPath;
        uint8_t x;
        uint8_t y;
        size_t rotation;
        boost::optional<int32_t> animationSequenceId;
        std::unordered_map<std::string, std::string> talkData;

        Npc() {}

        std::string dump() const;

    private:
        Npc(FAIO::FAFileObject& exe, const std::string& id, size_t nameAdr, size_t celAdr, size_t xAdr, size_t yAdr, size_t _rotation);

        friend class DiabloExe;
    };
}

#endif
