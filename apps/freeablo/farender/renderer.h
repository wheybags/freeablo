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
#include "../fagui/guimanager.h"
#include <numeric>

namespace Render
{
    enum class CursorHotspotLocation;
}


namespace FAWorld
{
    class GameLevel;
}

namespace FARender
{
    class FontInfo;

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
       uint32_t frame;
       FAWorld::Position position;
       boost::optional<Cel::Colour> hoverColor;
     };

    class RenderState
    {
        public:

        std::atomic_bool ready;

        FAWorld::Position mPos;

        std::vector<ObjectToRender> mItems;
        std::vector<ObjectToRender> mObjects;

        NuklearFrameDump nuklearData;

        Tileset tileset;

        FAWorld::GameLevel* level;

        FASpriteGroup* mCursorSpriteGroup;
        Render::CursorHotspotLocation mCursorHotspot;
        uint32_t mCursorFrame;

        bool mCursorEmpty;

        RenderState(Render::NuklearGraphicsContext& nuklearGraphicsData) : ready(true), nuklearData(nuklearGraphicsData.dev) {}
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

            void drawCursor(RenderState *State);

            bool renderFrame(RenderState* state, const std::vector<uint32_t>& spritesToPreload); ///< To be called only by Engine::ThreadManager
            void cleanup(); ///< To be called only by Engine::ThreadManager
            Misc::Point cursorSize () const { return mCursorSize; }

            nk_context* getNuklearContext()
            {
                return &mNuklearContext;
            }

            void getWindowDimensions(int32_t& w, int32_t& h);

            bool getAndClearSpritesNeedingPreloading(std::vector<uint32_t>& sprites);
            nk_user_font *smallFont () const;

        private:
            std::unique_ptr<FontInfo> generateFont (const std::string& texturePath);

        private:
            static Renderer* mRenderer; ///< Singleton instance

            std::atomic_bool mDone;
            Render::LevelObjects mLevelObjects;
            Render::LevelObjects mItems;

            size_t mNumRenderStates = 15;
            RenderState* mStates;

            SpriteManager mSpriteManager;
            Misc::Point mCursorSize;

            volatile bool mAlreadyExited = false;
            std::mutex mDoneMutex;
            std::condition_variable mDoneCV;

            nk_context mNuklearContext = nk_context();
            Render::NuklearGraphicsContext mNuklearGraphicsData = Render::NuklearGraphicsContext();

            std::atomic<std::int64_t> mWidthHeightTmp;
            std::unique_ptr<FontInfo> m_smallFont;
    };
}

#endif
