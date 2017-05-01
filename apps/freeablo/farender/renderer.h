#ifndef FA_RENDERER_H
#define FA_RENDERER_H

#include <stddef.h>
#include <stdint.h>

#include <map>
#include <condition_variable>
#include <mutex>
#include <tuple>
#include <atomic>

#include <render/render.h>

#include "../faworld/position.h"

#include "spritemanager.h"


namespace FAWorld
{
    class GameLevel;
}

namespace FARender
{

    class Renderer;
    class Tileset
    {
        private:
            FASpriteGroup* minTops;
            FASpriteGroup* minBottoms;
            friend class Renderer;
    };

    struct ObjectToRender {
      FASpriteGroup* spriteGroup;
      size_t frame;
      FAWorld::Position position;
      boost::optional<Cel::Colour> hoverColor;
    };

    class RenderState
    {
        public:

        std::atomic_bool ready;

        FAWorld::Position mPos;

        std::vector<ObjectToRender> mObjects; ///< group, index into group, and position

        std::vector<DrawCommand> guiDrawBuffer;

        Tileset tileset;

        FAWorld::GameLevel* level;

        FASpriteGroup* mCursorSpriteGroup;
        uint32_t mCursorFrame;

        bool mCursorEmpty;

        RenderState():ready(true) {}
    };

    FASpriteGroup* getDefaultSprite();

    class Renderer
    {
        public:
            static Renderer* get();

            Renderer(int32_t windowWidth, int32_t windowHeight, bool fullscreen);
            ~Renderer();

            void stop();
            void waitUntilDone();

            Tileset getTileset(const FAWorld::GameLevel& level);

            RenderState* getFreeState(); // ooh ah up de ra
            void setCurrentState(RenderState* current);

            FASpriteGroup* loadImage(const std::string& path);
            FASpriteGroup* loadServerImage(uint32_t index);
            void fillServerSprite(uint32_t index, const std::string& path);
            std::string getPathForIndex(uint32_t index);

            Render::Tile getTileByScreenPos(size_t x, size_t y, const FAWorld::Position& screenPos);

            Rocket::Core::Context* getRocketContext();

            void setCursor(RenderState *State);

            bool renderFrame(RenderState* state); ///< To be called only by Engine::ThreadManager
            void cleanup(); ///< To be called only by Engine::ThreadManager


        private:
            bool loadGuiTextureFunc(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source);
            bool generateGuiTextureFunc(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions);
            void releaseGuiTextureFunc(Rocket::Core::TextureHandle texture_handle);

            static Renderer* mRenderer; ///< Singleton instance

            std::atomic_bool mDone;
            Render::LevelObjects mLevelObjects;

            RenderState mStates[15];

            Rocket::Core::Context* mRocketContext;

            SpriteManager mSpriteManager;

            volatile bool mAlreadyExited = false;
            std::mutex mDoneMutex;
            std::condition_variable mDoneCV;
    };
}

#endif
