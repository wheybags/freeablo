#include "spritecache.h"
#include <cel/celfile.h>
#include <iostream>
#include <misc/assert.h>
#include <misc/stringops.h>
#include <numeric>

namespace FARender
{
    struct nk_image FASpriteGroup::getNkImage(int32_t frame)
    {
        release_assert(frame >= 0 && frame < (int32_t)frameHandles.size());
        auto ret = nk_subimage_handle(
            nk_handle_ptr(&frameHandles[frame]), this->getWidth(frame), this->getHeight(frame), nk_rect(0, 0, this->getWidth(frame), this->getHeight(frame)));
        return ret;
    }

    SpriteCache::SpriteCache(uint32_t size) : mMaxSize(size) {}

    SpriteCache::~SpriteCache()
    {
        for (auto block : mSpriteGroupStore)
            delete[] block;
    }

    uint32_t SpriteCache::newUniqueIndex() { return mNextCacheIndex++; }

    void SpriteCache::directInsert(Render::SpriteGroup* sprite, uint32_t cacheIndex)
    {
        mUsedList.push_front(cacheIndex);
        mCache[cacheIndex] = CacheEntry(sprite, mUsedList.begin(), true);

        mCurrentSize++;
    }

    Render::SpriteGroup* SpriteCache::get(uint32_t index)
    {
        if (!mCache.count(index))
            message_and_abort_fmt("ERROR INVALID SPRITE CACHE REQUEST %d", index);

        return mCache[index].sprite;
    }

    void SpriteCache::setImmortal(uint32_t index, bool immortal)
    {
        get(index);
        mCache[index].immortal = immortal;
    }

    void SpriteCache::clear()
    {
        for (std::list<uint32_t>::iterator it = mUsedList.begin(); it != mUsedList.end(); it++)
        {
            if (Render::SpriteGroup::canDeleteIndividualSprites())
                mCache[*it].sprite->destroy();
            delete mCache[*it].sprite;
        }

        mCache.clear();
        mUsedList.clear();
        mCurrentSize = 0;

        if (!Render::SpriteGroup::canDeleteIndividualSprites())
            Render::deleteAllSprites();
    }

    FASpriteGroup* SpriteCache::allocNewSpriteGroup()
    {
        if (mSpriteGroupCurrentBlockIndex == SPRITEGROUP_STORE_BLOCK_SIZE || mSpriteGroupStore.size() == 0)
        {
            mSpriteGroupStore.push_back(new FASpriteGroup[SPRITEGROUP_STORE_BLOCK_SIZE]);
            mSpriteGroupCurrentBlockIndex = 0;
        }

        FASpriteGroup* retval = &(mSpriteGroupStore[mSpriteGroupStore.size() - 1][mSpriteGroupCurrentBlockIndex]);
        mSpriteGroupCurrentBlockIndex++;

        return retval;
    }
}
