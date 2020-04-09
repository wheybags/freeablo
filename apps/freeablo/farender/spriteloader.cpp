#include "spriteloader.h"
#include "renderer.h"
#include <diabloexe/baseitem.h>
#include <diabloexe/diabloexe.h>
#include <diabloexe/monster.h>
#include <diabloexe/npc.h>
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

        for (const DiabloExe::Npc* npc : exe.getNpcs())
        {
            SpriteDefinition definition{npc->celPath, true};
            mNpcIdleAnimations[npc->id] = definition;
            mSpritesToLoad.insert(definition);
        }

        for (const auto& pair : exe.getMissileGraphicsTable())
        {
            const DiabloExe::MissileGraphics& missileGraphics = pair.second;
            if (missileGraphics.mNumAnimationFiles == 0 || missileGraphics.mFilename == " ")
                continue;

            std::vector<SpriteDefinition> missileDirections;

            if (missileGraphics.mNumAnimationFiles > 1)
            {
                for (uint32_t i = 0; i < missileGraphics.mNumAnimationFiles; i++)
                    missileDirections.emplace_back(SpriteDefinition{"missiles/" + missileGraphics.mFilename + std::to_string(i + 1) + ".cl2", true});
            }
            else
            {
                missileDirections.emplace_back(SpriteDefinition{"missiles/" + missileGraphics.mFilename + ".cl2", true});
            }

            for (const auto& definition : missileDirections)
                mSpritesToLoad.insert(definition);

            mMissileAnimations[pair.first] = std::move(missileDirections);
        }

        for (const auto& item : exe.getBaseItems())
        {
            SpriteDefinition definition{item.dropItemGraphicsPath, true};
            mItemDrops[item.idName] = definition;
            mSpritesToLoad.insert(definition);
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

        static std::vector<std::pair<std::string, std::string>> classes{{"warrior", "w"}, {"rogue", "r"}, {"sorceror", "s"}};
        static std::vector<std::pair<std::string, std::string>> armors{{"none", "l"}, {"heavy-armor", "h"}, {"medium-armor", "m"}, {"light-armor", "l"}};
        static std::vector<std::pair<std::string, std::string>> weapons{{"none", "n"},
                                                                        {"sword-1h", "s"},
                                                                        {"axe-1h", "s"},
                                                                        {"mace-1h", "m"},
                                                                        {"none-shield", "u"},
                                                                        {"sword-1h-shield", "d"},
                                                                        {"axe-1h-shield", "d"},
                                                                        {"mace-1h-shield", "h"},
                                                                        {"bow-2h", "b"},
                                                                        {"axe-2h", "a"},
                                                                        {"staff-2h", "t"}};

        static std::vector<std::pair<std::string, std::string>> animations{{"dead", "dt"},
                                                                           {"attack", "at"},
                                                                           {"hit", "ht"},
                                                                           {"cast-lightning", "lm"},
                                                                           {"cast-fire", "fm"},
                                                                           {"cast-magic", "qm"},
                                                                           {"block", "bl"},
                                                                           {"walk-town", "wl"},
                                                                           {"walk-dungeon", "aw"},
                                                                           {"idle-town", "st"},
                                                                           {"idle-dungeon", "as"}};

        for (const auto& classPair : classes)
        {
            for (const auto& armorPair : armors)
            {
                for (const auto& weaponPair : weapons)
                {
                    for (const auto& animationPair : animations)
                    {
                        const std::string& className = classPair.first;
                        std::string classNameSpriteCode = classPair.first;
                        std::string classSpriteCode = classPair.second;

                        const std::string& armorName = armorPair.first;
                        std::string armorSpriteCode = armorPair.second;

                        const std::string& weaponName = weaponPair.first;
                        std::string weaponSpriteCode = weaponPair.second;

                        const std::string& animationName = animationPair.first;
                        std::string animationSpriteCode = animationPair.second;

                        // temporary hack
                        classNameSpriteCode = "warrior";
                        classSpriteCode = "w";
                        armorSpriteCode = "l";

                        if (animationName == "dead")
                            weaponSpriteCode = "n"; // no weapons in death anims

                        if (animationName == "block" && !Misc::StringUtils::endsWith(weaponName, "-shield"))
                            animationSpriteCode = "ht"; // There are no block animations without shields, use hit as a placeholder

                        std::string spritePath = fmt::format(FMT_STRING("plrgfx/{}/{}{}{}/{}{}{}{}.cl2"),
                                                             classNameSpriteCode,

                                                             classSpriteCode,
                                                             armorSpriteCode,
                                                             weaponSpriteCode,

                                                             classSpriteCode,
                                                             armorSpriteCode,
                                                             weaponSpriteCode,
                                                             animationSpriteCode);

                        PlayerSpriteKey spriteKey{{{"class", className}, {"armor", armorName}, {"weapon", weaponName}, {"animation", animationName}}};
                        SpriteDefinition definition{spritePath, true};

                        mPlayerSpriteDefinitions[spriteKey] = definition;
                        mSpritesToLoad.insert(definition);
                    }
                }
            }
        }

        for (auto guiSpriteIt = reinterpret_cast<SpriteDefinition*>(&mGuiSprites); guiSpriteIt != &mGuiSprites.end__; guiSpriteIt++)
            mSpritesToLoad.insert(*guiSpriteIt);
    }

    FASpriteGroup* SpriteLoader::getSprite(const SpriteDefinition& definition, GetSpriteFailAction fail)
    {
        if (fail == GetSpriteFailAction::Error)
            return mLoadedSprites.at(definition);

        auto it = mLoadedSprites.find(definition);
        if (it == mLoadedSprites.end())
            return nullptr;

        return it->second;
    }

    void SpriteLoader::load()
    {
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

            std::vector<std::string> components = Misc::StringUtils::split(definition.path, '&');
            std::string sourcePath = components[0];

            uint32_t vAnim = 0;
            bool hasTrans = false;
            bool resize = false;
            bool convertToSingleTexture = false;
            uint32_t tileWidth = 0;
            uint32_t tileHeight = 0;
            uint32_t newWidth = 0;
            uint32_t newHeight = 0;
            uint32_t r = 0, g = 0, b = 0;
            int32_t celIndex;

            for (uint32_t i = 1; i < components.size(); i++)
            {
                std::vector<std::string> pair = Misc::StringUtils::split(components[i], '=');

                if (pair[0] == "trans")
                {
                    std::vector<std::string> rgbStr = Misc::StringUtils::split(pair[1], ',');

                    hasTrans = true;

                    std::istringstream rss(rgbStr[0]);
                    rss >> r;

                    std::istringstream gss(rgbStr[1]);
                    gss >> g;

                    std::istringstream bss(rgbStr[2]);
                    bss >> b;
                }
                else if (pair[0] == "vanim")
                {
                    std::istringstream vanimss(pair[1]);

                    vanimss >> vAnim;
                }
                else if (pair[0] == "resize")
                {
                    resize = true;

                    std::vector<std::string> newSize = Misc::StringUtils::split(pair[1], 'x');

                    std::istringstream wss(newSize[0]);
                    wss >> newWidth;

                    std::istringstream hss(newSize[1]);
                    hss >> newHeight;
                }
                else if (pair[0] == "tileSize")
                {
                    std::vector<std::string> tileSize = Misc::StringUtils::split(pair[1], 'x');

                    std::istringstream wss(tileSize[0]);
                    wss >> tileWidth;

                    std::istringstream hss(tileSize[1]);
                    hss >> tileHeight;
                }
                else if (pair[0] == "convertToSingleTexture")
                {
                    convertToSingleTexture = true;
                }
                else if (pair[0] == "frame")
                {
                    std::istringstream ss(pair[1]);
                    ss >> celIndex;
                }
            }

            Render::SpriteGroup* newSprite = nullptr;

            if (vAnim != 0)
                newSprite = Render::loadVanimSprite(sourcePath, vAnim, hasTrans, r, g, b, definition.trim);
            else if (resize)
                newSprite = Render::loadResizedSprite(sourcePath, newWidth, newHeight, tileWidth, tileHeight, hasTrans, r, g, b, definition.trim);
            else if (convertToSingleTexture)
                newSprite = Render::loadCelToSingleTexture(sourcePath, definition.trim);
            else
                newSprite = Render::loadSprite(sourcePath, hasTrans, r, g, b, definition.trim);

            auto* spriteGroup = new FASpriteGroup();
            spriteGroup->init(newSprite);
            mLoadedSprites[definition] = spriteGroup;
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

                auto* spriteGroup = new FASpriteGroup();
                Render::SpriteGroup* realSpriteGroup = Render::loadTilesetSprite(celPath, minPath, true, definition.trim);
                spriteGroup->init(realSpriteGroup);
                mLoadedSprites[definition] = spriteGroup;
            }
            {
                SpriteDefinition definition = {};
                definition.path = "virtual_diablo_tileset/bottom/" + std::to_string(i);
                definition.trim = true;
                mTilesetBottoms[i] = definition;

                auto* spriteGroup = new FASpriteGroup();
                Render::SpriteGroup* realSpriteGroup = Render::loadTilesetSprite(celPath, minPath, false, definition.trim);
                spriteGroup->init(realSpriteGroup);
                mLoadedSprites[definition] = spriteGroup;
            }
        }
    }
}