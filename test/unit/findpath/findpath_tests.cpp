#include "drawpath.h"
#include "levelimplstub.h"

#include <faworld/findpath.h>

#include <boost/make_unique.hpp>
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

    void SetUp() override { level = boost::make_unique<FAWorld::LevelImplStub>(GetParam().map); }
};

TEST_P(FindPathPatternsTest, goalIsReachable)
{
    FAWorld::pathFind(level.get(), GetParam().start, GetParam().goal, isReachable, false);

    ASSERT_TRUE(isReachable);
}

TEST_P(FindPathPatternsTest, pathBeginsNearStartPosition)
{
    auto path = FAWorld::pathFind(level.get(), GetParam().start, GetParam().goal, isReachable, false);
    auto expected = FAWorld::neighbors(level.get(), GetParam().start);

    ASSERT_NE(std::find(expected.begin(), expected.end(), path.front()), expected.end());
}

TEST_P(FindPathPatternsTest, pathEndsNearGoalPosition)
{
    auto path = FAWorld::pathFind(level.get(), GetParam().start, GetParam().goal, isReachable, false);
    auto expected = FAWorld::neighbors(level.get(), GetParam().goal);

    ASSERT_NE(std::find(expected.begin(), expected.end(), path.back()), expected.end());
}

TEST_P(FindPathPatternsTest, pathIsContinuous)
{
    const auto path = FAWorld::pathFind(level.get(), GetParam().start, GetParam().goal, isReachable, false);

    for (size_t index = 0; index < path.size() - 1; ++index)
    {
        auto possibleRoute = FAWorld::neighbors(level.get(), path.at(index));

        ASSERT_NE(std::find(possibleRoute.begin(), possibleRoute.end(), path.at(index + 1)), possibleRoute.end());
    }
}

TEST_P(FindPathPatternsTest, pathIsShort)
{
    auto path = FAWorld::pathFind(level.get(), GetParam().start, GetParam().goal, isReachable, false);

    if (path.size() > GetParam().expectedStepsAmount)
    {
        std::cout << GetParam().description << std::endl;
        test_utils::drawPath(*level, GetParam().start, path);
    }

    ASSERT_EQ(path.size(), GetParam().expectedStepsAmount);
}

TEST_P(FindPathPatternsTest, pathIsWalkable)
{
    auto path = FAWorld::pathFind(level.get(), GetParam().start, GetParam().goal, isReachable, false);

    for (auto tile : path)
        ASSERT_TRUE(level->isPassable(tile));
}

INSTANTIATE_TEST_CASE_P(FindPathPatternsParams,
                        FindPathPatternsTest,
                        ::testing::Values(FindPathPatternsParams{"Walk #1", basicMap(), Point(2, 3), Point{18, 5}, 16},
                                          FindPathPatternsParams{"Walk #1 reverse", basicMap(), Point(18, 5), Point{2, 3}, 16},
                                          FindPathPatternsParams{"Walk #2", basicMap(), Point(3, 10), Point{18, 10}, 18},
                                          FindPathPatternsParams{"Walk #3", basicMap(), Point(3, 10), Point{15, 10}, 18},
                                          FindPathPatternsParams{"Goal point is not passable #1", basicMap(), Point(18, 7), Point{10, 10}, 12},
                                          FindPathPatternsParams{"Goal point is not passable #2", basicMap(), Point(18, 10), Point{10, 10}, 13}), );

TEST(FindPathTests, walksStraightOnRectangleMap)
{
    const Point start{0, 1};
    const Point goal{14, 1};
    FAWorld::LevelImplStub level(Map{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
                                     {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, //
                                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}});

    bool isReachable = false;
    auto path = FAWorld::pathFind(&level, start, goal, isReachable, false);

    ASSERT_EQ(path.size(), 14);
}
