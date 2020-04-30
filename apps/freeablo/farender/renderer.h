#pragma once
#include "../faworld/position.h"
#include "diabloexe/diabloexe.h"
#include "fontinfo.h"
#include "levelrenderer.h"
#include "spriteloader.h"
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <nuklearmisc/nuklearframedump.h>
#include <render/cursor.h>
#include <render/render.h>
#include <tuple>

namespace DiabloExe
{
    class DiabloExe;
}

namespace FAWorld
{
    class GameLevel;
}

namespace FARender
{
    class CelFontInfo;
    class Renderer;
    class LevelRenderer;

    class Tileset
    {
    private:
        Render::SpriteGroup* minTops = nullptr;
        Render::SpriteGroup* minBottoms = nullptr;
        Render::SpriteGroup* mSpecialSprites = nullptr;
        std::map<int32_t, int32_t> mSpecialSpriteMap;
        friend class Renderer;
    };

    struct ObjectToRender
    {
        Render::SpriteGroup* spriteGroup = nullptr;
        uint32_t frame = 0;
        FAWorld::Position position;
        std::optional<ByteColour> hoverColor;
    };

    class RenderState
    {
    public:
        struct MoveableAtomicBool
        {
            std::atomic_bool val;

            MoveableAtomicBool(bool val) : val(val) {}
            MoveableAtomicBool(MoveableAtomicBool&& other) : val(other.val.load()) {}

            void operator=(bool newVal) { val = newVal; }
            operator bool() { return val; }
        };

        MoveableAtomicBool ready;

        FAWorld::Position mPos;

        std::vector<ObjectToRender> mItems;
        std::vector<ObjectToRender> mObjects;

        NuklearFrameDump nuklearData;

        Tileset tileset;

        FAWorld::GameLevel* level{};

        std::string mCursorPath;
        uint32_t mCursorFrame = 0;
        bool mCursorCentered = false;

        explicit RenderState(NuklearDevice& nuklearGraphicsData) : ready(true), nuklearData(nuklearGraphicsData) {}
        RenderState(RenderState&& other) = default;
    };

    class Renderer
    {
    public:
        static Renderer* get();

        Renderer(const DiabloExe::DiabloExe& exe, int32_t windowWidth, int32_t windowHeight, bool fullscreen);
        ~Renderer();

        void stop();
        void waitUntilDone();

        Tileset getTileset(const FAWorld::GameLevel& level);

        RenderState* getFreeState(); // ooh ah up de ra
        void setCurrentState(RenderState* current);

        Render::Tile getTileByScreenPos(size_t x, size_t y, const FAWorld::Position& screenPos);

        void updateCursor(RenderState* State);

        bool renderFrame(RenderState* state); ///< To be called only by Engine::ThreadManager
        Misc::Point cursorSize() const { return mCursorSize; }

        nk_context* getNuklearContext() { return &mNuklearContext; }

        void getWindowDimensions(int32_t& w, int32_t& h);
        void loadFonts(const DiabloExe::DiabloExe& exe);

        nk_user_font* smallFont() const;
        nk_user_font* bigTGoldFont() const;
        nk_user_font* goldFont(int height) const;
        nk_user_font* silverFont(int height) const;

    private:
        std::unique_ptr<CelFontInfo> generateCelFont(Render::SpriteGroup* fontTexture, const DiabloExe::FontData& fontData, int spacing);
        std::unique_ptr<PcxFontInfo> generateFont(Render::SpriteGroup* fontTexture, const std::string& binPath, const PcxFontInitData& fontInitData);

    public:
        SpriteLoader mSpriteLoader;
        std::unique_ptr<LevelRenderer> mLevelRenderer;

    private:
        static Renderer* mRenderer; ///< Singleton instance

        std::atomic_bool mDone;
        LevelObjects mLevelObjects;
        LevelObjects mItems;

        static constexpr size_t NUM_RENDER_STATES = 15;
        std::vector<RenderState> mStates;

        std::unique_ptr<Render::Cursor> mCurrentCursor = nullptr;
        uint32_t mCurrentCursorFrame = std::numeric_limits<uint32_t>::max();
        Misc::Point mCursorSize;

        volatile bool mAlreadyExited = false;
        std::mutex mDoneMutex;
        std::condition_variable mDoneCV;

        nk_context mNuklearContext = nk_context();
        std::unique_ptr<NuklearDevice> mNuklearGraphicsData;
        std::unique_ptr<Render::SpriteGroup> mNuklearFontTexture;

        std::atomic<std::int64_t> mWidthHeightTmp;
        std::unique_ptr<CelFontInfo> mSmallTextFont, mBigTGoldFont;
        std::map<int, std::unique_ptr<PcxFontInfo>> mGoldFont, mSilverFont;
    };
}
