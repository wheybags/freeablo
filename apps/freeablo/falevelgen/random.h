#ifndef FA_RANDOM_H
#define FA_RANDOM_H
#include <vector>
#include <initializer_list>
namespace FALevelGen
{
    void FAsrand(int seed);
    int normRand(int min, int max);
    int randomInRange (unsigned int min, unsigned int max);


    template<typename T>
    T chooseOne(std::initializer_list<T> parameters)
    {
        int n = randomInRange(0, parameters.size()-1);
        return *(parameters.begin() + n);
    }
}

#endif
