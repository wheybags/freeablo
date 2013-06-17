#include "min.h"

#include <stdio.h>
#include <iostream>

MinFile::MinFile(const std::string& filename)
{

    FILE* minF = fopen(filename.c_str(), "rb");
    fseek(minF, 0, SEEK_END);
    size_t numPillars = ftell(minF)/16; 
    
    fseek(minF, 0, SEEK_SET);
    
    MinPillar temp(16);
    
    for(size_t i = 0; i < numPillars; i++)
    {
        fread(&temp[0], 2, 16, minF);
        mPillars.push_back(temp);
    }
    
    fclose(minF);
}
     
const MinPillar& MinFile::operator[] (size_t index) const
{
    return mPillars[index];
}

size_t MinFile::size()
{
    return mPillars.size();
}

