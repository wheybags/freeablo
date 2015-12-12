#ifndef LEVELGEN_H
#define LEVELGEN_H

#include "../faworld/gamelevel.h"

namespace DiabloExe
{
    class DiabloExe;
}

namespace FALevelGen
{
  
    FAWorld::GameLevel* generate(size_t width, size_t height, size_t dLvl, const DiabloExe::DiabloExe& exe, size_t previous, size_t next);

}

#endif
