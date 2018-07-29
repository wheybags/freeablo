#include "drawpath.h"

#include <iomanip>
#include <iostream>

namespace test_utils
{
    void drawPath(FAWorld::GameLevel::GameLevelImpl& graph, const Misc::Point& start, const Misc::Points* actualPath, const Misc::Points* expectedPath)
    {
        const int fieldWidth = 2;

        for (int y = 0; y != graph.height(); ++y)
        {
            for (int x = 0; x != graph.width(); ++x)
            {
                Misc::Point id(x, y);

                std::cout << std::left << std::setw(fieldWidth);

                if (!graph.isPassable(id))
                {
                    std::cout << std::string(fieldWidth, '#');
                }
                else if (actualPath && find(actualPath->begin(), actualPath->end(), id) != actualPath->end())
                {
                    std::cout << u8"\u2714 ";
                }
                else if (expectedPath && find(expectedPath->begin(), expectedPath->end(), id) != expectedPath->end())
                {
                    std::cout << u8"\u2718 ";
                }
                else if (start == id)
                {
                    std::cout << u8"\u272B ";
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