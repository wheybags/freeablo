#ifndef FA_RANDOM_H
#define FA_RANDOM_H
#include <vector>
#include <initializer_list>
namespace FALevelGen
{
    void FAsrand(int seed);
    int normRand(int min, int max);
    int randomInRange (unsigned int min, unsigned int max);
    bool percentageChance(double chance);


    template<typename T>
    T chooseOne(std::initializer_list<T> parameters)
    {
        std::vector<T> choices;
        choices.insert(choices.end(), parameters.begin(), parameters.end());
        int n = randomInRange(0, choices.size()-1);

        return choices[n];
    }


}

#endif
