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

    ///< This is a really shit RNG but it is guaranteed to give the same results across multiple machines
    class RandLCG
    {
    public:
        RandLCG(int32_t seed) :mSeed(seed) {}

        int32_t get()
        {
            mSeed = (22695477 * mSeed + 1) % 4294967296;
            return mSeed;
        }

    private:

        int32_t mSeed;

    };
}

#endif
