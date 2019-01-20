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

    void GameLevel::insertActor(Actor* actor)
    {
        bool found = false;
        for (const auto actorInLevel : mActors)
        {
            if (actor == actorInLevel)
                found = true;
        }

        if (!found)
            mActors.push_back(actor);

        actorMapInsert(actor);
    }

    void GameLevel::actorMapInsert(Actor* actor)
    {
        // TODO: maybe have some separate layer for dead actors if we need one?
        if (actor->isDead())
            return;

        Actor* blocking = nullptr;
        if (mActorMap2D.count(actor->getPos().current()))
            blocking = mActorMap2D[actor->getPos().current()];
        debug_assert(!blocking);

        mActorMap2D[actor->getPos().current()] = actor;

        if (actor->getPos().isMoving())
        {
            if (mActorMap2D.count(actor->getPos().next()))
                blocking = mActorMap2D[actor->getPos().next()];
            debug_assert(!blocking);

            mActorMap2D[actor->getPos().next()] = actor;
        }
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

    Misc::Point GameLevel::getFreeSpotNear(Misc::Point point, int32_t radius) const
    {
        // partially based on https://stackoverflow.com/a/398302

        int32_t xOffset = 0;
        int32_t yOffset = 0;

        int32_t dx = 0;
        int32_t dy = -1;

        while (xOffset <= radius && yOffset <= radius)
        {
            Misc::Point targetPoint = point + Misc::Point{xOffset, yOffset};
            if (targetPoint.x >= 0 && targetPoint.x < width() && targetPoint.y >= 0 && targetPoint.y < height())
            {
                if (isPassable(targetPoint, nullptr))
                    return targetPoint;
            }

            if (xOffset == yOffset || (xOffset < 0 && xOffset == -yOffset) || (xOffset > 0 && xOffset == 1 - yOffset))
            {
                int32_t tmp = dx;
                dx = -dy;
                dy = tmp;
            }

            xOffset = xOffset + dx;
            yOffset = yOffset + dy;
        }

        return Misc::Point::invalid();
    }

    bool GameLevel::isPassable(const Misc::Point& point, const FAWorld::Actor* forActor) const
    {
        // Special hack because griswold spawns on an unpassable tile.
        // TODO: This should be fixed later by a patch on the town level data.
        if (forActor && forActor->mIsTowner)
            return true;

        if (point.x < 0 || point.x >= width() || point.y < 0 || point.y >= height() || !mLevel.get(point).passable())
            return false;

        FAWorld::Actor* actor = getActorAt(point);
        return actor == nullptr || actor == forActor || actor->isPassable();
    }

    Actor* GameLevel::getActorAt(const Misc::Point& point) const
    {
        auto it = mActorMap2D.find(point);
        if (it == mActorMap2D.end())
            return nullptr;

        return it->second;
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

    bool GameLevel::dropItem(std::unique_ptr<Item>&& item, const Actor& actor, const Tile& tile) { return mItemMap->dropItem(move(item), actor, tile); }

    bool GameLevel::dropItemClosestEmptyTile(Item& item, const Actor& actor, const Misc::Point& position, Misc::Direction direction)
    {
        auto tryDrop = [&](const Misc::Point& pos) {
            bool res = false;
            if (isPassable(pos, &actor) && !mItemMap->getItemAt(pos))
                res = dropItem(std::unique_ptr<Item>{new Item(item)}, actor, FAWorld::Tile(pos));
            return res;
        };

        if (direction == Misc::Direction::none)
        {
            if (tryDrop(position))
                return true;
            direction = Misc::Direction::south;
        }

        constexpr auto directionCnt = 8;
        for (auto diff : {0, -1, 1})
        {
            auto dir = static_cast<Misc::Direction>((static_cast<int32_t>(direction) + diff + directionCnt) % directionCnt);
            auto pos = Misc::getNextPosByDir(position, dir);
            if (tryDrop(pos))
                return true;
        }

        return tryDrop(position);
    }

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
