#include "pal.h"
#include <faio/fafileobject.h>

namespace Cel
{
    Pal::Pal() { contents.resize(256); }

    Pal::Pal(const std::string& filename) : Pal()
    {
        FAIO::FAFileObject pal_file(filename);

        for (int i = 0; i < 256; i++)
        {
            pal_file.FAfread(&contents[i].r, 1, 1);
            pal_file.FAfread(&contents[i].g, 1, 1);
            pal_file.FAfread(&contents[i].b, 1, 1);
            contents[i].a = 255;
        }
    }

    const Colour& Pal::operator[](size_t index) const { return contents[index]; }
}
