#include <faio/faio.h>
#include <misc/stringops.h>
#include "celfile.h"

namespace Cel
{
    CelFile::CelFile(const std::string& filename) :
        mDecoder(filename)
    {}

    size_t CelFile::numFrames() const
    {
        return mDecoder.numFrames();
    }

    size_t CelFile::animLength() const
    {
        return mDecoder.animationLength();
    }

    CelFrame& CelFile::operator[] (size_t index)
    {
        return mDecoder[index];
    }
}
