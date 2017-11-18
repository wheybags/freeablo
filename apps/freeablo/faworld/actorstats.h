#ifndef ACTORSTATS_H
#define ACTORSTATS_H

#include <stdint.h>
#include <algorithm>

namespace DiabloExe
{
    class CharacterStats;
}

namespace FAWorld
{
    template<typename T>
    class MaxCurrentItem
    {
    public:
        T max;
        T current;

        MaxCurrentItem(T max) : max(max), current(max) {}

        bool change(T delta, bool allowClamp = true)
        {
            T next = std::max(std::min(current + delta, max), 0);
            bool clamped = next != current + delta;

            if (!clamped || allowClamp)
                current = next;

            return clamped;
        }
    };

    class ActorStats
    {
    public:
        MaxCurrentItem<int32_t> mHp;
        MaxCurrentItem<int32_t> mMana;

        int32_t mAttackDamage = 3;

        ActorStats() : mHp(100), mMana(100) {}

        void takeDamage(int32_t damage)
        {
            mHp.change(-damage);
        }

        int32_t getAttackDamage()
        {
            return mAttackDamage;
        }
    };
}

#endif //ACTORSTATS_H
