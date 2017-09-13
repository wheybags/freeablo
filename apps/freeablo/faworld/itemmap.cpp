#include "ItemMap.h"

#include "item.h"

#include "gamelevel.h"
#include "world.h"
#include "../engine/threadmanager.h"
#include "../farender/animationplayer.h"

FAWorld::ItemMap::ItemData::ItemData(std::unique_ptr <Item> itemArg):
  item(std::move(itemArg)),
  mAnimation (std::make_unique<FARender::AnimationPlayer> ())
{
    mAnimation->playAnimation(item->getFlipSpriteGroup(), FAWorld::World::getTicksInPeriod(0.5f), FARender::AnimationPlayer::AnimationType::FreezeAtEnd);
}

void FAWorld::ItemMap::ItemData::update()
{
    mAnimation->update ();
}

std::pair<FARender::FASpriteGroup*, int32_t> FAWorld::ItemMap::ItemData::getSpriteFrame()
{
    return mAnimation->getCurrentFrame();
}

FAWorld::ItemMap::ItemMap(const GameLevel* level) : mWidth(level->width()), mHeight(level->height()), mLevel(level)
{
}

FAWorld::ItemMap::~ItemMap()
{
}

bool FAWorld::ItemMap::dropItem(std::unique_ptr<FAWorld::Item>&& item, const FAWorld::Actor& actor, int i, int j)
{
    if(!mLevel->isPassableFor(i, j, &actor))
        return false;

    auto it = mItems.find({i, j});
    if(it != mItems.end())
        return false;

    Engine::ThreadManager::get()->playSound(item->getFlipSoundPath());
    mItems.insert(std::make_pair(std::make_pair(i, j), std::move(item)));
    return true;
}
