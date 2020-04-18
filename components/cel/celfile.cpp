#include "celfile.h"
#include <faio/faio.h>
#include <misc/stringops.h>

namespace Cel
{
    CelFile::CelFile(const std::string& filename) : mDecoder(filename) {}

    int32_t CelFile::numFrames() const { return mDecoder.numFrames(); }

    int32_t CelFile::animLength() const { return mDecoder.animationLength(); }

    std::vector<Image> CelFile::decode() { return mDecoder.decode(); }
}
