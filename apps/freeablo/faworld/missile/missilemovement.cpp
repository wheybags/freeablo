#include "faworld/actor.h"
#include "missile.h"

namespace FAWorld::Missile
{
    void Missile::Movement::stationary(Missile&, MissileGraphic&) {}

    void Missile::Movement::linear(Missile&, MissileGraphic& graphic)
    {
        graphic.mCurPos.setFreeMovement();
        graphic.mCurPos.update(FixedPoint(7) / FixedPoint(World::ticksPerSecond));
    }

    void Missile::Movement::hoverOverCreator(Missile& missile, MissileGraphic& graphic)
    {
        // TODO: change level with actor.
        graphic.mCurPos = missile.mCreator->getPos();
    }
}
