#include "faworld/player.h"
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
        // Any player can use a town portal
        if (auto player = dynamic_cast<Player*>(&actor))
        {
            // Teleport to other portal
            auto& otherPortal = missile.mGraphics[0].get() != &graphic ? missile.mGraphics[0] : missile.mGraphics[1];
            auto noMissilesAtPoint = [&otherPortal](const Misc::Point& p) {
                return std::none_of(otherPortal->getLevel()->mMissileGraphics.begin(),
                                    otherPortal->getLevel()->mMissileGraphics.end(),
                                    [&p](const MissileGraphic* g) { return p == g->mCurPos.current(); });
            };
            auto point = otherPortal->getLevel()->getFreeSpotNear(otherPortal->mCurPos.current(), std::numeric_limits<int32_t>::max(), noMissilesAtPoint);
            player->teleport(otherPortal->getLevel(), Position(point));
            // Close the portal if the creator is teleporting back through the 2nd (town located) portal
            if (player == missile.mCreator && &graphic == missile.mGraphics[1].get())
            {
                graphic.stop();
                otherPortal->stop();
            }
        }
    }
}
