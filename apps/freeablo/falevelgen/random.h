#ifndef FA_RANDOM_H
#define FA_RANDOM_H
#include <tuple>
namespace FALevelGen
{
    void FAsrand(int seed);
    int normRand(int min, int max);
    int randomInRange (unsigned int min, unsigned int max);

    template <typename ... Params>
    Params chooseOne(Params ... parameters)
    {
        std::tuple<Params...> choices=std::make_tuple(paramaters...);
        int n = randomInRange(0, std::tuple_size<decltype(choices)>::value-1);

        return choices[n];


    }
}

#endif
