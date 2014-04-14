#ifndef CEL_DECODING_H
#define CEL_DECODING_H

#include <vector>
#include <stdint.h>

#include "pal.h"

namespace Cel
{
    int32_t normalDecode(const std::vector<uint8_t>& frame, const Pal& pal, std::vector<Colour>& raw_image, bool tileCel = false);
}

#endif
