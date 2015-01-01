#include "spritecache.h"

#include <cel/celfile.h>


namespace FARender
{
    SpriteCache::SpriteCache(size_t size)
        :mNextCacheIndex(0)
        ,mCurrentSize(0)
        ,mMaxSize(size)
    {}

    FASpriteGroup SpriteCache::get(const std::string& path)
    {
        if(!mStrToCache.count(path))
        {
            Cel::CelFile cel(path); //TODO: shouldn't need to do all this IO here
            FASpriteGroup newCacheEntry(cel.animLength(), mNextCacheIndex);
            mStrToCache[path] = newCacheEntry;
            mCacheToStr[mNextCacheIndex] = path;
            mNextCacheIndex++;
        }

        return mStrToCache[path];
    }

    Render::SpriteGroup* SpriteCache::get(size_t index)
    {
        if(!mCache.count(index))
        {
            if(mCurrentSize >= mMaxSize)
                evict();

            //TODO: replace mCacheToStr[index] with map.at(), to guarantee thread safety (once we switch to c++11)
            // until then, it is safe in practice.
            Render::SpriteGroup* newSprite = new Render::SpriteGroup(mCacheToStr[index]);
            mUsedList.push_front(index);
            mCache[index] = std::pair<Render::SpriteGroup*, std::list<size_t>::iterator>(newSprite, mUsedList.begin());
            mCurrentSize++;
        }
        else
        {
            mUsedList.erase(mCache[index].second);
            mUsedList.push_front(index);
            mCache[index].second = mUsedList.begin();
        }

        return mCache[index].first;
    }

    void SpriteCache::evict()
    {
        std::pair<Render::SpriteGroup*, std::list<size_t>::iterator> toEvict = mCache[mUsedList.back()];
        toEvict.first->destroy();
        delete toEvict.first;

        mCache.erase(mUsedList.back());
        mUsedList.pop_back();
        mCurrentSize--;
    }

    void SpriteCache::clear()
    {
        while(mCurrentSize)
            evict();
    }
}
