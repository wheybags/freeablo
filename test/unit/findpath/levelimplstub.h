#pragma once

#include <faworld/gamelevel.h>

namespace FAWorld
{
    class LevelImplStub : public FAWorld::GameLevelImpl
    {
    public:
        explicit LevelImplStub(std::vector<std::vector<int>> map) : map(std::move(map)){};

        int32_t width() const override { return map.empty() ? 0 : map[0].size(); }
        int32_t height() const override { return map.size(); }
        bool isPassable(const Misc::Point& point, const FAWorld::Actor*) const override { return map[point.y][point.x] == 0; }

    private:
        const std::vector<std::vector<int>> map;
    };
}
