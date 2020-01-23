#include "missile.h"

#include "faworld/actor.h"

namespace FAWorld
{
    namespace Missile
    {
        MissileMovement::MissileMovementMethod MissileMovement::get(MissileId missileId)
        {
            switch (missileId)
            {
                case MissileId::arrow:
                case MissileId::firebolt:
                case MissileId::farrow:
                case MissileId::larrow:
                    return linear;
                case MissileId::firewall:
                    return fixed;
                case MissileId::manashield:
                    return hoverOverCreator;
                default:
                    invalid_enum(MissileId, missileId);
                    // return nullptr;  // MSVC generates C4702 unreachable code.
            }
        }

        void MissileMovement::fixed(Missile&, MissileGraphic&) {}

        void MissileMovement::linear(Missile&, MissileGraphic& graphic)
        {
            graphic.mCurPos.setFreeMovement();
            graphic.mCurPos.update();
        }

        void MissileMovement::hoverOverCreator(Missile& missile, MissileGraphic& graphic)
        {
            // TODO: change level with actor.
            graphic.mCurPos = missile.mCreator->getPos();
        }
    }
}
