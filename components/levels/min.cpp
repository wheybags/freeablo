#include "min.h"

#include <stdio.h>
#include <iostream>

#include <faio/faio.h>

MinFile::MinFile(const std::string& filename)
{

    FAIO::FAFile* minF = FAIO::FAfopen(filename);
    size_t numPillars = FAIO::FAsize(minF)/16; 
    
    FAIO::FAfseek(minF, 0, SEEK_SET);
    
    MinPillar temp(16);
    
    for(size_t i = 0; i < numPillars; i++)
    {
        FAIO::FAfread(&temp[0], 2, 16, minF);
        mPillars.push_back(temp);
    }
    
    FAIO::FAfclose(minF);
}
     
const MinPillar& MinFile::operator[] (size_t index) const
{
    return mPillars[index];
}

size_t MinFile::size()
{
    return mPillars.size();
}

