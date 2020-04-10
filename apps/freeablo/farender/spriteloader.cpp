#include "spriteloader.h"
#include "renderer.h"
#include <cel/celfile.h>
#include <cel/tilesetimage.h>
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

        for (int32_t i = 0; i <= 4; i++)
        {
            {
                SpriteDefinition definition = {};
                definition.path = "virtual_diablo_tileset/top/" + std::to_string(i);
                definition.trim = true;
                mTilesetTops[i] = definition;
                mSpritesToLoad.insert(definition);
            }
            {
                SpriteDefinition definition = {};
                definition.path = "virtual_diablo_tileset/bottom/" + std::to_string(i);
                definition.trim = true;
                mTilesetBottoms[i] = definition;
                mSpritesToLoad.insert(definition);
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
        printf("Decoding + trimming sprites...\n");
        // Load and trim all the sprites
        LoadedImagesData loadedImagesData = loadImagesIntoCpuMemory(mSpritesToLoad);
        mSpritesToLoad.clear();
        printf("done\n");

        printf("Sorting sprites...\n");
        // Sort the images by size
        std::sort(loadedImagesData.allImages.begin(),
                  loadedImagesData.allImages.end(),
                  [](const std::unique_ptr<FinalImageData>& a, const std::unique_ptr<FinalImageData>& b) { return a->image.height() > b->image.height(); });
        printf("done\n");

        printf("Uploading sprites to texture atlas...\n");
        // Upload the sprites into the texture atlas
        for (auto& image : loadedImagesData.allImages)
        {
            auto sprite = (Render::Sprite)(intptr_t)Render::atlasTexture->addTexture(image->image, false, image->trimmedData);
            loadedImagesData.imagesToSprites[image.get()] = sprite;
        }
        printf("done\n");

        Render::atlasTexture->printUtilisation();

        // Rebuild the sprite groups into collections of frames (where each frame is a reference to the atlas)
        for (auto& pair : loadedImagesData.definitionToImageMap)
        {
            const SpriteDefinition& definition = pair.first;
            FinalImageDataFrames& definitionFrames = pair.second;

            std::vector<Render::Sprite> finalSprites;

            for (FinalImageData* frame : definitionFrames.frames)
            {
                Render::Sprite sprite = loadedImagesData.imagesToSprites[frame];
                finalSprites.push_back(sprite);
            }

            auto newSprite = std::make_unique<Render::SpriteGroup>(std::move(finalSprites), definitionFrames.animationLength);

            auto* spriteGroup = new FASpriteGroup();
            spriteGroup->init(std::move(newSprite));
            mLoadedSprites[definition] = spriteGroup;
        }
    }

    SpriteLoader::LoadedImagesData SpriteLoader::loadImagesIntoCpuMemory(const std::unordered_set<SpriteDefinition, SpriteDefinition::Hash>& spritesToLoad)
    {
        std::vector<std::unique_ptr<FinalImageData>> allImages;
        std::unordered_map<FinalImageData*, Render::Sprite> imagesToSprites;
        std::unordered_map<SpriteDefinition, FinalImageDataFrames, SpriteDefinition::Hash> definitionToImageMap;

        for (const auto& definition : spritesToLoad)
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

            std::vector<Image> finalImages;
            int32_t animationLength = 0;

            if (Misc::StringUtils::startsWith(definition.path, "virtual_diablo_tileset/"))
            {
                std::vector<std::string> tilesetComponents = Misc::StringUtils::split(definition.path, '/');
                release_assert(tilesetComponents.size() == 3);

                bool top = tilesetComponents[1] == "top";
                int32_t i = std::stoi(tilesetComponents[2]);

                std::string celPath = fmt::format("levels/l{}data/l{}.cel", i, i);
                std::string minPath = fmt::format("levels/l{}data/l{}.min", i, i);

                if (i == 0)
                {
                    celPath = "levels/towndata/town.cel";
                    minPath = "levels/towndata/town.min";
                }

                finalImages = Cel::loadTilesetImage(celPath, minPath, top);
                //                auto* spriteGroup = new FASpriteGroup();
                //                auto realSpriteGroup = std::unique_ptr<Render::SpriteGroup>(Render::loadTilesetSprite(celPath, minPath, true,
                //                definition.trim)); spriteGroup->init(std::move(realSpriteGroup)); mLoadedSprites[definition] = spriteGroup;
            }
            else if (vAnim != 0)
            {
                Image original = Render::loadNonCelImageTrans(sourcePath, hasTrans, r, g, b);

                std::vector<Render::Sprite> vec;

                for (size_t srcY = 0; srcY < (size_t)original.height() - 1; srcY += vAnim)
                {
                    Image tmp(original.width(), vAnim);

                    for (size_t x = 0; x < (size_t)original.width(); x++)
                    {
                        for (size_t y = 0; y < vAnim; y++)
                        {
                            if (srcY + y < (size_t)original.height())
                            {
                                Cel::Colour px = original.get(x, srcY + y);
                                tmp.get(x, y) = px;
                            }
                        }
                    }

                    finalImages.push_back(std::move(tmp));
                }
                animationLength = int32_t(finalImages.size());
            }
            else if (resize)
            {
                Image original = Render::loadNonCelImageTrans(sourcePath, hasTrans, r, g, b);

                Image tmp(newWidth, newHeight);

                size_t srcX = 0;
                size_t srcY = 0;
                size_t dstX = 0;
                size_t dstY = 0;

                while (true)
                {
                    for (size_t y = 0; y < tileHeight; y += 1)
                    {
                        for (size_t x = 0; x < tileWidth; x += 1)
                        {
                            Cel::Colour px = original.get(srcX + x, srcY + y);
                            tmp.get(dstX + x, dstY + y) = px;
                        }
                    }

                    srcX += tileWidth;
                    if (srcX >= (size_t)original.width())
                    {
                        srcX = 0;
                        srcY += tileHeight;
                    }

                    if (srcY >= (size_t)original.height())
                        break;

                    dstX += tileWidth;
                    if (dstX >= newWidth)
                    {
                        dstX = 0;
                        dstY += tileHeight;
                    }

                    if (dstY >= newHeight)
                        break;
                }

                finalImages.push_back(std::move(tmp));
                animationLength = 1;
            }
            else if (convertToSingleTexture)
            {
                Cel::CelFile cel(sourcePath);
                std::vector<Image> images = cel.decode();

                int32_t width = 0;
                int32_t height = 0;

                for (int32_t i = 0; i < cel.numFrames(); i++)
                {
                    width += images[i].width();
                    height = (images[i].height() > height ? images[i].height() : height);
                }

                debug_assert(width > 0);
                debug_assert(height > 0);

                Image finalImage(width, height);

                int32_t x = 0;
                for (int32_t i = 0; i < cel.numFrames(); i++)
                {
                    images[i].blitTo(finalImage, x, 0);
                    x += images[i].width();
                }

                finalImages.push_back(std::move(finalImage));
                animationLength = 1;
            }
            else
            {
                std::string extension = Misc::StringUtils::getFileExtension(sourcePath);
                if (Misc::StringUtils::ciEqual(extension, "cel") || Misc::StringUtils::ciEqual(extension, "cl2"))
                {
                    Cel::CelFile cel(sourcePath);
                    finalImages = cel.decode();
                    animationLength = cel.animLength();
                }
                else
                {
                    animationLength = 1;
                    finalImages.push_back(Render::loadNonCelImageTrans(sourcePath, hasTrans, r, g, b));
                }
            }

            FinalImageDataFrames& definitionFrames = definitionToImageMap[definition];
            definitionFrames.animationLength = animationLength;

            for (auto& image : finalImages)
            {
                std::unique_ptr<FinalImageData> finalImageData = std::make_unique<FinalImageData>();

                if (definition.trim)
                {
                    std::pair<Image, Image::TrimmedData> tmp = image.trimTransparentEdges();
                    finalImageData->image = std::move(tmp.first);
                    finalImageData->trimmedData = tmp.second;
                }
                else
                {
                    finalImageData->image = std::move(image);
                }

                definitionFrames.frames.push_back(finalImageData.get());
                allImages.emplace_back(std::move(finalImageData));
            }
        }

        return LoadedImagesData{std::move(allImages), std::move(imagesToSprites), std::move(definitionToImageMap)};
    }
}