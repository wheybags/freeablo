#pragma once
#include "spritecache.h"
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
        };

        FASpriteGroup* getSprite(const SpriteDefinition& definition) { return mLoadedSprites.at(definition); }

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

            SpriteDefinition end__;
        } mGuiSprites;

    private:
        std::unordered_set<SpriteDefinition, SpriteDefinition::Hash> mSpritesToLoad;
        std::unordered_map<SpriteDefinition, FASpriteGroup*, SpriteDefinition::Hash> mLoadedSprites;
    };
}