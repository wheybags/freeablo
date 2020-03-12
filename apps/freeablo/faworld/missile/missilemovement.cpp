#include "faworld/actor.h"
#include "missile.h"

namespace FAWorld::Missile
{
    void Missile::Movement::stationary(Missile&, MissileGraphic&) {}

    Missile::Movement::Method Missile::Movement::linear(FixedPoint speed, FixedPoint maxRange)
    {
        return [=](Missile& missile, MissileGraphic& graphic) { linear(missile, graphic, speed, maxRange); };
    }

    void Missile::Movement::linear(Missile& missile, MissileGraphic& graphic, FixedPoint speed, FixedPoint maxRange)
    {
        graphic.mCurPos.setFreeMovement();
        graphic.mCurPos.update(speed / FixedPoint(World::ticksPerSecond));

        // Stop after max range is exceeded.
        auto curPoint = graphic.mCurPos.current();
        auto distance = (Vec2Fix(curPoint.x, curPoint.y) - Vec2Fix(missile.mSrcPoint.x, missile.mSrcPoint.y)).magnitude();
        if (distance > maxRange)
            graphic.stop();
    }

    void Missile::Movement::hoverOverCreator(Missile& missile, MissileGraphic& graphic)
    {
        if (graphic.getLevel() != missile.mCreator->getLevel())
            graphic.setLevel(missile.mCreator->getLevel());
        graphic.mCurPos = missile.mCreator->getPos();
    }
}
