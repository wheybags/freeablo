#ifndef FA_RENDERER_H
#define FA_RENDERER_H

#include <stddef.h>

#include <map>

#include <stdint.h>

#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>

#include <render/render.h>

#include "../faworld/position.h"

namespace Level
{
    class Level;
}

namespace FARender
{       
    class CacheSpriteGroup;
    typedef boost::shared_ptr<CacheSpriteGroup> FASpriteGroup;

    class RenderState
    {
        public:

        boost::mutex mMutex;

        FAWorld::Position mPos;
        
        std::vector<boost::tuple<FASpriteGroup, size_t, FAWorld::Position> > mObjects; ///< group, index into group, and position
    };

    enum RenderThreadState
    {
        running,
        levelChange,
        loadSprite,
        pause,
        spriteDestroy,
        stopped
    };


    class Renderer
    {
        public:
            static Renderer* get();
            
            Renderer();
            ~Renderer();

            void stop();

            void setLevel(const Level::Level& level);

            RenderState* getFreeState(); // ooh ah up de ra
            void setCurrentState(RenderState* current);

            FASpriteGroup loadImage(const std::string& path);

            std::pair<size_t, size_t> getClickedTile(size_t x, size_t y);

        private:
            FASpriteGroup loadImageImp(const std::string& path);
            
            void destroySprite(Render::SpriteGroup* s);
            
            static Renderer* mRenderer; ///< Singleton instance

            void renderLoop();
            
            boost::thread* mThread;            

            boost::atomic<RenderThreadState> mRenderThreadState;

            void* mThreadCommunicationTmp;
            Render::RenderLevel* mLevel;
            bool mDone;

            RenderState mStates[3];

            RenderState* mCurrent;

            std::map<std::string, boost::weak_ptr<CacheSpriteGroup> > mSpriteCache;

            friend class CacheSpriteGroup;
    };

    class CacheSpriteGroup
    {
        public:
            CacheSpriteGroup(const std::string& path): mSpriteGroup(path) {}
            
            ~CacheSpriteGroup()
            {
                Renderer* r = Renderer::get();
                if(r && !r->mDone)
                {
                    r->mRenderThreadState = pause;
                    while(r->mRenderThreadState != stopped){} // wait until the render thread is definitely done

                    r->mSpriteCache.erase(mPath);

                    r->mRenderThreadState = running;

                    r->destroySprite(&mSpriteGroup); // destroy the sprite in the rendering thread
                }
            }

            void destroy()
            {
                mSpriteGroup.destroy();
            }

            Render::SpriteGroup mSpriteGroup; 
            std::string mPath;
    };
}

#endif
