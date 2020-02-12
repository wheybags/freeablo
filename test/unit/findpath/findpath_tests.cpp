#include "drawpath.h"
#include "levelimplstub.h"
#include <algorithm>
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
    size_t expectedStepsAmount;
};

struct FindPathPatternsTest : ::testing::TestWithParam<FindPathPatternsParams>
{
    std::unique_ptr<FAWorld::LevelImplStub> level;
    bool isReachable = false;

    void SetUp() override { level = std::make_unique<FAWorld::LevelImplStub>(GetParam().map); }
};

TEST_P(FindPathPatternsTest, goalIsReachable)
{
    FAWorld::pathFind(level.get(), nullptr, GetParam().start, GetParam().goal, isReachable, false);

    ASSERT_TRUE(isReachable);
}

TEST_P(FindPathPatternsTest, pathBeginsNearStartPosition)
{
    auto path = FAWorld::pathFind(level.get(), nullptr, GetParam().start, GetParam().goal, isReachable, false);
    auto expected = FAWorld::neighbors(level.get(), nullptr, GetParam().start);

    ASSERT_NE(std::find(expected.begin(), expected.end(), path.front()), expected.end());
}

TEST_P(FindPathPatternsTest, pathEndsNearGoalPosition)
{
    auto path = FAWorld::pathFind(level.get(), nullptr, GetParam().start, GetParam().goal, isReachable, false);
    auto expected = FAWorld::neighbors(level.get(), nullptr, GetParam().goal);

    ASSERT_NE(std::find(expected.begin(), expected.end(), path.back()), expected.end());
}

TEST_P(FindPathPatternsTest, pathIsContinuous)
{
    const auto path = FAWorld::pathFind(level.get(), nullptr, GetParam().start, GetParam().goal, isReachable, false);

    for (size_t index = 0; index < path.size() - 1; ++index)
    {
        auto possibleRoute = FAWorld::neighbors(level.get(), nullptr, path.at(index));

        ASSERT_NE(std::find(possibleRoute.begin(), possibleRoute.end(), path.at(index + 1)), possibleRoute.end());
    }
}

TEST_P(FindPathPatternsTest, pathIsShort)
{
    auto path = FAWorld::pathFind(level.get(), nullptr, GetParam().start, GetParam().goal, isReachable, false);

    if (path.size() > GetParam().expectedStepsAmount)
    {
        std::cout << GetParam().description << std::endl;
        test_utils::drawPath(*level, GetParam().start, path);
    }

    ASSERT_EQ(path.size(), GetParam().expectedStepsAmount);
}

TEST_P(FindPathPatternsTest, pathIsWalkable)
{
    auto path = FAWorld::pathFind(level.get(), nullptr, GetParam().start, GetParam().goal, isReachable, false);

    for (auto tile : path)
        ASSERT_TRUE(level->isPassable(tile, nullptr));
}

INSTANTIATE_TEST_SUITE_P(FindPathPatternsParams,
                         FindPathPatternsTest,
                         ::testing::Values(FindPathPatternsParams{"Walk #1", basicMap(), Point(2, 3), Point{18, 5}, 17},
                                           FindPathPatternsParams{"Walk #1 reverse", basicMap(), Point(18, 5), Point{2, 3}, 17},
                                           FindPathPatternsParams{"Walk #2", basicMap(), Point(3, 10), Point{18, 10}, 19},
                                           FindPathPatternsParams{"Walk #3", basicMap(), Point(3, 10), Point{15, 10}, 19},
                                           FindPathPatternsParams{"Goal point is not passable #1", basicMap(), Point(18, 7), Point{10, 10}, 13},
                                           FindPathPatternsParams{"Goal point is not passable #2", basicMap(), Point(18, 10), Point{10, 10}, 14}));

TEST(FindPathTests, walksStraight_whenSomeWallsInTheMiddle)
{
    const Point start{0, 1};
    const Point goal{14, 1};
    FAWorld::LevelImplStub level(Map{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
                                     {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, //
                                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}});
    const Points restrictedPositions{{2, 1}, {4, 1}, {6, 1}, {8, 1}, {10, 1}, {12, 1}};

    bool isReachable = false;
    auto path = FAWorld::pathFind(&level, nullptr, start, goal, isReachable, false);

    ASSERT_EQ(path.size(), 15);

    for (const auto& restrictedPosition : restrictedPositions)
        ASSERT_TRUE(std::end(path) == std::find(path.begin(), path.end(), restrictedPosition));
}

TEST(FindPathTests, walksStraight_vertically)
{
    FAWorld::LevelImplStub level(Map{{0, 0, 0, 0, 0}, //
                                     {0, 0, 0, 0, 0}, //
                                     {0, 0, 0, 0, 0}, //
                                     {0, 0, 0, 0, 0}, //
                                     {0, 0, 0, 0, 0}, //
                                     {0, 0, 0, 0, 0}});
    const Point start{2, 0};
    const Point goal{2, 5};
    const Points expected{{2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {2, 5}};

    bool isReachable = false;
    auto actual = FAWorld::pathFind(&level, nullptr, start, goal, isReachable, false);

    ASSERT_EQ(actual, expected);
}

TEST(FindPathTests, walksOnLargeMap)
{
    const size_t map_size = 500;
    const Point start{0, 0};
    const Point goal{map_size - 1, map_size - 1};

    Map map{map_size, std::vector<int>(map_size, 0)};

    FAWorld::LevelImplStub level(map);

    bool isReachable = false;
    auto path = FAWorld::pathFind(&level, nullptr, start, goal, isReachable, false);

    ASSERT_EQ(path.size(), map_size);
}

TEST(FindPathTests, pathfinderIsLimited)
{
    const size_t map_size = 5000;
    const Point start{0, 0};
    const Point goal{map_size - 1, map_size - 1};

    Map map{map_size, std::vector<int>(map_size, 0)};

    FAWorld::LevelImplStub level(map);

    bool isReachable = false;
    auto path = FAWorld::pathFind(&level, nullptr, start, goal, isReachable, false);

    EXPECT_EQ(path.size(), 0);
    ASSERT_FALSE(isReachable);
}
