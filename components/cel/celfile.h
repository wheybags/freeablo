#pragma once
#include "celdecoder.h"
#include "celframe.h"
#include <stdint.h>

namespace FAIO
{
    struct FAFile;
}

namespace Cel
{
    class CelFile
    {
    public:
        CelFile(const std::string& filename);

        // If normal cel file, returns same as numFrames(), for an archive, the number of frames in each subcel
        int32_t animLength() const;
        int32_t numFrames() const;

        std::vector<Image> decode();

    private:
        CelDecoder mDecoder;
    };
}
