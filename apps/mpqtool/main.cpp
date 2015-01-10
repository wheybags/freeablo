
#include <iostream>

#include <stdint.h>

#include <stdio.h>

#include <faio/faio.h>

int main(int, char** argv)
{
    FAIO::init();

    FAIO::FAFile* file = FAIO::FAfopen(argv[1]);
    FILE* output = fopen(argv[2], "w");
    
    int read = 1;
    uint8_t buffer[1024];

    while(read > 0)
    {
        read = FAIO::FAfread(buffer, 1, sizeof(buffer), file);
        fwrite(buffer, 1, read, output);
    }

    fclose(output);

    FAIO::quit();
    return 0;
}
