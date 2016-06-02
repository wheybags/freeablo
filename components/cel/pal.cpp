#include "pal.h"

#include <stdio.h>

#include <faio/faio.h>

namespace Cel
{
    Pal::Pal()
    {
        contents.resize(256);
    }

    Pal::Pal(const std::string& filename) : Pal()
    {
        FAIO::FAFile * pal_file;

        pal_file = FAIO::FAfopen(filename);
        
        for(int i = 0; i < 256; i++)
        {
                FAIO::FAfread(&contents[i].r, 1, 1, pal_file);
                FAIO::FAfread(&contents[i].g, 1, 1, pal_file);
                FAIO::FAfread(&contents[i].b, 1, 1, pal_file);
        }

        FAIO::FAfclose(pal_file);
    }

    const Colour& Pal::operator[](size_t index) const
    {
        return contents[index];
    }
}
