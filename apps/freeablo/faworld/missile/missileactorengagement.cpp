#include "missile.h"

#include "faworld/actor.h"

namespace FAWorld
{
    namespace Missile
    {
        MissileActorEngagement::MissileActorEngagementMethod MissileActorEngagement::get(MissileId missileId)
        {
            switch (missileId)
            {
                case MissileId::arrow:
                case MissileId::firebolt:
                case MissileId::farrow:
                case MissileId::larrow:
                    return damageEnemyAndStop;
                case MissileId::firewall:
                    return damageEnemy;
                case MissileId::manashield: // TODO
                default:
                    return none;
            }
        }

        void MissileActorEngagement::none(Missile&, MissileGraphic&, Actor&) {}

        void MissileActorEngagement::damageEnemy(Missile& missile, MissileGraphic&, Actor& actor)
        {
            const uint32_t damage = 10; // placeholder
            if (missile.mCreator->canIAttack(&actor))
            {
                missile.mCreator->inflictDamage(&actor, damage);
                missile.playImpactSound();
            }
        }

        void MissileActorEngagement::damageEnemyAndStop(Missile& missile, MissileGraphic& graphic, Actor& actor)
        {
            damageEnemy(missile, graphic, actor);
            // Stop on friendlies too.
            if (&actor != missile.mCreator)
                graphic.stop();
        }
    }
}
