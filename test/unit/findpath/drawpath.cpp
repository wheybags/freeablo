#include "drawpath.h"
#include <algorithm>
#include <iomanip>
#include <iostream>

namespace
{
    const char* CHECK_MARK = u8"\u2714 ";
    const char* STAR_MARK = u8"\u272B ";
}

namespace test_utils
{
    void drawPath(FAWorld::GameLevel::GameLevelImpl& graph, const Misc::Point& start, const Misc::Points& actualPath)
    {
        const int fieldWidth = 2;

        for (int y = 0; y != graph.height(); ++y)
        {
            for (int x = 0; x != graph.width(); ++x)
            {
                Misc::Point id(x, y);

                std::cout << std::left << std::setw(fieldWidth);

                bool containsActualPath = std::find(actualPath.begin(), actualPath.end(), id) != actualPath.end();

                if (!graph.isPassable(id, nullptr))
                {
                    std::cout << std::string(fieldWidth, '#');
                }
                else if (containsActualPath)
                {
                    std::cout << CHECK_MARK;
                }
                else if (start == id)
                {
                    std::cout << STAR_MARK;
                }
                else
                {
                    std::cout << '.';
                }
            }
            std::cout << std::endl;
        }
    }
}
