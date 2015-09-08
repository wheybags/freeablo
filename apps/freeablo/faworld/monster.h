#ifndef W_MONSTER_H
#define W_MONSTER_H

#include "actor.h"
#include "monsterstats.h"
namespace DiabloExe
{
    struct Monster;
}

namespace FAWorld
{


    class Monster: public Actor
    {
        public:
            typedef enum
            {
                undead,
                demon,
                animal

            }monsterType;
            Monster(const DiabloExe::Monster&
                    monster, Position pos,
                    MonsterStats * stats=nullptr);
            std::string getDieWav();
            std::string getHitWav();
            FARender::FASpriteGroup getCurrentAnim();
            MonsterStats * mStats;            
            int32_t getCurrentHP();
            void takeDamage(double amount);
            std::string getName();
            monsterType getType()
            {
                return mType;
            }



        private:
            std::string mAnimPath;
            std::string mSoundPath;
            std::string mName;
            monsterType mType;

    };
}

#endif     
