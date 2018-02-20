#include "gamelevel.h"
#include "../fasavegame/gameloader.h"
#include "actor.h"
#include "actorstats.h"
#include "itemmap.h"
#include "world.h"
#include <diabloexe/diabloexe.h>
#include <misc/assert.h>

namespace FAWorld
{
    GameLevel::GameLevel(World& world, Level::Level&& level, size_t levelIndex)
        : mWorld(world), mLevel(std::move(level)), mLevelIndex(levelIndex), mItemMap(new ItemMap(this))
    {
    }

    GameLevel::GameLevel(World& world, FASaveGame::GameLoader& loader)
        : mWorld(world), mLevel(Level::Level(loader)), mLevelIndex(loader.load<int32_t>()), mItemMap(new ItemMap(loader, this))
    {
        release_assert(loader.currentlyLoadingLevel == nullptr);
        loader.currentlyLoadingLevel = this;

        uint32_t actorsSize = loader.load<uint32_t>();

        mActors.reserve(actorsSize);
        for (uint32_t i = 0; i < actorsSize; i++)
        {
            std::string actorTypeId = loader.load<std::string>();
            Actor* actor = static_cast<Actor*>(mWorld.mObjectIdMapper.construct(actorTypeId, loader));
            mActors.push_back(actor);
        }

        release_assert(loader.currentlyLoadingLevel == this);
        loader.currentlyLoadingLevel = nullptr;
    }

    void GameLevel::save(FASaveGame::GameSaver& saver)
    {
        Serial::ScopedCategorySaver cat("GameLevel", saver);

        mLevel.save(saver);
        saver.save(mLevelIndex);
        mItemMap->save(saver);

        uint32_t actorsSize = mActors.size();
        saver.save(actorsSize);

        for (Actor* actor : mActors)
        {
            saver.save(actor->getTypeId());
            actor->save(saver);
        }
    }

    GameLevel::~GameLevel()
    {
        for (size_t i = 0; i < mActors.size(); i++)
            delete mActors[i];
    }

    Level::MinPillar GameLevel::getTile(size_t x, size_t y) { return mLevel.get(x, y); }

    int32_t GameLevel::width() const { return mLevel.width(); }

    int32_t GameLevel::height() const { return mLevel.height(); }

    const std::pair<int32_t, int32_t> GameLevel::upStairsPos() const { return mLevel.upStairsPos(); }

    const std::pair<int32_t, int32_t> GameLevel::downStairsPos() const { return mLevel.downStairsPos(); }

    void GameLevel::activate(size_t x, size_t y) { mLevel.activate(x, y); }

    int32_t GameLevel::getNextLevel() { return mLevel.getNextLevel(); }

    int32_t GameLevel::getPreviousLevel() { return mLevel.getPreviousLevel(); }

    void GameLevel::update(bool noclip)
    {
        for (size_t i = 0; i < mActors.size(); i++)
        {
            Actor* actor = mActors[i];

            actorMapRemove(actor);
            actor->update(noclip);
            actorMapInsert(actor);
        }

        actorMapRefresh();

        for (auto& p : mItemMap->mItems)
            p.second.update();
    }

    void GameLevel::actorMapInsert(Actor* actor)
    {
        mActorMap2D[actor->getPos().current()] = actor;
        if (actor->getPos().isMoving())
            mActorMap2D[actor->getPos().next()] = actor;
    }

    void GameLevel::actorMapRemove(Actor* actor)
    {
        if (mActorMap2D[actor->getPos().current()] == actor)
            mActorMap2D.erase(actor->getPos().current());
        if (actor->getPos().isMoving() && mActorMap2D[actor->getPos().next()] == actor)
            mActorMap2D.erase(actor->getPos().next());
    }

    void GameLevel::actorMapClear() { mActorMap2D.clear(); }

    void GameLevel::actorMapRefresh()
    {
        actorMapClear();
        for (size_t i = 0; i < mActors.size(); i++)
            actorMapInsert(mActors[i]);
    }

    bool GameLevel::isPassable(int x, int y) const
    {
        if (x > 0 && x < width() && y > 0 && y < height() && !mLevel.get(x, y).passable())
            return false;

        FAWorld::Actor* actor = getActorAt(x, y);
        return actor == NULL || actor->isPassable();
    }

    Actor* GameLevel::getActorAt(int32_t x, int32_t y) const
    {
        auto it = mActorMap2D.find(std::pair<int32_t, int32_t>(x, y));
        if (it == mActorMap2D.end())
            return nullptr;

        return it->second;
    }

    void GameLevel::addActor(Actor* actor)
    {
        mActors.push_back(actor);
        actorMapInsert(actor);
    }

    static Cel::Colour friendHoverColor() { return {180, 110, 110, true}; }
    static Cel::Colour enemyHoverColor() { return {164, 46, 46, true}; }
    static Cel::Colour itemHoverColor() { return {185, 170, 119, true}; }

    void GameLevel::fillRenderState(FARender::RenderState* state, Actor* displayedActor)
    {
        state->mObjects.clear();
        state->mItems.clear();

        for (size_t i = 0; i < mActors.size(); i++)
        {
            auto tmp = mActors[i]->mAnimation.getCurrentRealFrame();

            FARender::FASpriteGroup* sprite = tmp.first;
            int32_t frame = tmp.second;
            boost::optional<Cel::Colour> hoverColor;
            if (mHoverState.isActorHovered(mActors[i]->getId()))
                hoverColor = mActors[i]->isEnemy(displayedActor) ? enemyHoverColor() : friendHoverColor();
            // offset the sprite for the current direction of the actor

            if (sprite)
            {
                frame += static_cast<int32_t>(mActors[i]->getPos().getDirection()) * sprite->getAnimLength();
                state->mObjects.push_back({sprite, static_cast<uint32_t>(frame), mActors[i]->getPos(), hoverColor});
            }

            for (auto& p : mItemMap->mItems)
            {
                auto sf = p.second.getSpriteFrame();
                FARender::ObjectToRender o;
                o.spriteGroup = sf.first;
                o.frame = sf.second;
                o.position = {p.first.x, p.first.y};
                if (mHoverState.isItemHovered(p.first))
                    o.hoverColor = itemHoverColor();
                state->mItems.push_back(o);
            }
        }
    }

    void GameLevel::removeActor(Actor* actor)
    {
        for (auto i = mActors.begin(); i != mActors.end(); ++i)
        {
            if (*i == actor)
            {
                mActors.erase(i);
                actorMapRemove(actor);
                return;
            }
        }
        release_assert(false && "tried to remove actor that isn't in level");
    }

    bool GameLevel::isPassableFor(int x, int y, const Actor* actor) const
    {
        auto actorAtPos = getActorAt(x, y);
        return mLevel.get(x, y).passable() && (actorAtPos == nullptr || actorAtPos == actor || actorAtPos->isPassable());
    }

    bool GameLevel::dropItem(std::unique_ptr<Item>&& item, const Actor& actor, const Tile& tile) { return mItemMap->dropItem(move(item), actor, tile); }

    Actor* GameLevel::getActorById(int32_t id)
    {
        for (auto actor : mActors)
        {
            if (actor->getId() == id)
                return actor;
        }

        return NULL;
    }

    void GameLevel::getActors(std::vector<Actor*>& actors) { actors.insert(actors.end(), mActors.begin(), mActors.end()); }

    HoverState& GameLevel::getHoverState() { return mHoverState; }

    GameLevel::GameLevel(World& world) : mWorld(world) {}

    ItemMap& GameLevel::getItemMap() { return *mItemMap; }

    bool GameLevel::isTown() const { return mLevelIndex == 0; }
}
