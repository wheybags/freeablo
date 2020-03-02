#include "faworld/actor.h"
#include "missile.h"

namespace FAWorld::Missile
{
    void Missile::ActorEngagement::none(Missile&, MissileGraphic&, Actor&) {}

    void Missile::ActorEngagement::damageEnemy(Missile& missile, MissileGraphic&, Actor& actor)
    {
        const uint32_t damage = 10; // placeholder
        if (missile.mCreator->canIAttack(&actor))
        {
            missile.mCreator->dealDamageToEnemy(&actor, damage, DamageType::Bow);
            missile.playImpactSound();
        }
    }

    void Missile::ActorEngagement::damageEnemyAndStop(Missile& missile, MissileGraphic& graphic, Actor& actor)
    {
        damageEnemy(missile, graphic, actor);
        // Stop on friendlies too.
        if (&actor != missile.mCreator)
            graphic.stop();
    }

    void Missile::ActorEngagement::townPortal(Missile& missile, MissileGraphic& graphic, Actor& actor)
    {
        if (&actor == missile.mCreator)
        {
            // Teleport to other portal
            auto& otherPortal = missile.mGraphics[0].get() != &graphic ? missile.mGraphics[0] : missile.mGraphics[1];
            auto noMissilesAtPoint = [&otherPortal](const Misc::Point& p) {
                return std::none_of(
                    otherPortal->getLevel()->mMissileGraphics.begin(), otherPortal->getLevel()->mMissileGraphics.end(),
                    [&p](const MissileGraphic* g) { return p == g->mCurPos.current();});
            };
            auto point = otherPortal->getLevel()->getFreeSpotNear(otherPortal->mCurPos.current(), INT32_MAX, noMissilesAtPoint);
            actor.teleport(otherPortal->getLevel(), Position(point));
            // Close the portal if teleporting back through the 2nd (town located) portal
            if (&graphic == missile.mGraphics[1].get())
            {
                graphic.stop();
                otherPortal->stop();
            }
        }
    }
}
