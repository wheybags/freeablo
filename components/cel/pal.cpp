#include "pal.h"

#include <stdio.h>

Pal::Pal(std::string filename)
{
    contents.resize(256);

    FILE * pal_file;

    pal_file = fopen(filename.c_str(), "rb");
    
    for(int i = 0; i < 256; i++)
    {
            fread(&contents[i].r, 1, 1, pal_file);
            fread(&contents[i].g, 1, 1, pal_file);
            fread(&contents[i].b, 1, 1, pal_file);
    }

    fclose(pal_file);
}

colour& Pal::operator[](size_t index)
{
    return contents[index];
}
