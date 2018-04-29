#include "vec2fix.h"

void Vec2Fix::normalise()
{
    FixedPoint magnitude = (x * x + y * y).sqrt();
    x = x / magnitude;
    y = y / magnitude;
}

Misc::Direction Vec2Fix::getDirection() const
{
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

    FixedPoint n_ne_line_x = "0.382683";
    FixedPoint e_ne_line_x = "0.9239";
    FixedPoint e_se_line_x = e_ne_line_x;
    FixedPoint s_se_line_x = n_ne_line_x;

    FixedPoint n_nw_line_x = 0 - n_ne_line_x;
    FixedPoint w_nw_line_x = 0 - e_ne_line_x;
    FixedPoint w_sw_line_x = w_nw_line_x;
    FixedPoint s_sw_line_x = n_nw_line_x;

    if (n.x > 0)
    {
        if (n.y > 0)
        {
            if (n.x > n_ne_line_x)
            {
                if (n.x > e_ne_line_x)
                    return Misc::Direction::east;
                else
                    return Misc::Direction::north_east;
            }
            else
            {
                return Misc::Direction::north;
            }
        }
        else
        {
            if (n.x > s_se_line_x)
            {
                if (n.x > e_se_line_x)
                    return Misc::Direction::east;
                else
                    return Misc::Direction::south_east;
            }
            else
            {
                return Misc::Direction::south;
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
                    return Misc::Direction::west;
                else
                    return Misc::Direction::north_west;
            }
            else
            {
                return Misc::Direction::north;
            }
        }
        else
        {
            if (n.x < s_sw_line_x)
            {
                if (n.x < w_sw_line_x)
                    return Misc::Direction::west;
                else
                    return Misc::Direction::south_west;
            }
            else
            {
                return Misc::Direction::south;
            }
        }
    }
}
