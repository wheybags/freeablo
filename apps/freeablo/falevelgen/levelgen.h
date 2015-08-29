#ifndef LEVELGEN_H
#define LEVELGEN_H

#include <level/level.h>

namespace DiabloExe
{
    class DiabloExe;
}

namespace FALevelGen
{
  
    Level::Level generate(size_t width, size_t height, size_t dLvl, const DiabloExe::DiabloExe& exe, size_t previous, size_t next);

}

#endif
