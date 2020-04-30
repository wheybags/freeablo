#pragma once
#include "../fasavegame/gameloader.h"
#include <Image/image.h>
#include <atomic>
#include <misc/misc.h>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace DiabloExe
{
    class DiabloExe;
}

namespace Render
{
    class AtlasTexture;
    class SpriteGroup;
}

namespace FARender
{
    class SpriteLoader
    {
    public:
        explicit SpriteLoader(const DiabloExe::DiabloExe& exe);
        void load();

        struct SpriteDefinition
        {
            std::string path;
            bool trim = true;
            std::string category = "default";

            bool operator==(const SpriteDefinition& other) const { return path == other.path && trim == other.trim && category == other.category; }

            bool operator<(const SpriteDefinition& other) const
            {
                if (path != other.path)
                    return path < other.path;
                if (trim != other.trim)
                    return trim < other.trim;
                return category < other.category;
            }

            struct Hash
            {
                std::size_t operator()(const SpriteDefinition& def) const { return std::hash<std::string>{}(def.path); }
            };

            bool empty() const { return path.empty(); }
            void clear() { path.clear(); }

            // TODO: these really shouldn't be saved / loaded, we are just using it as a temporary workaround until
            // a proper mod-based asset loading pipeline is built
            void save(Serial::Saver& saver) const
            {
                saver.save(path);
                saver.save(category);
                saver.save(trim);
            }

            void load(Serial::Loader& loader)
            {
                path = loader.load<std::string>();
                category = loader.load<std::string>();
                trim = loader.load<bool>();
            }
        };

        enum class GetSpriteFailAction
        {
            Error,
            ReturnNull,
        };
        Render::SpriteGroup* getSprite(const SpriteDefinition& definition, GetSpriteFailAction fail = GetSpriteFailAction::Error);

        // TODO: monster sprite definitions are here for now, this stuff will all be moved somewhere more appropriate when we have a modding layer
        struct MonsterSpriteDefinition
        {
            SpriteDefinition walk;
            SpriteDefinition idle;
            SpriteDefinition dead;
            SpriteDefinition attack;
            SpriteDefinition hit;
        };
        std::unordered_map<std::string, MonsterSpriteDefinition> mMonsterSpriteDefinitions;

        std::unordered_map<std::string, SpriteDefinition> mNpcIdleAnimations;
        std::unordered_map<uint8_t, std::vector<SpriteDefinition>> mMissileAnimations;
        std::unordered_map<std::string, SpriteDefinition> mItemDrops;

        std::unordered_map<int32_t, SpriteDefinition> mTilesetTops;
        std::unordered_map<int32_t, SpriteDefinition> mTilesetBottoms;
        std::unordered_map<int32_t, SpriteDefinition> mTilesetSpecials;

        struct PlayerSpriteKey
        {
        public:
            explicit PlayerSpriteKey(const std::unordered_map<std::string, std::string>& variables) : mapString(generate(variables)) {}

            bool operator==(const PlayerSpriteKey& other) const { return mapString == other.mapString; }
            struct Hash
            {
                std::size_t operator()(const PlayerSpriteKey& def) const { return std::hash<std::string>{}(def.mapString); }
            };

        private:
            static std::string generate(const std::unordered_map<std::string, std::string>& variables)
            {
                std::vector<std::string> keys;
                {
                    keys.reserve(variables.size());
                    for (const auto& pair : variables)
                        keys.emplace_back(pair.first);
                }

                std::sort(keys.begin(), keys.end());

                std::ostringstream ss;
                for (const auto& key : keys)
                    ss << key << "=" << variables.at(key) << ":";

                return ss.str();
            }

        public:
            const std::string mapString;
        };
        std::unordered_map<PlayerSpriteKey, SpriteDefinition, PlayerSpriteKey::Hash> mPlayerSpriteDefinitions;

        struct GuiSprites
        {
            // Warning: don't put anything but SpriteDefinitions in here, we use pointer arithmetic to iterate them

            SpriteDefinition blackTexture{Misc::getResourcesPath().str() + "/black.png", false, "gui"};
            SpriteDefinition textbox{"data/textbox.cel", false, "gui"};
            SpriteDefinition textboxWide{"data/textbox2.cel", false, "gui"};
            SpriteDefinition slider{"data/textslid.cel", false, "gui"};
            SpriteDefinition bottomMenu{"ctrlpan/panel8.cel", false, "gui"};
            SpriteDefinition bottomMenuButtons{"ctrlpan/panel8bu.cel", false, "gui"};
            SpriteDefinition bottomMenuBulbs{"ctrlpan/p8bulbs.cel", false, "gui"};
            SpriteDefinition spellIcons{"ctrlpan/spelicon.cel", false, "gui"};
            SpriteDefinition spellIconsSmall{"data/spelli2.cel", false, "gui"};
            SpriteDefinition inventoryBackground{"data/inv/inv.cel", false, "gui"};
            SpriteDefinition spellsBackground{"data/spellbk.cel", false, "gui"};
            SpriteDefinition spellsButtons{"data/spellbkb.cel", false, "gui"};
            SpriteDefinition characterBackground{"data/char.cel", false, "gui"};
            SpriteDefinition questsBackground{"data/quest.cel", false, "gui"};
            SpriteDefinition bigPentagramSpin{"data/pentspin.cel", false, "gui"};
            SpriteDefinition mediumPentagramSpin{"ui_art/focus.pcx&trans=0,255,0&vanim=30", false, "gui"};
            SpriteDefinition smallPentagramSpin{"data/pentspn2.cel", false, "gui"};
            SpriteDefinition goldSplitBackground{"ctrlpan/golddrop.cel", false, "gui"};
            SpriteDefinition mainMenuLogo{"ui_art/smlogo.pcx&trans=0,255,0&vanim=154", false, "gui"};
            SpriteDefinition pauseMenuLogo{"data/diabsmal.cel", false, "gui"};
            SpriteDefinition characterSelectPortraits{"ui_art/heros.pcx&vanim=76", false, "gui"};
            SpriteDefinition characterSelectBackground{"ui_art/selhero.pcx", false, "gui"};
            SpriteDefinition mainMenuBackground{"ui_art/mainmenu.pcx", false, "gui"};
            SpriteDefinition itemCursors{"data/inv/objcurs.cel", false, "gui"};
            SpriteDefinition smallTextFont{"ctrlpan/smaltext.cel&convertToSingleTexture", false, "gui"};
            SpriteDefinition bigTGoldFont{"data/bigtgold.cel&convertToSingleTexture", false, "gui"};
            SpriteDefinition fontGold16{"ui_art/font16g.pcx&trans=0,255,0&resize=256x256&tileSize=16x16", false, "gui"};
            SpriteDefinition fontGold24{"ui_art/font24g.pcx&trans=0,255,0&resize=384x416&tileSize=24x26", false, "gui"};
            SpriteDefinition fontGold30{"ui_art/font30g.pcx&trans=0,255,0&resize=512x496&tileSize=32x31", false, "gui"};
            SpriteDefinition fontGold42{"ui_art/font42g.pcx&trans=0,255,0&resize=640x672&tileSize=40x42", false, "gui"};
            SpriteDefinition fontSilver16{"ui_art/font16s.pcx&trans=0,255,0&resize=256x256&tileSize=16x16", false, "gui"};
            SpriteDefinition fontSilver24{"ui_art/font24s.pcx&trans=0,255,0&resize=384x416&tileSize=24x26", false, "gui"};
            SpriteDefinition fontSilver30{"ui_art/font30s.pcx&trans=0,255,0&resize=512x496&tileSize=32x31", false, "gui"};

            SpriteDefinition end__;
        } mGuiSprites;

    private:
        struct FinalImageData
        {
            std::unique_ptr<Image> image;
            std::string category;
            std::optional<Image::TrimmedData> trimmedData;
        };

        struct FinalImageDataFrames
        {
            std::vector<FinalImageData*> frames;
            int32_t animationLength = 0;
        };

        struct LoadedImagesData
        {
            std::vector<std::unique_ptr<FinalImageData>> allImages;
            std::unordered_map<SpriteDefinition, FinalImageDataFrames, SpriteDefinition::Hash> definitionToImageMap;
        };

        static LoadedImagesData loadImagesIntoCpuMemory(const std::unordered_set<SpriteDefinition, SpriteDefinition::Hash>& spritesToLoad,
                                                        std::atomic_int32_t& progress);

        void saveToCache(const filesystem::path& atlasDirectory) const;
        void loadFromCache(const filesystem::path& atlasDirectory);

        typedef std::array<uint8_t, 16> SpriteDefinitionsHash;
        static SpriteDefinitionsHash hashSpriteDefinitions(const std::vector<SpriteDefinition>& definitions);

    private:
        std::unordered_set<SpriteDefinition, SpriteDefinition::Hash> mSpritesToLoad;
        std::unordered_map<SpriteDefinition, std::unique_ptr<Render::SpriteGroup>, SpriteDefinition::Hash> mLoadedSprites;
        std::unique_ptr<Render::AtlasTexture> mAtlasTexture;

        const filesystem::path mAtlasDirectory = Misc::getResourcesPath() / "cache" / "atlas";
        static constexpr int32_t ATLAS_CACHE_VERSION = 1;
    };
}