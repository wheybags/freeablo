#ifndef LEVELGEN_H
#define LEVELGEN_H

#include <level/level.h>

namespace DiabloExe
{
    class DiabloExe;
}

namespace FALevelGen
{
  
    Level::Level* generate(size_t width, size_t height, size_t levelNum, const DiabloExe::DiabloExe& exe, const std::string& celPath);

}

#endif
