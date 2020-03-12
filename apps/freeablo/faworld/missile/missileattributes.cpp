#include "missile.h"

namespace FAWorld::Missile
{
    Missile::Attributes::Attributes(Creation::Method creation, Movement::Method movement, ActorEngagement::Method actorEngagement, Tick timeToLive)
        : mCreation(creation), mMovement(movement), mActorEngagement(actorEngagement), mTimeToLive(timeToLive)
    {
    }

    Missile::Attributes Missile::Attributes::fromId(MissileId missileId)
    {
        static Tick ttlIgnore = std::numeric_limits<Tick>::max();

        switch (missileId)
        {
            case MissileId::arrow:
                return Attributes(Creation::singleFrame16Direction, Movement::linear(30, 15), ActorEngagement::damageEnemyAndStop, ttlIgnore);
            case MissileId::firebolt:
                return Attributes(Creation::animated16Direction, Movement::linear(15, 15), ActorEngagement::damageEnemyAndStop, ttlIgnore);
            case MissileId::farrow:
            case MissileId::larrow:
                return Attributes(Creation::animated16Direction, Movement::linear(30, 15), ActorEngagement::damageEnemyAndStop, ttlIgnore);
            case MissileId::firewall:
            case MissileId::firewalla:
            case MissileId::firewallc:
                return Attributes(Creation::firewall, Movement::stationary, ActorEngagement::damageEnemy, World::getTicksInPeriod(8));
            case MissileId::manashield:
                return Attributes(Creation::basicAnimated, Movement::hoverOverCreator, ActorEngagement::none, World::getTicksInPeriod(8));
            case MissileId::town:
                return Attributes(Creation::townPortal, Movement::stationary, ActorEngagement::townPortal, ttlIgnore);
            default:
                invalid_enum(MissileId, missileId);
        }
    }
}
