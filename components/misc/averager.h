#pragma once
#include <string>
#include <unordered_map>

namespace Misc
{
    class Averager
    {
    public:
        double getAverage(const std::string& name, double currentValue);

        constexpr static double AVERAGE_SAMPLES = 200;

    private:
        std::unordered_map<std::string, double> mRunningAverageValues;
    };
}