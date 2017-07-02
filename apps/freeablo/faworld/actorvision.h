#ifndef ACTORVISION_H
#define ACTORVISION_H

#include <stdint.h>
#include <set>

namespace FAWorld
{

    class Actor;

    class ActorVision
    {
    public:
        ActorVision(const Actor* actor): mActor(actor) {};
        ~ActorVision() {}

        void update();

        bool canSee(int32_t x, int32_t y) const
        {
            return mVisibleTiles.find({x, y}) != mVisibleTiles.end();
        }

    private:
        const Actor* mActor;
        std::set<std::pair<int32_t, int32_t>> mVisibleTiles;
    };

}

#endif
