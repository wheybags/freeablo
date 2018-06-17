#include "vec2fix.h"

FixedPoint Vec2Fix::magnitude() const { return FixedPoint(x * x + y * y).sqrt(); }

void Vec2Fix::normalise()
{
    FixedPoint mag = magnitude();
    x = x / mag;
    y = y / mag;
}

Misc::Direction Vec2Fix::getIsometricDirection() const
{
    using namespace Misc;

    // First we calculate the direction normally, not accounting for irometric perspective.

    auto calculate = [this]() {
        //                            |n_ne_line_x
        //                            |         |e_ne_line_x
        //     |        |             |         |
        //     |        | _.---+----. |         |
        //     |      ,`|'           `|-.       |
        //     |   ,-'  |      N     /|  `-.    |
        //     |  /     |\          / |     ,'  |
        //     |,'      | \        /  |      `. |
        //     +     NW |  \      ,'  | NE     \|
        //    `+.._     |   \     /   |     _.-'|
        //    ;|   `-._ |    \   /    | ,,-'    |
        //   ; |       `+..  `. /,  ,-|'        |:
        //  .. |  W     |  ::-._:-    |    E    |.
        //   : |        | _,,-'+`-.   |         |;
        //    :|      _.+'   .'|\   `-+._       |
        //    :| _,,-'  |    |   \    |  `-._   |
        //    .+'       |   /     \   |      `-.|
        //     |`.   SW |  /       \  |SE    ,' |
        //     |  \     | /    S   `. |     /   |
        //     |   `-.  |/          | |  ,-\    |
        //     |      `-|:           \|-'       |
        //     |        | `----+---'' |         |

        // We calculate the direction by looking directly at the normalised x and y values of the vector.
        // Why do this instead of just using atan and thresholding like a normal person? Because then I'd need
        // to implement a fixed point version of atan :v

        Vec2Fix n = *this;
        n.normalise();

        static FixedPoint n_ne_line_x = "0.382683";
        static FixedPoint e_ne_line_x = "0.9239";
        FixedPoint e_se_line_x = e_ne_line_x;
        FixedPoint s_se_line_x = n_ne_line_x;

        FixedPoint n_nw_line_x = FixedPoint(0) - n_ne_line_x;
        FixedPoint w_nw_line_x = FixedPoint(0) - e_ne_line_x;
        FixedPoint w_sw_line_x = w_nw_line_x;
        FixedPoint s_sw_line_x = n_nw_line_x;

        if (n.x > 0)
        {
            if (n.y > 0)
            {
                if (n.x > n_ne_line_x)
                {
                    if (n.x > e_ne_line_x)
                        return Direction::east;
                    else
                        return Direction::north_east;
                }
                else
                {
                    return Direction::north;
                }
            }
            else
            {
                if (n.x > s_se_line_x)
                {
                    if (n.x > e_se_line_x)
                        return Direction::east;
                    else
                        return Direction::south_east;
                }
                else
                {
                    return Direction::south;
                }
            }
        }
        else
        {
            if (n.y > 0)
            {
                if (n.x < n_nw_line_x)
                {
                    if (n.x < w_nw_line_x)
                        return Direction::west;
                    else
                        return Direction::north_west;
                }
                else
                {
                    return Direction::north;
                }
            }
            else
            {
                if (n.x < s_sw_line_x)
                {
                    if (n.x < w_sw_line_x)
                        return Direction::west;
                    else
                        return Direction::south_west;
                }
                else
                {
                    return Direction::south;
                }
            }
        }
    };

    Misc::Direction normalDirection = calculate();

    Misc::Direction mapping[] = {Direction::north_east,
                                 Direction::north,
                                 Direction::north_west,
                                 Direction::west,
                                 Direction::south_west,
                                 Direction::south,
                                 Direction::south_east,
                                 Direction::east};

    Misc::Direction isometric = mapping[size_t(normalDirection)];
    return isometric;
}

Vec2Fix Vec2Fix::operator-(const Vec2Fix& other) const { return Vec2Fix(x - other.x, y - other.y); }
