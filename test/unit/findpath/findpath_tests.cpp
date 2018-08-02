#include "drawpath.h"
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
        return {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}, //
                {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
    }
}

struct FindPathPatternsParams
{
    std::string description;
    Map map;
    Point start;
    Point goal;
    Points expected;
};

struct FindPathPatternsTest : ::testing::TestWithParam<FindPathPatternsParams>
{
};

TEST_P(FindPathPatternsTest, sample)
{
    FAWorld::LevelImplStub level(GetParam().map);

    bool isReachable = false;
    auto points = FAWorld::pathFind(&level, GetParam().start, GetParam().goal, isReachable, false);

    if (!isReachable || points != GetParam().expected)
    {
        std::cout << GetParam().description << std::endl;
        test_utils::drawPath(level, GetParam().start, points, GetParam().expected);
    }

    ASSERT_TRUE(isReachable);
    ASSERT_EQ(points, GetParam().expected);
}

INSTANTIATE_TEST_CASE_P(
    FindPathPatternsParams,
    FindPathPatternsTest,
    ::testing::Values(
        FindPathPatternsParams{
            "Walk #1",
            basicMap(),
            Point(2, 3),
            Point{18, 5},
            Points{{3, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3}, {8, 3}, {9, 3}, {10, 3}, {11, 3}, {12, 3}, {13, 3}, {14, 2}, {15, 3}, {16, 4}, {17, 5}, {18, 5}}},
        FindPathPatternsParams{
            "Walk #1 reverse",
            basicMap(),
            Point(18, 5),
            Point{2, 3},
            Points{{17, 4}, {16, 3}, {15, 3}, {14, 2}, {13, 3}, {12, 3}, {11, 3}, {10, 3}, {9, 3}, {8, 3}, {7, 3}, {6, 3}, {5, 3}, {4, 3}, {3, 3}, {2, 3}}},
        FindPathPatternsParams{"Walk #2",
                               basicMap(),
                               Point(3, 10),
                               Point{18, 10},
                               Points{{4, 10},
                                      {5, 10},
                                      {6, 11},
                                      {7, 12},
                                      {8, 13},
                                      {9, 14},
                                      {10, 15},
                                      {11, 15},
                                      {12, 15},
                                      {13, 16},
                                      {14, 17},
                                      {15, 16},
                                      {16, 15},
                                      {17, 14},
                                      {18, 13},
                                      {18, 12},
                                      {18, 11},
                                      {18, 10}}},
        FindPathPatternsParams{"Walk #3",
                               basicMap(),
                               Point(3, 10),
                               Point{15, 10},
                               Points{{4, 10},
                                      {5, 10},
                                      {6, 11},
                                      {7, 12},
                                      {8, 13},
                                      {9, 14},
                                      {10, 15},
                                      {11, 15},
                                      {12, 15},
                                      {13, 16},
                                      {14, 17},
                                      {15, 16},
                                      {15, 15},
                                      {15, 14},
                                      {15, 13},
                                      {15, 12},
                                      {15, 11},
                                      {15, 10}}},
        FindPathPatternsParams{"Goal point is not passable #1",
                               basicMap(),
                               Point(18, 7),
                               Point{10, 10},
                               Points{{17, 6}, {16, 5}, {15, 4}, {15, 3}, {14, 2}, {13, 3}, {12, 4}, {11, 5}, {10, 6}, {10, 7}, {10, 8}, {10, 9}}},
        FindPathPatternsParams{
            "Goal point is not passable #2",
            basicMap(),
            Point(18, 10),
            Point{10, 10},
            Points{{17, 11}, {16, 12}, {15, 13}, {15, 14}, {15, 15}, {15, 16}, {14, 17}, {13, 16}, {12, 15}, {11, 14}, {11, 13}, {11, 12}, {11, 11}}}), );
