#ifndef CL2_DECODING_H
#define CL2_DECODING_H

#include <vector>
#include <stdint.h>

#include "pal.h"

namespace Cel
{
    int32_t cl2Decode(const std::vector<uint8_t>& frame, const Pal& pal, std::vector<Colour>& rawImage);
}

#endif
