#ifndef LEVELGEN_H
#define LEVELGEN_H

#include "../faworld/gamelevel.h"

namespace DiabloExe
{
    class DiabloExe;
}

namespace FALevelGen
{
  
    FAWorld::GameLevel* generate(int32_t width, int32_t height, int32_t dLvl, const DiabloExe::DiabloExe& exe, int32_t previous, int32_t next);

}

#endif
