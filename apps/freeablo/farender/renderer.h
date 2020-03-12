#pragma once
#include "../faworld/position.h"
#include "diabloexe/diabloexe.h"
#include "fontinfo.h"
#include "spritemanager.h"
#include <atomic>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <render/render.h>
#include <stddef.h>
#include <stdint.h>
#include <tuple>

namespace FAWorld
{
    class GameLevel;
}

namespace FARender
{
    class CelFontInfo;

    class Renderer;

    class Tileset
    {
    private:
        FASpriteGroup* minTops;
        FASpriteGroup* minBottoms;
        FASpriteGroup* mSpecialSprites;
        std::map<int32_t, int32_t> mSpecialSpriteMap;
        friend class Renderer;
    };

    struct ObjectToRender
    {
        FASpriteGroup* spriteGroup;
        uint32_t frame;
        FAWorld::Position position;
        std::optional<Cel::Colour> hoverColor;
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

        std::string mCursorPath;
        uint32_t mCursorFrame;
        bool mCursorCentered;

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

        void drawCursor(RenderState* State);

        bool renderFrame(RenderState* state, const std::vector<uint32_t>& spritesToPreload); ///< To be called only by Engine::ThreadManager
        void cleanup();                                                                      ///< To be called only by Engine::ThreadManager
        Misc::Point cursorSize() const { return mCursorSize; }

        nk_context* getNuklearContext() { return &mNuklearContext; }

        void getWindowDimensions(int32_t& w, int32_t& h);
        void loadFonts(const DiabloExe::DiabloExe& exe);

        bool getAndClearSpritesNeedingPreloading(std::vector<uint32_t>& sprites);
        nk_user_font* smallFont() const;
        nk_user_font* bigTGoldFont() const;
        nk_user_font* goldFont(int height) const;
        nk_user_font* silverFont(int height) const;

    private:
        std::unique_ptr<CelFontInfo> generateCelFont(const std::string& texturePath, const DiabloExe::FontData& fontData, int spacing);
        std::unique_ptr<PcxFontInfo> generateFont(const std::string& pcxPath, const std::string& binPath, const PcxFontInitData& fontInitData);

    private:
        static Renderer* mRenderer; ///< Singleton instance

        std::atomic_bool mDone;
        Render::LevelObjects mLevelObjects;
        Render::LevelObjects mItems;

        size_t mNumRenderStates = 15;
        RenderState* mStates;

        SpriteManager mSpriteManager;
        Render::FACursor mCurrentCursor = NULL;
        uint32_t mCurrentCursorFrame = UINT32_MAX;
        Misc::Point mCursorSize;

        volatile bool mAlreadyExited = false;
        std::mutex mDoneMutex;
        std::condition_variable mDoneCV;

        nk_context mNuklearContext = nk_context();
        Render::NuklearGraphicsContext mNuklearGraphicsData = Render::NuklearGraphicsContext();

        std::atomic<std::int64_t> mWidthHeightTmp;
        std::unique_ptr<CelFontInfo> mSmallTextFont, mBigTGoldFont;
        std::map<int, std::unique_ptr<PcxFontInfo>> mGoldFont, mSilverFont;
    };
}
