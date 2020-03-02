#include "missile.h"

namespace FAWorld::Missile
{
    Missile::Attributes::Attributes(
        Creation::Method creation, Movement::Method movement, ActorEngagement::Method actorEngagement, FixedPoint maxRange, Tick timeToLive)
        : mCreation(creation), mMovement(movement), mActorEngagement(actorEngagement), mMaxRange(maxRange), mTimeToLive(timeToLive)
    {
    }

    Missile::Attributes Missile::Attributes::fromId(MissileId missileId)
    {
        static FixedPoint maxRangeIgnore = FixedPoint::fromRawValue(INT64_MAX);
        static Tick ttlIgnore = std::numeric_limits<Tick>::max();

        switch (missileId)
        {
            case MissileId::arrow:
                return Attributes(Creation::singleFrame16Direction, Movement::linear(30), ActorEngagement::damageEnemyAndStop, 15, ttlIgnore);
            case MissileId::firebolt:
                return Attributes(Creation::animated16Direction, Movement::linear(15), ActorEngagement::damageEnemyAndStop, 15, ttlIgnore);
            case MissileId::farrow:
            case MissileId::larrow:
                return Attributes(Creation::animated16Direction, Movement::linear(30), ActorEngagement::damageEnemyAndStop, 15, ttlIgnore);
            case MissileId::firewall:
            case MissileId::firewalla:
            case MissileId::firewallc:
                return Attributes(Creation::firewall, Movement::stationary, ActorEngagement::damageEnemy, maxRangeIgnore, World::getTicksInPeriod(8));
            case MissileId::manashield:
                return Attributes(Creation::basicAnimated, Movement::hoverOverCreator, ActorEngagement::none, maxRangeIgnore, World::getTicksInPeriod(8));
            case MissileId::town:
                return Attributes(Creation::townPortal, Movement::stationary, ActorEngagement::townPortal, maxRangeIgnore, ttlIgnore);
            default:
                invalid_enum(MissileId, missileId);
        }
    }
}
