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

    Level::MinPillar GameLevel::getTile(const Misc::Point& point) const { return mLevel.get(point); }

    int32_t GameLevel::width() const { return mLevel.width(); }

    int32_t GameLevel::height() const { return mLevel.height(); }

    const Misc::Point GameLevel::upStairsPos() const { return mLevel.upStairsPos(); }

    const Misc::Point GameLevel::downStairsPos() const { return mLevel.downStairsPos(); }

    void GameLevel::activate(const Misc::Point& point) { mLevel.activate(point); }

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

    bool GameLevel::isPassable(const Misc::Point& point) const
    {
        if (point.x > 0 && point.x < width() && point.y > 0 && point.y < height() && !mLevel.get(point).passable())
            return false;

        FAWorld::Actor* actor = getActorAt(point);
        return actor == nullptr || actor->isPassable();
    }

    Actor* GameLevel::getActorAt(const Misc::Point& point) const
    {
        auto it = mActorMap2D.find(point);
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

    void GameLevel::fillRenderState(FARender::RenderState* state, Actor* displayedActor, const HoverStatus& hoverStatus)
    {
        state->mObjects.clear();
        state->mItems.clear();

        for (size_t i = 0; i < mActors.size(); i++)
        {
            auto tmp = mActors[i]->mAnimation.getCurrentRealFrame();

            FARender::FASpriteGroup* sprite = tmp.first;
            int32_t frame = tmp.second;
            boost::optional<Cel::Colour> hoverColor;
            if (mActors[i]->getId() == hoverStatus.hoveredActorId)
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
                o.position = Position(p.first.position);
                if (p.first == hoverStatus.hoveredItemTile)
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

    bool GameLevel::isPassableFor(const Misc::Point& point, const Actor* actor) const
    {
        auto actorAtPos = getActorAt(point);
        return mLevel.get(point).passable() && (actorAtPos == nullptr || actorAtPos == actor || actorAtPos->isPassable());
    }

    bool GameLevel::dropItem(std::unique_ptr<Item>&& item, const Actor& actor, const Tile& tile) { return mItemMap->dropItem(move(item), actor, tile); }

    Actor* GameLevel::getActorById(int32_t id)
    {
        for (auto actor : mActors)
        {
            if (actor->getId() == id)
                return actor;
        }

        return nullptr;
    }

    void GameLevel::getActors(std::vector<Actor*>& actors) { actors.insert(actors.end(), mActors.begin(), mActors.end()); }

    GameLevel::GameLevel(World& world) : mWorld(world) {}

    ItemMap& GameLevel::getItemMap() { return *mItemMap; }

    bool GameLevel::isTown() const { return mLevelIndex == 0; }
}
