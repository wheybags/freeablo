#ifndef AI_H
#define AI_H

#include "player.h"
#include "actor.h"

namespace FAWorld
{

    class Actor;

    class AI
    {
    public:
        AI(const Actor* actor): mActor(actor) {};
        virtual ~AI() {};

        virtual void update() = 0;

    protected:
        const Actor * mActor;

    };

    // Does nothing
    class NullAI : public AI
    {
    public:
        NullAI(const Actor* actor): AI(actor) {};
        ~NullAI() {};
        void update() {};
    };

    class BasicMonsterAI : public AI
    {
    public:
        BasicMonsterAI(const Actor* actor): AI(actor) {};
        ~BasicMonsterAI() {};
        void update();

    private:
        const Player * findNearestPlayer();
    };

}

#endif
