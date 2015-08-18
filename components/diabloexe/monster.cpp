#include "monster.h"

#include <faio/faio.h>

#include <iostream>
#include <sstream>

namespace DiabloExe
{
    Monster::Monster(FAIO::FAFile* exe, size_t codeOffset)
    {
        animSize = FAIO::read32(exe);
        seedSize = FAIO::read32(exe);
        uint32_t cl2Tmp = FAIO::read32(exe);
        secondAttack = FAIO::read32(exe);
        uint32_t soundTmp = FAIO::read32(exe);
        specialSound = FAIO::read32(exe);

        usesTrn = FAIO::read32(exe);
        uint32_t trnTmp = FAIO::read32(exe);

        idleFrameSet = FAIO::read32(exe);
        walkFrameSet = FAIO::read32(exe);
        attackFrameSet = FAIO::read32(exe);
        recoveryFrameSet = FAIO::read32(exe);
        deathFrameSet = FAIO::read32(exe);
        secondAttackFrameSet = FAIO::read32(exe);

        idlePlayback = FAIO::read32(exe);
        walkPlayback = FAIO::read32(exe);
        attackPlayback = FAIO::read32(exe);
        recoveryPlayback = FAIO::read32(exe);
        deathPlayback = FAIO::read32(exe);
        secondAttackPlayback = FAIO::read32(exe);
        
        uint32_t nameTmp = FAIO::read32(exe);

        minDunLevel = FAIO::read8(exe);
        maxDunLevel = FAIO::read8(exe);

        level = FAIO::read16(exe);

        minHp = FAIO::read32(exe);
        maxHp = FAIO::read32(exe);

        attackType = FAIO::read8(exe);

        unknown1 = FAIO::read8(exe);
        unknown2 = FAIO::read8(exe);
        unknown3 = FAIO::read8(exe);
        unknown4 = FAIO::read8(exe);

        intelligence = FAIO::read8(exe);

        unknown5 = FAIO::read8(exe);
        unknown6 = FAIO::read8(exe);

        subType = FAIO::read8(exe);

        toHit = FAIO::read8(exe);
        hitFrame = FAIO::read8(exe);
        minDamage = FAIO::read8(exe);
        maxDamage = FAIO::read8(exe);

        toHitSecond = FAIO::read8(exe);
        hitFrameSecond = FAIO::read8(exe);
        minDamageSecond = FAIO::read8(exe);
        maxDamageSecond = FAIO::read8(exe);

        armourClass = FAIO::read8(exe);

        type = FAIO::read16(exe);

        normalResistanceImmunitiesFlags = FAIO::read16(exe);
        hellResistanceImmunitiesFlags = FAIO::read16(exe);

        drops = FAIO::read16(exe);

        selectionOutline = FAIO::read16(exe);

        exp = FAIO::read32(exe);

        cl2Path = FAIO::readCStringFromWin32Binary(exe, cl2Tmp, codeOffset);
        soundPath = FAIO::readCStringFromWin32Binary(exe, soundTmp, codeOffset);
        trnPath = FAIO::readCStringFromWin32Binary(exe, trnTmp, codeOffset);
        monsterName = FAIO::readCStringFromWin32Binary(exe, nameTmp, codeOffset);
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
