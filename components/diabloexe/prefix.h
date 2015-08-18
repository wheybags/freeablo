#ifndef PREFIX_H
#define PREFIX_H
#include <stdint.h>

#include <string>
#include <map>
#include <faio/faio.h>



namespace DiabloExe
{
    struct Prefix
    {
    public:

        std::string prefixName;

        uint32_t prefexEffect;
        uint32_t minEffect;
        uint32_t maxEffect;
        uint32_t qualLevel;
        uint8_t  bowjewelProb;
        uint8_t  wsProb;
        uint8_t  asProb;
        uint8_t  unknown0;
        uint32_t excludedCombination0;
        uint32_t excludedCombination1;
        uint32_t cursed;
        uint32_t minGold;
        uint32_t maxGold;
        uint32_t multiplier;


        std::string dump() const;
        Prefix(){}

    private:
        Prefix(FAIO::FAFile* exe, size_t codeOffset);
        friend class DiabloExe;


    };



}
















#endif // PREFIX_H
