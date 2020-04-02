#pragma once
#include "spritecache.h"
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

    private:
        std::unordered_set<SpriteDefinition, SpriteDefinition::Hash> mSpritesToLoad;
        std::unordered_map<SpriteDefinition, FASpriteGroup*, SpriteDefinition::Hash> mLoadedSprites;
    };
}