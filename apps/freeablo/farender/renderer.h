#ifndef FA_RENDERER_H
#define FA_RENDERER_H

#include <stddef.h>
#include <stdint.h>

#include <map>

#include <boost/thread.hpp>

#include <render/render.h>

#include "../faworld/position.h"

#include "spritecache.h"

namespace Level
{
    class Level;
}

namespace FARender
{       
    class RenderState
    {
        public:

        boost::mutex mMutex;

        FAWorld::Position mPos;
        
        std::vector<boost::tuple<FASpriteGroup, size_t, FAWorld::Position> > mObjects; ///< group, index into group, and position

        std::vector<drawCommand> guiDrawBuffer;
    };

    class Renderer
    {
        public:
            static Renderer* get();
            
            Renderer(int32_t windowWidth, int32_t windowHeight);
            ~Renderer();

            void stop();
            
            void setLevel(Render::RenderLevel* renderLevel, const Level::Level* level);
            void setLevel(const Level::Level* level);

            RenderState* getFreeState(); // ooh ah up de ra
            void setCurrentState(RenderState* current);

            FASpriteGroup loadImage(const std::string& path);

            std::pair<size_t, size_t> getClickedTile(size_t x, size_t y);

            Rocket::Core::Context* getRocketContext();

            bool renderFrame(); ///< To be called only by Engine::ThreadManager
            void cleanup(); ///< To be called only by Engine::ThreadManager
            
        private:

            FASpriteGroup loadImageImp(const std::string& path);
            
            void destroySprite(Render::SpriteGroup* s);
            
            static Renderer* mRenderer; ///< Singleton instance


            Render::RenderLevel* mLevel;
            bool mDone;
            Render::LevelObjects mLevelObjects;

            RenderState mStates[3];

            RenderState* mCurrent;

            Rocket::Core::Context* mRocketContext;

            SpriteCache mCache;
    };
}

#endif
