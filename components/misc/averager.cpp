#include "averager.h"

namespace Misc
{
    double Averager::getAverage(const std::string& name, double currentValue)
    {
        mRunningAverageValues[name] -= mRunningAverageValues[name] / AVERAGE_SAMPLES;
        mRunningAverageValues[name] += currentValue / AVERAGE_SAMPLES;

        return mRunningAverageValues[name];
    }
}