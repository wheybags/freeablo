#ifndef DECODING_H
#define DECODING_H

#include <vector>
#include <stdint.h>
#include "pal.h"

bool isTileCel(const std::string& fileName);
size_t decodeTileFrame(const std::vector<uint8_t>& frame, const Pal& pal, std::vector<Colour>& rawImage);

#endif
