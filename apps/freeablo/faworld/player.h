
#pragma once

#include "actor.h"
#include "playerstats.h"

#include <boost/signals2/signal.hpp>

namespace FAWorld
{
    class PlayerBehaviour;

    enum class PlayerClass
    {
        warrior = 0,
        rogue,
        sorcerer,
    };

    // note that this function features misspelling of sorcerer as sorceror
    // because it's written this way on character panel
    const char* toString(PlayerClass value);

    class Player : public Actor
    {
    public:
        static const std::string typeId;
        const std::string& getTypeId() override { return typeId; }

        Player(World& world);
        Player(World& world, const DiabloExe::CharacterStats& charStats);
        void initCommon();
        Player(World& world, FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver) override;
        virtual bool checkHit(Actor* enemy) override;

        virtual ~Player();
        void updateSprites() override;
        bool dropItem(const FAWorld::Tile& clickedTile);

        virtual void update(bool noclip) override;

        PlayerBehaviour* getPlayerBehaviour() { return (PlayerBehaviour*)mBehaviour.get(); }
        const PlayerStats& getPlayerStats() const { return mPlayerStats; }
        void setPlayerClass(PlayerClass playerClass);

        boost::signals2::signal<void(const Misc::Point&)> positionReached;
        int getArmorPenetration() const { /* placeholder */ return 0; }
        int getCharacterLevel() const { /* placeholder */ return 1; }
        PlayerClass getClass() const { return mPlayerClass; }
        int32_t meleeDamageVs(const Actor* actor) const override;
        int getMaxDamage() const { /* placeholder */ return 20; }
        int getPercentDamageBonus() const { return 0; }
        int getCharacterBaseDamage() const { /* placeholder */ return 0; }
        int getDamageBonus() const { /* placeholder */ return 0; }
        ItemBonus getItemBonus() const;

    private:
        void init(const DiabloExe::CharacterStats& charStats);
        bool canTalkTo(Actor* actor);

    private:
        PlayerStats mPlayerStats;
        PlayerClass mPlayerClass = PlayerClass::warrior;
    };
}
