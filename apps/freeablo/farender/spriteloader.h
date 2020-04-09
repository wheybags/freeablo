#pragma once
#include "../fasavegame/gameloader.h"
#include "spritegroup.h"
#include <misc/misc.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace DiabloExe
{
    class DiabloExe;
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

            bool operator==(const SpriteDefinition& other) const { return path == other.path && trim == other.trim; }
            struct Hash
            {
                std::size_t operator()(const SpriteDefinition& def) const { return std::hash<std::string>{}(def.path); }
            };

            bool empty() const { return path.empty(); }
            void clear() { path.clear(); }

            // TODO: these really shouldn't be saved / loaded, we are just using it as a temporary workaround until
            // a proper mod-based asset loading pipeline is built
            void save(FASaveGame::GameSaver& saver) const
            {
                saver.save(path);
                saver.save(trim);
            }

            void load(FASaveGame::GameLoader& loader)
            {
                path = loader.load<std::string>();
                trim = loader.load<bool>();
            }
        };

        enum class GetSpriteFailAction
        {
            Error,
            ReturnNull,
        };
        FASpriteGroup* getSprite(const SpriteDefinition& definition, GetSpriteFailAction fail = GetSpriteFailAction::Error);

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

            SpriteDefinition blackTexture{Misc::getResourcesPath().str() + "/black.png", false};
            SpriteDefinition textbox{"data/textbox.cel", false};
            SpriteDefinition textboxWide{"data/textbox2.cel", false};
            SpriteDefinition slider{"data/textslid.cel", false};
            SpriteDefinition bottomMenu{"ctrlpan/panel8.cel", false};
            SpriteDefinition bottomMenuButtons{"ctrlpan/panel8bu.cel", false};
            SpriteDefinition bottomMenuBulbs{"ctrlpan/p8bulbs.cel", false};
            SpriteDefinition spellIcons{"ctrlpan/spelicon.cel", false};
            SpriteDefinition spellIconsSmall{"data/spelli2.cel", false};
            SpriteDefinition inventoryBackground{"data/inv/inv.cel", false};
            SpriteDefinition spellsBackground{"data/spellbk.cel", false};
            SpriteDefinition spellsButtons{"data/spellbkb.cel", false};
            SpriteDefinition characterBackground{"data/char.cel", false};
            SpriteDefinition questsBackground{"data/quest.cel", false};
            SpriteDefinition bigPentagramSpin{"data/pentspin.cel", false};
            SpriteDefinition mediumPentagramSpin{"ui_art/focus.pcx&trans=0,255,0&vanim=30", false};
            SpriteDefinition smallPentagramSpin{"data/pentspn2.cel", false};
            SpriteDefinition goldSplitBackground{"ctrlpan/golddrop.cel", false};
            SpriteDefinition mainMenuLogo{"ui_art/smlogo.pcx&trans=0,255,0&vanim=154", false};
            SpriteDefinition pauseMenuLogo{"data/diabsmal.cel", false};
            SpriteDefinition characterSelectPortraits{"ui_art/heros.pcx&vanim=76", false};
            SpriteDefinition characterSelectBackground{"ui_art/selhero.pcx", false};
            SpriteDefinition mainMenuBackground{"ui_art/mainmenu.pcx", false};
            SpriteDefinition itemCursors{"data/inv/objcurs.cel", false};
            SpriteDefinition smallTextFont{"ctrlpan/smaltext.cel&convertToSingleTexture", false};
            SpriteDefinition bigTGoldFont{"data/bigtgold.cel&convertToSingleTexture", false};
            SpriteDefinition fontGold16{"ui_art/font16g.pcx&trans=0,255,0&resize=256x256&tileSize=16x16", false};
            SpriteDefinition fontGold24{"ui_art/font24g.pcx&trans=0,255,0&resize=384x416&tileSize=24x26", false};
            SpriteDefinition fontGold30{"ui_art/font30g.pcx&trans=0,255,0&resize=512x496&tileSize=32x31", false};
            SpriteDefinition fontGold42{"ui_art/font42g.pcx&trans=0,255,0&resize=640x672&tileSize=40x42", false};
            SpriteDefinition fontSilver16{"ui_art/font16s.pcx&trans=0,255,0&resize=256x256&tileSize=16x16", false};
            SpriteDefinition fontSilver24{"ui_art/font24s.pcx&trans=0,255,0&resize=384x416&tileSize=24x26", false};
            SpriteDefinition fontSilver30{"ui_art/font30s.pcx&trans=0,255,0&resize=512x496&tileSize=32x31", false};

            SpriteDefinition end__;
        } mGuiSprites;

    private:
        struct FinalImageData
        {
            Image image;
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
            std::unordered_map<FinalImageData*, Render::Sprite> imagesToSprites;
            std::unordered_map<SpriteDefinition, FinalImageDataFrames, SpriteDefinition::Hash> definitionToImageMap;
        };

        static LoadedImagesData loadImagesIntoCpuMemory(const std::unordered_set<SpriteDefinition, SpriteDefinition::Hash>& spritesToLoad);

    private:
        std::unordered_set<SpriteDefinition, SpriteDefinition::Hash> mSpritesToLoad;
        std::unordered_map<SpriteDefinition, FASpriteGroup*, SpriteDefinition::Hash> mLoadedSprites;
    };
}