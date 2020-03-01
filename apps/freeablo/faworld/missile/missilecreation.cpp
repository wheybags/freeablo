#include "missile.h"
#include <misc/simplevec2.h>

namespace FAWorld
{
    namespace Missile
    {
        void Missile::Creation::singleFrame16Direction(Missile& missile, Misc::Point dest)
        {
            auto direction = (Vec2Fix(dest.x, dest.y) - Vec2Fix(missile.mSrcPoint.x, missile.mSrcPoint.y)).getDirection();
            auto srcPos = Position(missile.mSrcPoint, direction);
            int32_t direction16 = static_cast<int32_t>(direction.getDirection16());
            missile.mGraphics.push_back(std::make_unique<MissileGraphic>("", missile.getGraphicsPath(0), direction16, srcPos));
        }

        void Missile::Creation::animated16Direction(Missile& missile, Misc::Point dest)
        {
            auto direction = (Vec2Fix(dest.x, dest.y) - Vec2Fix(missile.mSrcPoint.x, missile.mSrcPoint.y)).getDirection();
            auto srcPos = Position(missile.mSrcPoint, direction);
            int32_t direction16 = static_cast<int32_t>(direction.getDirection16());
            missile.mGraphics.push_back(std::make_unique<MissileGraphic>("", missile.getGraphicsPath(direction16), std::nullopt, srcPos));
        }

        void Missile::Creation::firewall(Missile& missile, Misc::Point dest)
        {
            // Flames are placed at -5 -> +5 perpendicular to the clicked point, and
            // two flames are placed at the clicked point (for double damage).
            auto direction = (Vec2Fix(dest.x, dest.y) - Vec2Fix(missile.mSrcPoint.x, missile.mSrcPoint.y)).getDirection();
            for (auto angleOffset : {-90, 90})
            {
                Misc::Direction dir = direction;
                dir.adjust(angleOffset);
                auto point = dest;
                for (int32_t i = 0; i < 6; i++)
                {
                    missile.mGraphics.push_back(
                        std::make_unique<MissileGraphic>(missile.getGraphicsPath(0), missile.getGraphicsPath(1), std::nullopt, Position(point)));
                    point = Misc::getNextPosByDir(point, dir);
                }
            }
        }

        void Missile::Creation::basicAnimated(Missile& missile, Misc::Point)
        {
            missile.mGraphics.push_back(std::make_unique<MissileGraphic>("", missile.getGraphicsPath(0), std::nullopt, Position(missile.mSrcPoint)));
        }
    }
}
