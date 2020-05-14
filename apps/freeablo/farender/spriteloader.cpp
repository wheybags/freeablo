#include "spriteloader.h"
#include "renderer.h"
#include <cel/celfile.h>
#include <cel/tilesetimage.h>
#include <diabloexe/baseitem.h>
#include <diabloexe/diabloexe.h>
#include <diabloexe/monster.h>
#include <diabloexe/npc.h>
#include <fmt/format.h>
#include <misc/md5.h>
#include <misc/stringops.h>
#include <render/spritegroup.h>
#include <serial/textstream.h>
#include <thread>

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
                        const std::string& classNameSpriteCode = classPair.first;
                        const std::string& classSpriteCode = classPair.second;

                        const std::string& armorName = armorPair.first;
                        const std::string& armorSpriteCode = armorPair.second;

                        const std::string& weaponName = weaponPair.first;
                        std::string weaponSpriteCode = weaponPair.second;

                        const std::string& animationName = animationPair.first;
                        std::string animationSpriteCode = animationPair.second;

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
                        SpriteDefinition definition{spritePath, true, "player"};

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

    Render::SpriteGroup* SpriteLoader::getSprite(const SpriteDefinition& definition, GetSpriteFailAction fail)
    {
        if (fail == GetSpriteFailAction::Error)
            return mLoadedSprites.at(definition).get();

        auto it = mLoadedSprites.find(definition);
        if (it == mLoadedSprites.end())
            return nullptr;

        return it->second.get();
    }

    void SpriteLoader::load()
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

        for (auto it = mSpritesToLoad.begin(); it != mSpritesToLoad.end();)
        {
            if (badCelNames.count(it->path))
                it = mSpritesToLoad.erase(it);
            else
                ++it;
        }

        Render::setWindowTitle(Render::getWindowTitle() + ", trying to load sprites from cache...");

        bool loadedFromCache = false;
        try
        {
            loadFromCache(mAtlasDirectory);
            loadedFromCache = true;
        }
        catch (std::runtime_error&)
        {
        }

        if (loadedFromCache)
        {
            mSpritesToLoad.clear();
            return;
        }

        // Load images into cpu memory + trim
        LoadedImagesData loadedImagesData;
        {
            auto start = std::chrono::steady_clock::now();

            size_t numWorkers = std::max(std::thread::hardware_concurrency(), 1u);

            printf("Decoding + trimming sprites with %d %s...\n", int(numWorkers), numWorkers > 1 ? "threads" : "thread");

            size_t spritesToLoadTotal = mSpritesToLoad.size();

            std::vector<std::thread> workers;
            std::vector<LoadedImagesData> threadResults(numWorkers);
            std::vector<std::atomic_int> progressCounts(numWorkers);
            std::condition_variable conditionVariable;

            for (size_t i = 0; i < numWorkers; i++)
            {
                std::unordered_set<SpriteDefinition, SpriteDefinition::Hash> thisThreadImages;
                for (size_t j = 0; j < size_t(ceil(double(spritesToLoadTotal) / numWorkers)) && !mSpritesToLoad.empty(); j++)
                    thisThreadImages.emplace(mSpritesToLoad.extract(mSpritesToLoad.begin()).value());

                workers.emplace_back(
                    [results(&threadResults[i]), thisThreadImages(std::move(thisThreadImages)), resultCounter(&progressCounts[i]), &conditionVariable] {
                        *results = loadImagesIntoCpuMemory(thisThreadImages, *resultCounter);
                        conditionVariable.notify_one();
                    });
            }

            release_assert(mSpritesToLoad.empty());

            std::mutex mutex;
            while (true)
            {
                std::unique_lock lock(mutex);
                conditionVariable.wait_for(lock, std::chrono::milliseconds(500));

                size_t sum = 0;
                for (const auto& item : progressCounts)
                    sum += size_t(item);

                if (sum == spritesToLoadTotal)
                    break;

                int32_t percentage = int32_t(floorf((float(sum) / spritesToLoadTotal) * 100));
                percentage = std::min(percentage, 99);

                Render::setWindowTitle(Render::getWindowTitle() + ", Loading Sprites: " + std::to_string(percentage) + "%");
            }

            // Just to be sure the threads are done
            for (auto& thread : workers)
                thread.join();

            for (auto& result : threadResults)
            {
                loadedImagesData.allImages.insert(
                    loadedImagesData.allImages.end(), std::make_move_iterator(result.allImages.begin()), std::make_move_iterator(result.allImages.end()));

                while (!result.definitionToImageMap.empty())
                    loadedImagesData.definitionToImageMap.insert(result.definitionToImageMap.extract(result.definitionToImageMap.begin()));
            }

            mSpritesToLoad.clear();

            auto elapsed = std::chrono::steady_clock::now() - start;
            int32_t milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

            float seconds = float(milliseconds) / 1000.0f;
            printf("done in %.1f seconds\n", seconds);
        }

        printf("Sorting sprites...\n");
        std::sort(loadedImagesData.allImages.begin(),
                  loadedImagesData.allImages.end(),
                  [](const std::unique_ptr<FinalImageData>& a, const std::unique_ptr<FinalImageData>& b) {
                      int32_t aHeight = a->trimmedData ? a->trimmedData->trimmedHeight : a->image->height();
                      int32_t bHeight = b->trimmedData ? b->trimmedData->trimmedHeight : b->image->height();

                      return aHeight > bHeight;
                  });
        printf("done\n");

        std::unordered_map<const Image*, const Render::TextureReference*> imagesToSprites;

        printf("Uploading sprites to texture atlas...\n");
        {
            mAtlasTexture = std::make_unique<Render::AtlasTexture>(*Render::mainRenderInstance, *Render::mainCommandQueue);

            Render::AtlasTexture::SpriteLoadInputMap imagesByCategory;

            for (auto& image : loadedImagesData.allImages)
                imagesByCategory[image->category].push_back(Render::AtlasTexture::LoadImageData{*image->image, image->trimmedData});

            Render::AtlasTexture::SpriteLoadResultMap loadedSpritesByCategory = mAtlasTexture->loadSprites(imagesByCategory);

            for (const auto& pair : loadedSpritesByCategory)
            {
                const std::string& category = pair.first;
                const std::vector<Render::AtlasTexture::LoadImageData>& images = imagesByCategory[category];
                const std::vector<NonNullConstPtr<Render::TextureReference>>& sprites = pair.second;

                for (size_t index = 0; index < sprites.size(); index++)
                    imagesToSprites[&images[index].image] = sprites[index].get();
            }
        }
        printf("done\n");

        mAtlasTexture->printUtilisation();

        // Rebuild the sprite groups into collections of frames (where each frame is a reference to the atlas)
        for (auto& pair : loadedImagesData.definitionToImageMap)
        {
            const SpriteDefinition& definition = pair.first;
            FinalImageDataFrames& definitionFrames = pair.second;

            std::vector<const Render::TextureReference*> finalSprites;

            for (FinalImageData* frame : definitionFrames.frames)
            {
                const Render::TextureReference* sprite = imagesToSprites[frame->image.get()];
                finalSprites.push_back(sprite);
            }

            mLoadedSprites[definition] = std::make_unique<Render::SpriteGroup>(std::move(finalSprites), definitionFrames.animationLength);
        }

        Render::setWindowTitle(Render::getWindowTitle() + ", saving sprite cache...");
        saveToCache(mAtlasDirectory);

        Render::setWindowTitle(Render::getWindowTitle());
    }

    void SpriteLoader::saveToCache(const filesystem::path& atlasDirectory) const
    {
        if (atlasDirectory.exists())
            release_assert(filesystem::remove_all(atlasDirectory));
        filesystem::create_directories(atlasDirectory);

        mAtlasTexture->saveTexturesToCache(atlasDirectory);

        std::vector<SpriteDefinition> allSpriteDefinitions;
        {
            for (const auto& pair : mLoadedSprites)
                allSpriteDefinitions.push_back(pair.first);

            std::sort(allSpriteDefinitions.begin(), allSpriteDefinitions.end());
        }

        Serial::TextWriteStream stream;
        Serial::Saver saver(stream);

        saver.save(ATLAS_CACHE_VERSION);

        SpriteDefinitionsHash definitionsHash = hashSpriteDefinitions(allSpriteDefinitions);
        for (const auto& byte : definitionsHash)
            saver.save(byte);

        saver.save(uint32_t(allSpriteDefinitions.size()));

        for (const auto& definition : allSpriteDefinitions)
        {
            Serial::ScopedCategory definitionCat("DEF", saver);

            definition.save(saver);

            const Render::SpriteGroup* sprite = mLoadedSprites.at(definition).get();

            saver.save(sprite->getAnimationLength());
            saver.save(sprite->size());

            for (int32_t i = 0; i < sprite->size(); i++)
            {
                Serial::ScopedCategory frameCat("Frame_" + std::to_string(i), saver);

                const Render::TextureReference* frame = sprite->getFrame(i);

                saver.save(frame->mX);
                saver.save(frame->mY);
                saver.save(frame->mWidth);
                saver.save(frame->mHeight);
                saver.save(frame->mTrimmedOffsetX);
                saver.save(frame->mTrimmedOffsetY);
                saver.save(frame->mTrimmedWidth);
                saver.save(frame->mTrimmedHeight);

                int32_t textureIndex = -1;
                {
                    const std::vector<Render::AtlasTexture::Layer>& layers = mAtlasTexture->getLayersByCategory().at(definition.category).layers;

                    for (int32_t j = 0; j < int32_t(layers.size()); j++)
                    {
                        if (frame->mTexture == layers[j].texture.get())
                        {
                            textureIndex = j;
                            break;
                        }
                    }

                    release_assert(textureIndex != -1);
                }

                saver.save(textureIndex);
            }
        }

        std::pair<const uint8_t*, size_t> data = stream.getData();

        FILE* f = fopen((atlasDirectory / "data.txt").str().c_str(), "wb");
        fwrite(data.first, 1, data.second, f);
        fclose(f);
    }

    void SpriteLoader::loadFromCache(const filesystem::path& atlasDirectory)
    {
        if (!atlasDirectory.exists())
            throw std::runtime_error("no cache to load");

        std::string data;
        {
            if (!filesystem::exists(atlasDirectory / "data.txt"))
                throw std::runtime_error("missing data.txt");

            FILE* f = fopen((atlasDirectory / "data.txt").str().c_str(), "rb");
            fseek(f, 0, SEEK_END);
            size_t size = ftell(f);
            fseek(f, 0, SEEK_SET);
            data.resize(size);
            fread(data.data(), 1, size, f);
            fclose(f);
        }

        Serial::TextReadStream stream(data);
        Serial::Loader loader(stream);

        int32_t cachedVersion = loader.load<int32_t>();

        if (cachedVersion != ATLAS_CACHE_VERSION)
            throw std::runtime_error("wrong atlas cache version");

        SpriteDefinitionsHash spritesToLoadDefinitionsHash;
        {
            std::vector<SpriteDefinition> allSpriteDefinitions;

            for (const auto& definition : mSpritesToLoad)
                allSpriteDefinitions.push_back(definition);

            std::sort(allSpriteDefinitions.begin(), allSpriteDefinitions.end());

            spritesToLoadDefinitionsHash = hashSpriteDefinitions(allSpriteDefinitions);
        }

        SpriteDefinitionsHash cachedSpriteDefinitionsHash;
        for (auto& byte : cachedSpriteDefinitionsHash)
            byte = loader.load<uint8_t>();

        if (cachedSpriteDefinitionsHash != spritesToLoadDefinitionsHash)
            throw std::runtime_error("sprite definitions have changed");

        mAtlasTexture = std::make_unique<Render::AtlasTexture>(*Render::mainRenderInstance, *Render::mainCommandQueue);
        mAtlasTexture->loadTexturesFromCache(atlasDirectory);

        uint32_t definitionCount = loader.load<uint32_t>();
        std::vector<std::unique_ptr<Render::TextureReference>> allAtlasEntries;

        for (uint32_t definitionIndex = 0; definitionIndex < definitionCount; definitionIndex++)
        {
            SpriteDefinition definition;
            definition.load(loader);

            int32_t animationLength = loader.load<int32_t>();
            int32_t spriteSize = loader.load<int32_t>();

            std::vector<const Render::TextureReference*> frames;
            const std::vector<Render::AtlasTexture::Layer>& layers = mAtlasTexture->getLayersByCategory().at(definition.category).layers;

            for (int32_t frameIndex = 0; frameIndex < spriteSize; frameIndex++)
            {
                auto frame = std::unique_ptr<Render::TextureReference>(new Render::TextureReference(Render::TextureReference::Tag{}));

                frame->mX = loader.load<int32_t>();
                frame->mY = loader.load<int32_t>();
                frame->mWidth = loader.load<int32_t>();
                frame->mHeight = loader.load<int32_t>();
                frame->mTrimmedOffsetX = loader.load<int32_t>();
                frame->mTrimmedOffsetY = loader.load<int32_t>();
                frame->mTrimmedWidth = loader.load<int32_t>();
                frame->mTrimmedHeight = loader.load<int32_t>();

                int32_t textureIndex = loader.load<int32_t>();

                if (int32_t(layers.size()) < textureIndex)
                    throw std::runtime_error("missing layer");

                frame->mTexture = layers[textureIndex].texture.get();

                frames.push_back(frame.get());
                allAtlasEntries.push_back(std::move(frame));
            }

            mLoadedSprites[definition] = std::make_unique<Render::SpriteGroup>(std::move(frames), animationLength);
        }

        mAtlasTexture->replaceTextureEntries(std::move(allAtlasEntries));

        // for debugging
        // saveToCache(Misc::getResourcesPath() / "cache" / "atlas_resaved");
    }

    SpriteLoader::SpriteDefinitionsHash SpriteLoader::hashSpriteDefinitions(const std::vector<SpriteDefinition>& definitions)
    {
        Serial::TextWriteStream stream;
        Serial::Saver saver(stream);

        saver.save(uint32_t(definitions.size()));
        for (const auto& definition : definitions)
            definition.save(saver);

        std::pair<uint8_t*, size_t> data = stream.getData();

        Misc::md5_state_t state;
        SpriteDefinitionsHash digest;

        Misc::md5_init(&state);
        Misc::md5_append(&state, reinterpret_cast<Misc::md5_byte_t*>(data.first), int(data.second));
        Misc::md5_finish(&state, reinterpret_cast<Misc::md5_byte_t*>(digest.data()));

        return digest;
    }

    static Image loadNonCelImageTrans(const std::string& path, bool hasTrans, size_t transR, size_t transG, size_t transB)
    {
        Image image = Image::loadFromFile(path);

        if (hasTrans)
        {
            for (int x = 0; x < image.width(); x++)
            {
                for (int y = 0; y < image.height(); y++)
                {
                    Cel::Colour& px = image.get(x, y);
                    if (px.r == transR && px.g == transG && px.b == transB)
                        px.a = 0;
                }
            }
        }

        return image;
    }

    SpriteLoader::LoadedImagesData SpriteLoader::loadImagesIntoCpuMemory(const std::unordered_set<SpriteDefinition, SpriteDefinition::Hash>& spritesToLoad,
                                                                         std::atomic_int32_t& progress)
    {
        std::vector<std::unique_ptr<FinalImageData>> allImages;
        std::unordered_map<SpriteDefinition, FinalImageDataFrames, SpriteDefinition::Hash> definitionToImageMap;

        for (const auto& definition : spritesToLoad)
        {
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
            int32_t celIndex = 0;

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

                Cel::TilesetImagePart imagePart = tilesetComponents[1] == "top" ? Cel::TilesetImagePart::Top : Cel::TilesetImagePart::Bottom;
                int32_t i = std::stoi(tilesetComponents[2]);

                std::string celPath = fmt::format("levels/l{}data/l{}.cel", i, i);
                std::string minPath = fmt::format("levels/l{}data/l{}.min", i, i);

                if (i == 0)
                {
                    celPath = "levels/towndata/town.cel";
                    minPath = "levels/towndata/town.min";
                }

                finalImages = Cel::loadTilesetImage(celPath, minPath, imagePart);
            }
            else if (vAnim != 0)
            {
                Image original = loadNonCelImageTrans(sourcePath, hasTrans, r, g, b);

                std::vector<const Render::TextureReference*> vec;

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
                Image original = loadNonCelImageTrans(sourcePath, hasTrans, r, g, b);

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
                    finalImages.push_back(loadNonCelImageTrans(sourcePath, hasTrans, r, g, b));
                }
            }

            FinalImageDataFrames& definitionFrames = definitionToImageMap[definition];
            definitionFrames.animationLength = animationLength;

            for (auto& image : finalImages)
            {
                std::unique_ptr<FinalImageData> finalImageData = std::make_unique<FinalImageData>();
                if (definition.trim)
                    finalImageData->trimmedData = image.calculateTrimTransparentEdges();

                finalImageData->category = definition.category;
                finalImageData->image = std::make_unique<Image>(std::move(image));

                definitionFrames.frames.push_back(finalImageData.get());
                allImages.emplace_back(std::move(finalImageData));
            }

            progress += 1;
        }

        return LoadedImagesData{std::move(allImages), std::move(definitionToImageMap)};
    }
}
