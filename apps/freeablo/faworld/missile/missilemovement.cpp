#include "faworld/actor.h"
#include "missile.h"

namespace FAWorld::Missile
{
    void Missile::Movement::stationary(Missile&, MissileGraphic&) {}

    Missile::Movement::Method Missile::Movement::linear(FixedPoint speed)
    {
        return [=](Missile&, MissileGraphic& graphic) { linear(graphic, speed); };
    }

    void Missile::Movement::linear(MissileGraphic& graphic, FixedPoint speed)
    {
        graphic.mCurPos.setFreeMovement();
        graphic.mCurPos.update(speed / FixedPoint(World::ticksPerSecond));
    }

    void Missile::Movement::hoverOverCreator(Missile& missile, MissileGraphic& graphic)
    {
        // TODO: change level with actor.
        graphic.mCurPos = missile.mCreator->getPos();
    }
}
