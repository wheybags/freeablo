#include "faworld/player.h"
#include "missile.h"
#include <engine/debugsettings.h>
#include <random/random.h>

namespace FAWorld::Missile
{
    void Missile::ActorEngagement::none(Missile&, MissileGraphic&, Actor&) {}

    void Missile::ActorEngagement::damageEnemy(Missile& missile, MissileGraphic&, Actor& actor, int32_t damage)
    {
        if (missile.mCreator->canIAttack(&actor))
        {
            missile.mCreator->dealDamageToEnemy(&actor, damage, DamageType::Bow);
            missile.playImpactSound();
        }
    }

    void Missile::ActorEngagement::damageEnemyAndStop(Missile& missile, MissileGraphic& graphic, Actor& actor)
    {
        damageEnemy(missile, graphic, actor, 10);
        // Stop on friendlies too.
        if (&actor != missile.mCreator)
            graphic.stop();
    }

    void Missile::ActorEngagement::arrowEngagement(Missile& missile, MissileGraphic& graphic, Actor& actor)
    {
        World* world = actor.getWorld();

        if (missile.mCreator->canIAttack(&actor))
        {
            int32_t distanceSquared =
                int32_t((graphic.mCurPos.getFractionalPos() - Vec2Fix(missile.mSrcPoint.x, missile.mSrcPoint.y)).magnitudeSquared().floor());

            int32_t toHit = missile.mToHitRanged.getCombined();
            toHit -= distanceSquared / 2;
            toHit -= actor.getStats().getCalculatedStats().armorClass;
            toHit = Misc::clamp(toHit, missile.mToHitMinMaxCap.min, missile.mToHitMinMaxCap.max);
            int32_t roll = world->mRng->randomInRange(0, 99);

            if (roll < toHit || DebugSettings::Instakill)
            {
                int32_t damage = missile.mRangedDamage;
                damage += world->mRng->randomInRange(missile.mRangedDamageBonusRange.start, missile.mRangedDamageBonusRange.end);
                missile.mCreator->dealDamageToEnemy(&actor, damage, DamageType::Bow);
            }

            missile.playImpactSound();
        }

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
