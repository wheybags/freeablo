#include "npc.h"

#include <sstream>

namespace DiabloExe
{
    Npc::Npc(FAIO::FAFile* exe, size_t nameAdr, size_t celAdr, size_t xAdr, size_t yAdr)
    {
        name = FAIO::readCString(exe, nameAdr);
        celPath = FAIO::readCString(exe, celAdr);

        FAIO::FAfseek(exe, xAdr, SEEK_SET);
        FAIO::FAfread(&x, 1, 1, exe);

        FAIO::FAfseek(exe, yAdr, SEEK_SET);
        FAIO::FAfread(&y, 1, 1, exe);
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
