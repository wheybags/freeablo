#include <faio/faio.h>
#include <misc/stringops.h>
#include "celfile.h"

namespace Cel
{
    CelFile::CelFile(const std::string& filename) :
        mDecoder(filename)
    {}

    int32_t CelFile::numFrames() const
    {
        return mDecoder.numFrames();
    }

    int32_t CelFile::animLength() const
    {
        return mDecoder.animationLength();
    }

    CelFrame& CelFile::operator[] (int32_t index)
    {
        return mDecoder[index];
    }
}
