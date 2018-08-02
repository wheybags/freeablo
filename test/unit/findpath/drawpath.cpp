#include "drawpath.h"

#include <iomanip>
#include <iostream>

namespace
{
    const char* CHECK_MARK = u8"\u2714 ";
    const char* CROSS_MARK = u8"\u2718 ";
    const char* RING_MARK = u8"\u25CB ";
    const char* STAR_MARK = u8"\u272B ";
}

namespace test_utils
{
    void drawPath(FAWorld::GameLevel::GameLevelImpl& graph, const Misc::Point& start, const Misc::Points& actualPath, const Misc::Points& expectedPath)
    {
        const int fieldWidth = 2;

        for (int y = 0; y != graph.height(); ++y)
        {
            for (int x = 0; x != graph.width(); ++x)
            {
                Misc::Point id(x, y);

                std::cout << std::left << std::setw(fieldWidth);

                bool containsActualPath = find(actualPath.begin(), actualPath.end(), id) != actualPath.end();
                bool containsExpectedPath = find(expectedPath.begin(), expectedPath.end(), id) != expectedPath.end();

                if (!graph.isPassable(id))
                {
                    std::cout << std::string(fieldWidth, '#');
                }
                else if (containsActualPath && containsExpectedPath)
                {
                    std::cout << CHECK_MARK;
                }
                else if (containsActualPath)
                {
                    std::cout << CROSS_MARK;
                }
                else if (containsExpectedPath)
                {
                    std::cout << RING_MARK;
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