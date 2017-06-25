#ifndef AI_H
#define AI_H

#include <misc/misc.h>

namespace FAWorld
{

    class Actor;
    class Player;

    class AI
    {
    public:
        AI(const Actor* actor): mActor(actor) {};
        virtual ~AI() {};

        virtual void update(size_t ticksPassed) = 0;

    protected:
        const Actor * mActor;

    };

    // Does nothing
    class NullAI : public AI
    {
    public:
        NullAI(const Actor* actor): AI(actor) {};
        ~NullAI() {};
        void update(size_t ticksPassed) {
            UNUSED_PARAM(ticksPassed);
        };
    };

    class BasicMonsterAI : public AI
    {
    public:
        BasicMonsterAI(const Actor* actor): AI(actor) {};
        ~BasicMonsterAI() {};
        void update(size_t ticksPassed);

    private:
        size_t mLastActionTick;

        const Player * findNearestPlayer();
    };

}

#endif
