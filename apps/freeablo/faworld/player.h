
#pragma once

#include "actor.h"
#include "inventory.h"
#include <boost/format.hpp>

#include <boost/signals2/signal.hpp>

namespace FAWorld
{
    class PlayerBehaviour;

    class Player : public Actor
    {
    public:
        static const std::string typeId;
        const std::string& getTypeId() override { return typeId; }

        Player();
        Player(const std::string& className, const DiabloExe::CharacterStats& charStats);
        void initCommon();
        Player(FASaveGame::GameLoader& loader, const DiabloExe::DiabloExe& exe);
        void save(FASaveGame::GameSaver& saver);

        virtual ~Player();
        void setSpriteClass(std::string className);
        void updateSprites();
        bool dropItem(const FAWorld::Tile& clickedTile);

        virtual void update(bool noclip) override;

        PlayerBehaviour* getPlayerBehaviour() { return (PlayerBehaviour*)mBehaviour; }
        int getTotalGold() const;

    private:
        void init(const std::string& className, const DiabloExe::CharacterStats& charStats);
        bool canTalkTo(Actor* actor);

        std::string mClassName;
    };
}
