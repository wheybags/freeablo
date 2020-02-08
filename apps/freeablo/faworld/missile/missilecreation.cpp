#include "missile.h"
#include <misc/vec2fix.h>

namespace FAWorld
{
    namespace Missile
    {
        MissileCreation::MissileCreationMethod MissileCreation::get(MissileId missileId)
        {
            switch (missileId)
            {
                case MissileId::arrow:
                    return singleFrame16Direction;
                case MissileId::firebolt:
                case MissileId::farrow:
                case MissileId::larrow:
                    return animated16Direction;
                case MissileId::firewall:
                    return firewall;
                case MissileId::manashield:
                    return basicAnimated;
                default:
                    invalid_enum(MissileId, missileId);
                    // return nullptr;  // MSVC generates C4702 unreachable code.
            }
        }

        void MissileCreation::singleFrame16Direction(Missile& missile, Misc::Point dest)
        {
            auto direction = (Vec2Fix(dest.x, dest.y) - Vec2Fix(missile.mSrcPoint.x, missile.mSrcPoint.y)).getDirection();
            auto srcPos = Position(missile.mSrcPoint, direction);
            int32_t direction16 = static_cast<int32_t>(direction.getDirection16());
            srcPos.setSpeed(3536);
            missile.mGraphics.push_back(nonstd::make_unique<MissileGraphic>("", missile.getGraphicsPath(0), direction16, srcPos));
        }

        void MissileCreation::animated16Direction(Missile& missile, Misc::Point dest)
        {
            auto direction = (Vec2Fix(dest.x, dest.y) - Vec2Fix(missile.mSrcPoint.x, missile.mSrcPoint.y)).getDirection();
            auto srcPos = Position(missile.mSrcPoint, direction);
            int32_t direction16 = static_cast<int32_t>(direction.getDirection16());
            srcPos.setSpeed(3536);
            missile.mGraphics.push_back(nonstd::make_unique<MissileGraphic>("", missile.getGraphicsPath(direction16), nonstd::nullopt, srcPos));
        }

        void MissileCreation::firewall(Missile& missile, Misc::Point dest)
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
                        nonstd::make_unique<MissileGraphic>(missile.getGraphicsPath(0), missile.getGraphicsPath(1), nonstd::nullopt, Position(point)));
                    point = Misc::getNextPosByDir(point, dir);
                }
            }
        }

        void MissileCreation::basicAnimated(Missile& missile, Misc::Point)
        {
            missile.mGraphics.push_back(nonstd::make_unique<MissileGraphic>("", missile.getGraphicsPath(0), nonstd::nullopt, Position(missile.mSrcPoint)));
        }
    }
}
