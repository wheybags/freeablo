#include <random>
#include <functional>

namespace FALevelGen
{
    std::mt19937 rng;

    void FAsrand(int seed)
    {
        rng.seed(seed);
    }
    
    int normRand(int min, int max)
    {
        std::normal_distribution<> nd(min, (float)(max-min)/3.5);

        int result;
        do
        {
            result = nd(rng);
        }
        while(result < min || result > max);

        return result;
    }

    int randomInRange (unsigned int min, unsigned int max)
    {
        std::uniform_int_distribution<> range(min, max);
        int result = range(rng);
        return result;
    }

    bool percentageChance(double chance)
    {
        std::bernoulli_distribution percent_chance(chance/100.0);
        return percent_chance(rng);
    }
}
