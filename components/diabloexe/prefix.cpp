#include "prefix.h"
#include <string>
#include <map>
#include <faio/faio.h>
#include <sstream>
namespace DiabloExe
{

    Prefix::Prefix(FAIO::FAFile *exe, size_t codeOffset)
    {
        uint32_t prefixNameTemp = FAIO::read32(exe);
        minEffect      = FAIO::read32(exe);
        maxEffect      = FAIO::read32(exe);
        qualLevel      = FAIO::read32(exe);

        bowjewelProb = FAIO::read8(exe);
        wsProb       = FAIO::read8(exe);
        asProb       = FAIO::read8(exe);
        unknown0     = FAIO::read8(exe);

        excludedCombination0 = FAIO::read32(exe);
        excludedCombination1 = FAIO::read32(exe);

        cursed     = FAIO::read32(exe);
        minGold    = FAIO::read32(exe);
        maxGold    = FAIO::read32(exe);
        multiplier = FAIO::read32(exe);
        prefixName = FAIO::readCString(exe, prefixNameTemp - codeOffset);



    }

    std::string Prefix::dump() const
    {
        std::stringstream ss;
        ss << "{" << std::endl
        << "\tprefixName: " << prefixName << std::endl
        << "\tminEffect: "  << (size_t)minEffect << "," << std::endl
        << "\tmaxEffect: "  << (size_t)maxEffect << "," << std::endl
        << "\tqualEffect: " << (size_t)qualLevel << "," << std::endl
        << "\tbowjewelProb: " << (size_t)bowjewelProb << "," << std::endl
        << "\twsProb: " << (size_t)wsProb << "," << std::endl
        << "\tasProb: " << (size_t)unknown0 << "," << std::endl
        << "\texcludedCombination0: " << (size_t)excludedCombination0 << "," << std::endl
        << "\texcludedCombination1: " << (size_t)excludedCombination1 << "," << std::endl
        << "\tcursed: " << (size_t)cursed << "," << std::endl
        << "\tminGold: " << (size_t)minGold << "," << std::endl
        << "\tmaxGold: " << (size_t)maxGold << "," << std::endl
        << "\tmultiplier: " << (size_t)multiplier << "," << std::endl
        << "}" << std::endl;

        return ss.str();

    }
}
