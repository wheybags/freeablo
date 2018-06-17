#pragma once
#include <cstdint>
#include <initializer_list>
#include <misc/assert.h>
#include <random>
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
        virtual void save(Serial::Saver& saver) = 0;

        template <typename T> T chooseOne(std::initializer_list<T> parameters)
        {
            int32_t n = randomInRange(0, parameters.size() - 1);
            return *(parameters.begin() + n);
        }
    };

    class RngMersenneTwister : public Rng
    {
    public:
        explicit RngMersenneTwister() : mRng(0U) {}
        explicit RngMersenneTwister(uint32_t seed) : mRng(seed) {}
        virtual ~RngMersenneTwister() override = default;
        RngMersenneTwister(const RngMersenneTwister&) = delete;

        virtual void load(Serial::Loader& loader) override;
        virtual void save(Serial::Saver& saver) override;

        virtual int32_t squaredRand(int32_t min, int32_t max) override;
        virtual int32_t randomInRange(int32_t min, int32_t max) override;

    private:
        std::mt19937 mRng;
    };

    class DummyRng : public Rng
    {
    public:
        virtual ~DummyRng() override = default;

        virtual int32_t squaredRand(int32_t min, int32_t max) override { return min + (max - min) / 2; }
        virtual int32_t randomInRange(int32_t min, int32_t max) override { return min + (max - min) / 2; }

        virtual void load(Serial::Loader&) override { message_and_abort("cannot load DummRng"); }
        virtual void save(Serial::Saver&) override { message_and_abort("cannot save DummRng"); }

        static DummyRng instance;

    private:
        DummyRng() = default;
    };
}
