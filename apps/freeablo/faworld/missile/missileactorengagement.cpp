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
}
