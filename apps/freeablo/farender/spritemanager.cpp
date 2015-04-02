#include "spritemanager.h"

#include <cstring>

namespace FARender
{
    SpriteManager::SpriteManager(size_t cacheSize): mCache(cacheSize) {}

    FASpriteGroup SpriteManager::get(const std::string& path)
    {
        return mCache.get(path);
    }

    FASpriteGroup SpriteManager::getTileset(const std::string& celPath, const std::string& minPath, bool top)
    {
        return mCache.getTileset(celPath, minPath, top);
    }

    FASpriteGroup SpriteManager::getFromRaw(const uint8_t* source, size_t width, size_t height)
    {
        size_t size = (width*4)*height;

        uint8_t* buffer = new uint8_t[size];
        memcpy(buffer, source, size);

        size_t index = mCache.newUniqueIndex();
        RawCacheTmp rawTmp;
        rawTmp.buffer = buffer;
        rawTmp.width = width;
        rawTmp.height = height;

        mRawCache[index] = rawTmp;

        FASpriteGroup retval;
        retval.spriteCacheIndex = index;
        retval.animLength = 1;
        retval.width = width;
        retval.height = height;

        return retval;
    }

    Render::SpriteGroup* SpriteManager::get(size_t index)
    {
        if(mRawCache.count(index))
        {
            RawCacheTmp tmp = mRawCache[index];

            Render::SpriteGroup* newSprite = Render::loadSprite(tmp.buffer, tmp.width, tmp.height);
            delete[] tmp.buffer;

            mCache.directInsert(newSprite, index);
            mRawCache.erase(index);

            return newSprite;
        }

        return mCache.get(index);
    }

    void SpriteManager::setImmortal(size_t index, bool immortal)
    {
        mCache.setImmortal(index, immortal);
    }

    void SpriteManager::clear()
    {
        mCache.clear();
    }
}
