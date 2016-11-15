#ifndef MONSTER_H
#define MONSTER_H

#include <stdint.h>

#include <string>
#include <map>

namespace FAIO
{
    struct FAFile;
	class FAFileObject;
}

namespace DiabloExe
{
    struct Monster
    {
        public:
            uint32_t animSize;
            uint32_t seedSize;
            std::string cl2Path; // uint32_t ptr in exe
            uint32_t secondAttack;
            std::string soundPath; // uint32_t ptr in exe
            uint32_t specialSound;
            
            uint32_t usesTrn;
            std::string trnPath; // uint32_t p
            
            uint32_t idleFrameSet;
            uint32_t walkFrameSet;
            uint32_t attackFrameSet;
            uint32_t recoveryFrameSet;
            uint32_t deathFrameSet;
            uint32_t secondAttackFrameSet;
            
            uint32_t idlePlayback;
            uint32_t walkPlayback;
            uint32_t attackPlayback;
            uint32_t recoveryPlayback;
            uint32_t deathPlayback;
            uint32_t secondAttackPlayback;

            std::string monsterName; // uint32_t ptr in exe

            uint8_t minDunLevel;
            uint8_t maxDunLevel;
            
            uint16_t level;

            uint32_t minHp;
            uint32_t maxHp;

            uint8_t attackType;
            
            uint8_t unknown1;
            uint8_t unknown2;
            uint8_t unknown3;
            uint8_t unknown4;
            
            uint8_t intelligence;
            
            uint8_t unknown5;
            uint8_t unknown6;

            uint8_t subType;

            uint8_t toHit;
            uint8_t hitFrame;
            uint8_t minDamage;
            uint8_t maxDamage;
            
            uint8_t toHitSecond;
            uint8_t hitFrameSecond;
            uint8_t minDamageSecond;
            uint8_t maxDamageSecond;

            uint8_t armourClass;

            uint16_t type;

            uint16_t normalResistanceImmunitiesFlags;
            uint16_t hellResistanceImmunitiesFlags;
            
            uint16_t drops;

            uint16_t selectionOutline;
            
            uint32_t exp;

            std::string dump() const;

            Monster() {} 

        private:
            Monster(FAIO::FAFileObject& exe, size_t codeOffset);

            friend class DiabloExe;
    };
}

#endif
