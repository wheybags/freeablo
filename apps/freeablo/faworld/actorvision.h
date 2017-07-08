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
        ActorVision(const Actor* actor);
        ~ActorVision() {}

        void update();

        bool canSee(int32_t x, int32_t y) const
        {
            return mVisibleTiles.find({x, y}) != mVisibleTiles.end();
        }

    private:
        const Actor* mActor;
        // TODO: add an event to Position that will trigger when the
        // actor's position changes.  We can hook vision on that and
        // update only when necessary.  Then this variable can be
        // removed.
        std::pair<int32_t, int32_t> mOrigin;
        std::set<std::pair<int32_t, int32_t>> mVisibleTiles;
    };

}

#endif
