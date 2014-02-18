#ifndef FA_RENDERER_H
#define FA_RENDERER_H

#include <stddef.h>

#include <map>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>

#include <render/render.h>

#include "../faworld/position.h"

namespace Level
{
    class DunFile;
}

namespace FARender
{
    typedef boost::shared_ptr<Render::SpriteGroup> FASpriteGroup;

    class RenderState
    {
        public:

        boost::mutex mMutex;

        FAWorld::Position mPos;
        
        std::vector<boost::tuple<FASpriteGroup, size_t, FAWorld::Position> > mObjects; ///< group, index into group, and position
    };

    class Renderer
    {
        public:
            static Renderer* get();
            
            Renderer();
            ~Renderer();
            bool setLevel(const Level::DunFile& dun, size_t level);

            RenderState* getFreeState(); // ooh ah up de ra
            void setCurrentState(RenderState* current);

            FASpriteGroup loadImage(const std::string& path);

        private:
            static Renderer* mRenderer; ///< Singleton instance

            void renderLoop();
            
            boost::thread* mThread;            
            bool mLevelReady; 
            bool mDone;

            RenderState mStates[3];

            RenderState* mCurrent;

            std::map<std::string, boost::weak_ptr<Render::SpriteGroup> > mSpriteCache;
    };
}

#endif
