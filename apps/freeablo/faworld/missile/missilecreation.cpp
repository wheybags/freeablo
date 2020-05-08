#include "engine/enginemain.h"
#include "faworld/gamelevel.h"
#include "missile.h"
#include <misc/simplevec2.h>

namespace FAWorld::Missile
{
    void Missile::Creation::singleFrame16Direction(Missile& missile, Vec2Fix dest, GameLevel* level)
    {
        Misc::Direction direction = (dest - missile.mSrcPoint).getDirection();
        Position srcPos(missile.mSrcPoint, direction);
        srcPos.setFreeMovement();
        srcPos.update(FixedPoint("0.5"));
        int32_t direction16 = static_cast<int32_t>(direction.getDirection16());
        missile.mGraphics.push_back(
            std::make_unique<MissileGraphic>(FARender::SpriteLoader::SpriteDefinition(), missile.getGraphic(0), direction16, srcPos, level));
    }

    void Missile::Creation::animated16Direction(Missile& missile, Vec2Fix dest, GameLevel* level)
    {
        Misc::Direction direction = (dest - missile.mSrcPoint).getDirection();
        Position srcPos(missile.mSrcPoint, direction);
        srcPos.setFreeMovement();
        srcPos.update(FixedPoint("0.5"));
        int32_t direction16 = static_cast<int32_t>(direction.getDirection16());
        missile.mGraphics.push_back(
            std::make_unique<MissileGraphic>(FARender::SpriteLoader::SpriteDefinition(), missile.getGraphic(direction16), std::nullopt, srcPos, level));
    }

    void Missile::Creation::firewall(Missile& missile, Vec2Fix dest, GameLevel* level)
    {
        // Flames are placed at -5 -> +5 perpendicular to the clicked point, and
        // two flames are placed at the clicked point (for double damage).
        Misc::Direction direction = (dest - missile.mSrcPoint).getDirection();
        for (auto angleOffset : {-90, 90})
        {
            Misc::Direction dir = direction;
            dir.adjust(angleOffset);
            Vec2i point(dest);
            for (int32_t i = 0; i < 6; i++)
            {
                missile.mGraphics.push_back(
                    std::make_unique<MissileGraphic>(missile.getGraphic(0), missile.getGraphic(1), std::nullopt, Position(point), level));
                point = Misc::getNextPosByDir(point, dir);
            }
        }
    }

    void Missile::Creation::basicAnimated(Missile& missile, Vec2Fix, GameLevel* level)
    {
        missile.mGraphics.push_back(std::make_unique<MissileGraphic>(
            FARender::SpriteLoader::SpriteDefinition(), missile.getGraphic(0), std::nullopt, Position(missile.mSrcPoint), level));
    }

    void Missile::Creation::townPortal(Missile& missile, Vec2Fix, GameLevel* level)
    {
        // Add portal near player
        auto noMissilesAtPoint = [&level](const Misc::Point& p) {
            return std::none_of(
                level->mMissileGraphics.begin(), level->mMissileGraphics.end(), [&p](const MissileGraphic* g) { return p == g->mCurPos.current(); });
        };
        auto point = level->getFreeSpotNear(Vec2i(missile.mSrcPoint), std::numeric_limits<int32_t>::max(), noMissilesAtPoint);
        missile.mGraphics.push_back(std::make_unique<MissileGraphic>(missile.getGraphic(0), missile.getGraphic(1), std::nullopt, Position(point), level));
        // Add portal in town
        auto town = Engine::EngineMain::get()->mWorld->getLevel(0);
        static const Misc::Point townPortalPoint = Misc::Point(60, 80);
        auto noMissilesAtTownPoint = [&town](const Misc::Point& p) {
            return std::none_of(
                town->mMissileGraphics.begin(), town->mMissileGraphics.end(), [&p](const MissileGraphic* g) { return p == g->mCurPos.current(); });
        };
        point = town->getFreeSpotNear(townPortalPoint, std::numeric_limits<int32_t>::max(), noMissilesAtTownPoint);
        missile.mGraphics.push_back(std::make_unique<MissileGraphic>(missile.getGraphic(0), missile.getGraphic(1), std::nullopt, Position(point), town));
    }
}
