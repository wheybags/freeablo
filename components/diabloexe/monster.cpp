#include "monster.h"

#include <faio/faio.h>

#include <iostream>
#include <sstream>

namespace DiabloExe
{
    Monster::Monster(FAIO::FAFile* exe, size_t codeOffset)
    {
        animSize = load32(exe);
        seedSize = load32(exe);
        uint32_t cl2Tmp = load32(exe);
        secondAttack = load32(exe);
        uint32_t soundTmp = load32(exe);
        specialSound = load32(exe);

        usesTrn = load32(exe);
        uint32_t trnTmp = load32(exe);

        idleFrameSet = load32(exe);
        walkFrameSet = load32(exe);
        attackFrameSet = load32(exe);
        recoveryFrameSet = load32(exe);
        deathFrameSet = load32(exe);
        secondAttackFrameSet = load32(exe);

        idlePlayback = load32(exe);
        walkPlayback = load32(exe);
        attackPlayback = load32(exe);
        recoveryPlayback = load32(exe);
        deathPlayback = load32(exe);
        secondAttackPlayback = load32(exe);
        
        uint32_t nameTmp = load32(exe);

        minDunLevel = load8(exe);
        maxDunLevel = load8(exe);

        level = load16(exe);

        minHp = load32(exe);
        maxHp = load32(exe);

        attackType = load8(exe);

        unknown1 = load8(exe);
        unknown2 = load8(exe);
        unknown3 = load8(exe);
        unknown4 = load8(exe);

        intelligence = load8(exe);

        unknown5 = load8(exe);
        unknown6 = load8(exe);

        subType = load8(exe);

        toHit = load8(exe);
        hitFrame = load8(exe);
        minDamage = load8(exe);
        maxDamage = load8(exe);

        toHitSecond = load8(exe);
        hitFrameSecond = load8(exe);
        minDamageSecond = load8(exe);
        maxDamageSecond = load8(exe);

        armourClass = load8(exe);

        type = load16(exe);

        normalResistanceImmunitiesFlags = load16(exe);
        hellResistanceImmunitiesFlags = load16(exe);

        drops = load16(exe);

        selectionOutline = load16(exe);

        exp = load32(exe);

        cl2Path = loadString(exe, cl2Tmp, codeOffset);
        soundPath = loadString(exe, soundTmp, codeOffset);
        trnPath = loadString(exe, trnTmp, codeOffset);
        monsterName = loadString(exe, nameTmp, codeOffset);
    }

    uint32_t Monster::load32(FAIO::FAFile* exe)
    {
        uint32_t tmp;
        FAIO::FAfread(&tmp, 4, 1, exe);
        return tmp;
    }

    uint16_t Monster::load16(FAIO::FAFile* exe)
    {
        uint16_t tmp;
        FAIO::FAfread(&tmp, 2, 1, exe);
        return tmp;
    }

    uint8_t Monster::load8(FAIO::FAFile* exe)
    {
        uint8_t tmp;
        FAIO::FAfread(&tmp, 1, 1, exe);
        return tmp;
    }
    
    std::string Monster::loadString(FAIO::FAFile* exe, int32_t ptr, size_t codeOffset)
    {
        std::string retval = "";
        
        if(ptr)
        {
            FAIO::FAfseek(exe, ptr-codeOffset, SEEK_SET);
            char c;

            FAIO::FAfread(&c, 1, 1, exe);

            while(c != '\0')
            {
                retval += c;
                FAIO::FAfread(&c, 1, 1, exe);
            }
        }

        return retval;
    }

    std::string Monster::dump() const
    {
        std::stringstream ss;
        ss << "{" << std::endl
        << "\tanimSize: " << animSize << "," << std::endl
        << "\tseedSize: " << seedSize << "," << std::endl
        << "\tcl2Path : " << cl2Path  << "," << std::endl
        << "\tsecondAttack: " << secondAttack << "," << std::endl
        << "\tsoundPath: " << soundPath << "," << std::endl
        << "\tspecialSound: " << specialSound << "," << std::endl

        << "\tusesTrn: " << usesTrn << "," << std::endl
        << "\ttrnPath: " << trnPath << "," << std::endl

        << "\tidleFrameSet: " << idleFrameSet << "," << std::endl
        << "\twalkFrameSet: " << walkFrameSet << "," << std::endl
        << "\tattackFrameSet: " << attackFrameSet << "," << std::endl
        << "\trecoveryFrameSet: " << recoveryFrameSet << "," << std::endl
        << "\tdeathFrameSet: " << deathFrameSet << "," << std::endl
        << "\tsecondAttackFrameSet: " << secondAttackFrameSet << "," << std::endl

        << "\tidlePlayback: " << idlePlayback << "," << std::endl
        << "\twalkPlayback: " << walkPlayback << "," << std::endl
        << "\tattackPlayback: " << attackPlayback << "," << std::endl
        << "\trecoveryPlayback: " << recoveryPlayback << "," << std::endl
        << "\tdeathPlayback: " << deathPlayback << "," << std::endl
        << "\tsecondAttackPlayback: " << secondAttackPlayback << "," << std::endl

        << "\tmonsterName : " << monsterName  << "," << std::endl

        << "\tminDunLevel: " << (size_t)minDunLevel << "," << std::endl
        << "\tmaxDunLevel: " << (size_t)maxDunLevel << "," << std::endl

        << "\tlevel: " << level << "," << std::endl

        << "\tminHp: " << minHp << "," << std::endl
        << "\tmaxHp: " << maxHp << "," << std::endl

        << "\tattackType: " << (size_t)attackType << "," << std::endl

        << "\tunknown1: " << (size_t)unknown1 << "," << std::endl
        << "\tunknown2: " << (size_t)unknown2 << "," << std::endl
        << "\tunknown3: " << (size_t)unknown3 << "," << std::endl
        << "\tunknown4: " << (size_t)unknown4 << "," << std::endl

        << "\tintelligence: " << (size_t)intelligence << "," << std::endl

        << "\tunknown5: " << (size_t)unknown5 << "," << std::endl
        << "\tunknown6: " << (size_t)unknown6 << "," << std::endl

        << "\tsubType: " << (size_t)subType << "," << std::endl

        << "\ttoHit: " << (size_t)toHit << "," << std::endl
        << "\thitFrame: " << (size_t)hitFrame << "," << std::endl
        << "\tminDamage: " << (size_t)minDamage << "," << std::endl
        << "\tmaxDamage: " << (size_t)maxDamage << "," << std::endl

        << "\ttoHitSecond: " << (size_t)toHitSecond << "," << std::endl
        << "\thitFrameSecond: " << (size_t)hitFrameSecond << "," << std::endl
        << "\tminDamageSecond: " << (size_t)minDamageSecond << "," << std::endl
        << "\tmaxDamageSecond: " << (size_t)maxDamageSecond << "," << std::endl

        << "\tarmourClass: " << (size_t)armourClass << "," << std::endl

        << "\ttype: " << type << "," << std::endl

        << "\tnormalResistanceImmunitiesFlags: " << normalResistanceImmunitiesFlags << "," << std::endl
        << "\thellResistanceImmunitiesFlags: " << hellResistanceImmunitiesFlags << "," << std::endl

        << "\tdrops: " << drops << "," << std::endl

        << "\tselectionOutline: " << selectionOutline << "," << std::endl
        << "\texp: " << exp << std::endl
        << "}" << std::endl;

        return ss.str();
    }
}
