
#pragma once

#include "actor.h"

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

    class Player : public Actor
    {
    public:
        static const std::string typeId;
        const std::string& getTypeId() override { return typeId; }

        Player(World& world);
        Player(World& world, const std::string& className, const DiabloExe::CharacterStats& charStats);
        void initCommon();
        Player(World& world, FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver) override;
        virtual bool checkHit(Actor* enemy) override;

        virtual ~Player();
        void setSpriteClass(std::string className);
        void updateSprites() override;
        bool dropItem(const FAWorld::Tile& clickedTile);

        virtual void update(bool noclip) override;

        PlayerBehaviour* getPlayerBehaviour() { return (PlayerBehaviour*)mBehaviour.get(); }

        boost::signals2::signal<void(const std::pair<int32_t, int32_t>&)> positionReached;
        int getDexterity() const { /*placeholder */ return 0; }
        int getArmorPenetration() const { /* placeholder */ return 0; }
        int getCharacterLevel() const { /* placeholder */ return 1; }
        PlayerClass getClass() const { /* placeholder */ return PlayerClass::warrior; }
        double meleeDamageVs(const Actor* actor) const override;
        int getMaxDamage() const { /* placeholder */ return 20; }
        int getPercentDamageBonus() const { return 0; }
        int getCharacterBaseDamage() const { /* placeholder */ return 0; }
        int getDamageBonus() const { /* placeholder */ return 0; }
        ItemBonus getItemBonus() const;

    private:
        void init(const std::string& className, const DiabloExe::CharacterStats& charStats);
        bool canTalkTo(Actor* actor);

    private:
        std::string mClassName = "warrior";
    };
}
