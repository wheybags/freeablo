#include "levelimplstub.h"
#include <faworld/findpath.h>
#include <gtest/gtest.h>

using Point = Misc::Point;
using Points = Misc::Points;

namespace
{
    using Map = std::vector<std::vector<int>>;

    Map basicMap()
    {
        return {{0, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0}};
    }
}

struct NeighborsPatternsParams
{
    Map map;
    Point position;
    Points expected;
};

struct NeighborsPatternsTest : ::testing::TestWithParam<NeighborsPatternsParams>
{
    std::unique_ptr<FAWorld::LevelImplStub> level;

    void SetUp() override { level = std::make_unique<FAWorld::LevelImplStub>(GetParam().map); }
};

TEST_P(NeighborsPatternsTest, equals)
{
    auto actual = FAWorld::neighbors(level.get(), nullptr, GetParam().position);
    auto expected = GetParam().expected;

    std::sort(actual.begin(), actual.end());
    std::sort(expected.begin(), expected.end());

    ASSERT_EQ(actual, expected);
}

INSTANTIATE_TEST_SUITE_P(FindPathPatternsParams,
                         NeighborsPatternsTest,
                         ::testing::Values(NeighborsPatternsParams{basicMap(), Point(-2, -2), Points{}},
                                           NeighborsPatternsParams{basicMap(), Point(0, 0), Points{{0, 0}, {1, 0}, {0, 1}, {1, 1}}},
                                           NeighborsPatternsParams{basicMap(), Point(5, 5), Points{{4, 4}, {5, 4}, {4, 5}, {5, 5}}},
                                           NeighborsPatternsParams{basicMap(), Point(6, 6), Points{{5, 5}}},
                                           NeighborsPatternsParams{
                                               basicMap(), Point(1, 1), Points{{0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1}, {2, 1}, {0, 2}, {1, 2}, {2, 2}}}));
