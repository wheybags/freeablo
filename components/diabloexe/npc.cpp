#include "npc.h"

#include <sstream>

namespace DiabloExe
{
    Npc::Npc(FAIO::FAFileObject& exe, const std::string& npcId, size_t nameAdr, size_t celAdr, size_t xAdr, size_t yAdr, size_t _rotation)
    {
        id = npcId;
        name = exe.readCString(nameAdr);
        celPath = exe.readCString(celAdr);

        exe.FAfseek(xAdr, SEEK_SET);
        exe.FAfread(&x, 1, 1);

        exe.FAfseek(yAdr, SEEK_SET);
        exe.FAfread(&y, 1, 1);

        rotation = _rotation;
    }
    
    std::string Npc::dump() const
    {
        std::stringstream ss;

        ss << "{" << std::endl <<
        "\tname: " << name << std::endl <<
        "\tcelPath: " << celPath << std::endl <<
        "\tx: " << (int)x << std::endl <<
        "\ty: " << (int)y << std::endl <<
        "}" << std::endl;

        return ss.str();
    }
}
