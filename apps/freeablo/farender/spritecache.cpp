#include "spritecache.h"

#include <assert.h>

#include <sstream>

#include <cel/celfile.h>


namespace FARender
{
    SpriteCache::SpriteCache(size_t size)
        :mNextCacheIndex(1)
        ,mCurrentSize(0)
        ,mMaxSize(size)
    {}

    FASpriteGroup SpriteCache::get(const std::string& path)
    {
        if(!mStrToCache.count(path))
        {
            FASpriteGroup newCacheEntry;
            size_t cacheIndex = newUniqueIndex();
            newCacheEntry.spriteCacheIndex = cacheIndex;
            Render::getImageInfo(path, newCacheEntry.width, newCacheEntry.height, newCacheEntry.animLength, 0);

            mStrToCache[path] = newCacheEntry;
            mCacheToStr[cacheIndex] = path;
        }

        return mStrToCache[path];
    }

    FASpriteGroup SpriteCache::getTileset(const std::string& celPath, const std::string& minPath, bool top)
    {
        std::stringstream ss;
        ss << celPath << ":::" << minPath << ":::" << top;
        std::string key = ss.str();

        if(!mStrToTilesetCache.count(key))
        {
            FASpriteGroup newCacheEntry;
            size_t cacheIndex = newUniqueIndex();
            newCacheEntry.spriteCacheIndex = cacheIndex;
            mStrToTilesetCache[key] = newCacheEntry;
            mCacheToTilesetPath[cacheIndex] = TilesetPath(celPath, minPath, top);
        }

        return mStrToTilesetCache[key];
    }

    size_t SpriteCache::newUniqueIndex()
    {
        return mNextCacheIndex++;
    }

    void SpriteCache::directInsert(Render::SpriteGroup* sprite, size_t cacheIndex)
    {
        if(mCurrentSize >= mMaxSize)
                evict();

        mUsedList.push_front(cacheIndex);
        mCache[cacheIndex] = CacheEntry(sprite, mUsedList.begin(), true);

        mCurrentSize++;
    }

    Render::SpriteGroup* SpriteCache::get(size_t index)
    {
        if(!mCache.count(index))
        {
            if(mCurrentSize >= mMaxSize)
                evict();

            Render::SpriteGroup* newSprite = NULL;

            if(mCacheToStr.count(index))
            {
                //TODO: replace mCacheToStr[index] with map.at(), to guarantee thread safety (once we switch to c++11)
                // until then, it is safe in practice.
                newSprite = Render::loadSprite(mCacheToStr[index]);
            }
            else if(mCacheToTilesetPath.count(index))
            {
                TilesetPath p = mCacheToTilesetPath[index]; //TODO: same as above
                newSprite = Render::loadTilesetSprite(p.celPath, p.minPath, p.top);
            }
            else
            {
                std::cerr << "ERROR INVALID SPRITE CACHE REQUEST " << index << std::endl;
            }

            mUsedList.push_front(index);
            mCache[index] = CacheEntry(newSprite, mUsedList.begin(), false);
            mCurrentSize++;
        }
        else
        {
            moveToFront(index);
        }

        return mCache[index].sprite;
    }

    void SpriteCache::moveToFront(size_t index)
    {
        mUsedList.erase(mCache[index].it);
        mUsedList.push_front(index);
        mCache[index].it = mUsedList.begin();
    }

    void SpriteCache::setImmortal(size_t index, bool immortal)
    {
        get(index);
        mCache[index].immortal = immortal;
    }

    void SpriteCache::evict()
    {
        std::list<size_t>::reverse_iterator it;

        for(it = mUsedList.rbegin(); it != mUsedList.rend(); it++)
        {
            if(!mCache[*it].immortal)
                break;
        }

        assert(it != mUsedList.rend() && "no evictable slots found. This should never happen");

        CacheEntry toEvict = mCache[*it];

        toEvict.sprite->destroy();
        delete toEvict.sprite;

        mCache.erase(*it);
        mUsedList.erase(--(it.base()));
        mCurrentSize--;
    }

    void SpriteCache::clear()
    {
        for(std::list<size_t>::iterator it = mUsedList.begin(); it != mUsedList.end(); it++)
        {
            mCache[*it].sprite->destroy();
            delete mCache[*it].sprite;
        }
    }
}
