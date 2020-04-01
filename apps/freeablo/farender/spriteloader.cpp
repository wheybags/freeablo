#include "spriteloader.h"
#include "renderer.h"
#include <diabloexe/diabloexe.h>
#include <diabloexe/monster.h>
#include <fmt/format.h>
#include <misc/stringops.h>

namespace FARender
{
    SpriteLoader::SpriteLoader(const DiabloExe::DiabloExe& exe)
    {
        for (const auto& pair : exe.getMonsters())
        {
            const DiabloExe::Monster& monsterData = pair.second;

            std::string cl2PathFormat = monsterData.cl2Path;
            Misc::StringUtils::replace(cl2PathFormat, "%c", "{}");

            MonsterSpriteDefinition definition = {};
            definition.walk = {fmt::format(cl2PathFormat, 'w'), true};
            definition.idle = {fmt::format(cl2PathFormat, 'n'), true};
            definition.dead = {fmt::format(cl2PathFormat, 'd'), true};
            definition.attack = {fmt::format(cl2PathFormat, 'a'), true};
            definition.hit = {fmt::format(cl2PathFormat, 'h'), true};

            mSpritesToLoad.insert(definition.walk);
            mSpritesToLoad.insert(definition.idle);
            mSpritesToLoad.insert(definition.dead);
            mSpritesToLoad.insert(definition.attack);
            mSpritesToLoad.insert(definition.hit);

            mMonsterSpriteDefinitions[pair.first] = std::move(definition);
        }

        for (int32_t i = 0; i <= 2; i++)
        {
            std::string specialPath = fmt::format("levels/l{}data/l{}s.cel", i, i);
            if (i == 0)
                specialPath = "levels/towndata/towns.cel";

            SpriteDefinition definition{specialPath, true};
            mTilesetSpecials[i] = definition;
            mSpritesToLoad.insert(definition);
        }
    }

    void SpriteLoader::load()
    {
        Renderer* renderer = Renderer::get();

        for (const auto& definition : mSpritesToLoad)
        {
            // TODO: This is a temporary hack, once we have a proper data loader, we just won't specify these
            static std::unordered_set<std::string> badCelNames{
                "Monsters\\Golem\\Golemh.CL2",
                "Monsters\\Worm\\Wormh.CL2",
                "Monsters\\Unrav\\Unravw.CL2",
                "Monsters\\Golem\\Golemn.CL2",
                "Monsters\\Worm\\Wormd.CL2",
                "Monsters\\Worm\\Wormw.CL2",
                "Monsters\\Worm\\Wormn.CL2",
                "Monsters\\Worm\\Worma.CL2",
            };

            if (badCelNames.count(definition.path))
                continue;

            mLoadedSprites[definition] = renderer->loadImage(definition.path, definition.trim);
        }

        mSpritesToLoad.clear();

        for (int32_t i = 0; i <= 4; i++)
        {
            std::string celPath = fmt::format("levels/l{}data/l{}.cel", i, i);
            std::string minPath = fmt::format("levels/l{}data/l{}.min", i, i);

            if (i == 0)
            {
                celPath = "levels/towndata/town.cel";
                minPath = "levels/towndata/town.min";
            }

            {
                SpriteDefinition definition = {};
                definition.path = "virtual_diablo_tileset/top/" + std::to_string(i);
                definition.trim = true;
                mTilesetTops[i] = definition;
                mLoadedSprites[definition] = renderer->mSpriteManager.getTileset(celPath, minPath, true, definition.trim);
            }
            {
                SpriteDefinition definition = {};
                definition.path = "virtual_diablo_tileset/bottom/" + std::to_string(i);
                definition.trim = true;
                mTilesetBottoms[i] = definition;
                mLoadedSprites[definition] = renderer->mSpriteManager.getTileset(celPath, minPath, false, definition.trim);
            }
        }
    }
}