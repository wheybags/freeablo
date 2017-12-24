
#pragma once

#include <boost/optional/optional.hpp>
#include <faio/fafileobject.h>

#include <unordered_map>

namespace DiabloExe
{
    class Npc
    {
    public:
        std::string id;   ///< An internal id used in freeablo to identify this npc. Pulled from teh exe ini file
        std::string name; ///< The npc's actual ingame name
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

