#pragma once
#include "missileenums.h"
#include "missilegraphic.h"
#include <misc/misc.h>
#include <vector>

namespace FASaveGame
{
    class GameLoader;
    class GameSaver;
}

namespace FAWorld
{
    class Actor;

    namespace Missile
    {
        class Missile
        {
            friend class MissileCreation;
            friend class MissileMovement;
            friend class MissileActorEngagement;
            friend class MissileMaxRange;
            friend class MissileTimeToLive;

        public:
            virtual ~Missile() = default;
            Missile() = default;

            Missile(MissileId missileId, Actor& creator, Misc::Point dest);
            Missile(FASaveGame::GameLoader& loader);

            virtual void save(FASaveGame::GameSaver& saver);
            virtual void update();
            virtual bool isComplete() const { return mComplete; }
            MissileId getMissileId() const { return mMissileId; }
            const GameLevel* getLevel() const { return mLevel; }

            std::vector<std::unique_ptr<MissileGraphic>> mGraphics;

        protected:
            Actor* mCreator = nullptr;
            MissileId mMissileId = MissileId(0);
            GameLevel* mLevel = nullptr;
            Misc::Point mSrcPoint;
            bool mComplete = false;

            const DiabloExe::MissileData& missileData() const;
            const DiabloExe::MissileGraphics& missileGraphics() const;
            std::string getGraphicsPath(int32_t i) const;
            void playImpactSound();
        };

        class MissileCreation
        {
        public:
            typedef void (*MissileCreationMethod)(Missile& missile, Misc::Point dest);
            static MissileCreationMethod get(MissileId missileId);

        protected:
            static void singleFrame16Direction(Missile& missile, Misc::Point dest);
            static void animated16Direction(Missile& missile, Misc::Point dest);
            static void firewall(Missile& missile, Misc::Point dest);
            static void basicAnimated(Missile& missile, Misc::Point dest);
        };

        class MissileMovement
        {
        public:
            typedef void (*MissileMovementMethod)(Missile& missile, MissileGraphic& graphic);
            static MissileMovementMethod get(MissileId missileId);

        protected:
            static void fixed(Missile& missile, MissileGraphic& graphic);
            static void linear(Missile& missile, MissileGraphic& graphic);
            static void hoverOverCreator(Missile& missile, MissileGraphic& graphic);
        };

        class MissileActorEngagement
        {
        public:
            typedef void (*MissileActorEngagementMethod)(Missile& missile, MissileGraphic& graphic, Actor& actor);
            static MissileActorEngagementMethod get(MissileId missileId);

        protected:
            static void none(Missile& missile, MissileGraphic& graphic, Actor& actor);
            static void damageEnemy(Missile& missile, MissileGraphic& graphic, Actor& actor);
            static void damageEnemyAndStop(Missile& missile, MissileGraphic& graphic, Actor& actor);
        };

        class MissileMaxRange
        {
        public:
            static FixedPoint get(MissileId missileId)
            {
                switch (missileId)
                {
                    case MissileId::arrow:
                    case MissileId::firebolt:
                    case MissileId::farrow:
                    case MissileId::larrow:
                        return 15; // placeholder
                    default:
                        return FixedPoint::fromRawValue(INT64_MAX);
                }
            }
        };

        class MissileTimeToLive
        {
        public:
            static Tick get(MissileId missileId)
            {
                switch (missileId)
                {
                    case MissileId::firewall:
                    case MissileId::firewalla:
                    case MissileId::firewallc:
                    case MissileId::manashield:
                        return World::getTicksInPeriod("8"); // placeholder
                    default:
                        return std::numeric_limits<Tick>::max();
                }
            }
        };
    }
}
