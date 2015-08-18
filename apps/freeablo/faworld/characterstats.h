#ifndef DIABLOCHARACTERSTATS_H
#define DIABLOCHARACTERSTATS_H
#include "actorstats.hpp"
namespace FAWorld
{
    class Inventory;

    class WarriorStats : public ActorStats
    {
    public:
        WarriorStats(uint32_t strength,
                     uint32_t maxStrength,
                     uint32_t magic,
                     uint32_t maxMagic,
                     uint32_t dexterity,
                     uint32_t maxDexterity,
                     uint32_t vitality,
                     uint32_t maxVitality,
                     uint32_t blocking) : ActorStats(
                                              strength,
                                              maxStrength,
                                              magic,
                                              maxMagic,
                                              dexterity,
                                              maxDexterity,
                                              vitality,
                                              maxVitality,
                                              blocking){}
        void recalculateDerivedStats() final;
    private:
        friend class Inventory;

    };

    class RogueStats : public ActorStats
    {
        public:
            RogueStats(uint32_t strength,
                       uint32_t maxStrength,
                       uint32_t magic,
                       uint32_t maxMagic,
                       uint32_t dexterity,
                       uint32_t maxDexterity,
                       uint32_t vitality,
                       uint32_t maxVitality,
                       uint32_t blocking) : ActorStats(
                                                strength,
                                                maxStrength,
                                                magic,
                                                maxMagic,
                                                dexterity,
                                                maxDexterity,
                                                vitality,
                                                maxVitality,
                                                blocking){}


        private:
            void recalculateDerivedStats() final;
            friend class Inventory;

    };

    class SorcererStats : public ActorStats
    {
        public:
            SorcererStats(uint32_t strength,
                        uint32_t maxStrength,
                        uint32_t magic,
                        uint32_t maxMagic,
                        uint32_t dexterity,
                        uint32_t maxDexterity,
                        uint32_t vitality,
                        uint32_t maxVitality,
                        uint32_t blocking) : ActorStats(
                                                strength,
                                                maxStrength,
                                                magic,
                                                maxMagic,
                                                dexterity,
                                                maxDexterity,
                                                vitality,
                                                maxVitality,
                                                blocking){}
        private:

            void recalculateDerivedStats() final;
            friend class Inventory;

    };
}
#endif // CHARACTERSTATS_H
