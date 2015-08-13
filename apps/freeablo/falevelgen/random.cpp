#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

namespace FALevelGen
{
    boost::mt19937 rng;

    void FAsrand(int seed)
    {
        rng.seed(seed);
    }
    
    int normRand(int min, int max)
    {
        boost::normal_distribution<> nd(min, (float)(max-min)/3.5);

        boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > varNor(rng, nd);
        
        int result;
        
        do
        {
            result = (int)varNor();
        }
        while(result < min || result > max);

        return result;
    }

    int randomInRange (unsigned int min, unsigned int max)
    {
        boost::uniform_int<> range(min, max);
        boost::variate_generator<boost::mt19937&, boost::uniform_int<> > varRange(rng, range);
        
        return varRange();
    }
}
