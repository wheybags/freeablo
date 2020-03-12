#pragma once
#include "mersennetwister.h"
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <misc/assert.h>
#include <vector>

namespace Serial
{
    class Loader;
    class Saver;
}

namespace Random
{
    class Rng
    {
    public:
        virtual ~Rng();

        virtual int32_t squaredRand(int32_t min, int32_t max) = 0;

        /// range is inclusive
        virtual int32_t randomInRange(int32_t min, int32_t max) = 0;

        virtual void load(Serial::Loader& loader) = 0;
        virtual void save(Serial::Saver& saver) const = 0;

        template <typename T> T chooseOne(std::initializer_list<T> parameters)
        {
            int32_t n = randomInRange(0, parameters.size() - 1);
            return *(parameters.begin() + n);
        }

        template <typename Iter> Iter chooseOneInContainer(Iter begin, Iter end)
        {
            int32_t n = randomInRange(0, std::distance(begin, end) - 1);
            std::advance(begin, n);
            return begin;
        }
    };

    class RngMersenneTwister : public Rng
    {
    public:
        explicit RngMersenneTwister() = default;
        explicit RngMersenneTwister(uint32_t seed) : mRng(seed) {}
        virtual ~RngMersenneTwister() override = default;
        RngMersenneTwister(const RngMersenneTwister&) = delete;

        virtual void load(Serial::Loader& loader) override;
        virtual void save(Serial::Saver& saver) const override;

        virtual int32_t squaredRand(int32_t min, int32_t max) override;
        virtual int32_t randomInRange(int32_t min, int32_t max) override;

    private:
        mt19937 mRng;
    };

    class DummyRng : public Rng
    {
    public:
        virtual ~DummyRng() override = default;

        virtual int32_t squaredRand(int32_t min, int32_t max) override { return min + (max - min) / 2; }
        virtual int32_t randomInRange(int32_t min, int32_t max) override { return min + (max - min) / 2; }

        virtual void load(Serial::Loader&) override { message_and_abort("cannot load DummRng"); }
        virtual void save(Serial::Saver&) const override { message_and_abort("cannot save DummRng"); }

        static DummyRng instance;

    private:
        DummyRng() = default;
    };
}
