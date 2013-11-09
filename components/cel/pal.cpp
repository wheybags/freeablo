#include "pal.h"

#include <stdio.h>

#include <faio/faio.h>

Pal::Pal(std::string filename)
{
    contents.resize(256);

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

colour& Pal::operator[](size_t index)
{
    return contents[index];
}
