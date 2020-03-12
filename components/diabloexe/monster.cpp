#include "monster.h"
#include <faio/fafileobject.h>
#include <iostream>
#include <misc/assert.h>
#include <sstream>

namespace DiabloExe
{
    Monster::Monster(FAIO::FAFileObject& exe, size_t codeOffset)
    {
        animSize = exe.read32();
        seedSize = exe.read32();
        uint32_t cl2Tmp = exe.read32();
        secondAttack = exe.read32();
        uint32_t soundTmp = exe.read32();
        specialSound = exe.read32();

        usesTrn = exe.read32();
        uint32_t trnTmp = exe.read32();

        idleFrameSet = exe.read32();
        walkFrameSet = exe.read32();
        attackFrameSet = exe.read32();
        recoveryFrameSet = exe.read32();
        deathFrameSet = exe.read32();
        secondAttackFrameSet = exe.read32();

        idlePlayback = exe.read32();
        walkPlayback = exe.read32();
        attackPlayback = exe.read32();
        recoveryPlayback = exe.read32();
        deathPlayback = exe.read32();
        secondAttackPlayback = exe.read32();

        uint32_t nameTmp = exe.read32();

        minDunLevel = exe.read8();
        maxDunLevel = exe.read8();

        level = exe.read16();

        minHp = exe.read32();
        maxHp = exe.read32();

        attackType = MonsterAttackType(exe.read8());

        unknown1 = exe.read8();
        unknown2 = exe.read8();
        unknown3 = exe.read8();
        unknown4 = exe.read8();

        intelligence = exe.read8();

        unknown5 = exe.read8();
        unknown6 = exe.read8();

        subType = exe.read8();

        toHit = exe.read8();
        hitFrame = exe.read8();
        minDamage = exe.read8();
        maxDamage = exe.read8();

        toHitSecond = exe.read8();
        hitFrameSecond = exe.read8();
        minDamageSecond = exe.read8();
        maxDamageSecond = exe.read8();

        armourClass = exe.read8();

        type = exe.read16();

        normalResistanceImmunitiesFlags = exe.read16();
        hellResistanceImmunitiesFlags = exe.read16();

        drops = exe.read16();

        selectionOutline = exe.read16();

        exp = exe.read32();

        cl2Path = exe.readCStringFromWin32Binary(cl2Tmp, codeOffset);
        soundPath = exe.readCStringFromWin32Binary(soundTmp, codeOffset);
        trnPath = exe.readCStringFromWin32Binary(trnTmp, codeOffset);
        monsterName = exe.readCStringFromWin32Binary(nameTmp, codeOffset);
    }

    std::string Monster::dump() const
    {
        std::stringstream ss;
        ss << "{" << std::endl
           << "\tanimSize: " << animSize << "," << std::endl
           << "\tseedSize: " << seedSize << "," << std::endl
           << "\tcl2Path : " << cl2Path << "," << std::endl
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

           << "\tmonsterName : " << monsterName << "," << std::endl

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

    FixedPoint getSpeedByMonsterAttackType(MonsterAttackType type)
    {
        // https://wheybags.gitlab.io/jarulfs-guide/#monster-timing-information
        switch (type)
        {
            case MonsterAttackType::Zombie:
                return FixedPoint(1) / FixedPoint("1.2");
            case MonsterAttackType::Overlord:
                return FixedPoint(1) / FixedPoint("0.5");
            case MonsterAttackType::Skeleton:
                return FixedPoint(1) / FixedPoint("0.4");
            case MonsterAttackType::SkeletonArcher:
                return FixedPoint(1) / FixedPoint("0.4");
            case MonsterAttackType::Scavenger:
                return FixedPoint(1) / FixedPoint("0.4");
            case MonsterAttackType::HornedDemon:
                return FixedPoint(1) / FixedPoint("0.4");
            case MonsterAttackType::GoatMan:
                return FixedPoint(1) / FixedPoint("0.4");
            case MonsterAttackType::GoatManArcher:
                return FixedPoint(1) / FixedPoint("0.4");
            case MonsterAttackType::FallenOne:
                return FixedPoint(1) / FixedPoint("0.55"); // TODO: This should be different for Fallen Ones with spears / swords
            case MonsterAttackType::MagmaDemon:
                return FixedPoint(1) / FixedPoint("0.5");
            case MonsterAttackType::SkeletonCaptain:
                return FixedPoint(1) / FixedPoint("0.4");
            case MonsterAttackType::WingedFiend:
                return FixedPoint(1) / FixedPoint("0.65");
            case MonsterAttackType::Gargoyle:
                return FixedPoint(1) / FixedPoint("0.7");
            case MonsterAttackType::Butcher:
                return FixedPoint(1) / FixedPoint("0.4");
            case MonsterAttackType::Succubus:
                return FixedPoint(1) / FixedPoint("0.4");
            case MonsterAttackType::Hidden:
                return FixedPoint(1) / FixedPoint("0.4");
            case MonsterAttackType::LightningDemon:
                return FixedPoint(1) / FixedPoint("0.4");
            case MonsterAttackType::Fireman:
                return FixedPoint(1) / FixedPoint("0.4"); // Missing from the guide
            case MonsterAttackType::GharbadTheWeak:
                return FixedPoint(1) / FixedPoint("0.4"); // Missing from the guide
            case MonsterAttackType::SpittingTerror:
                return FixedPoint(1) / FixedPoint("0.4");
            case MonsterAttackType::FastSpittingTerror:
                return FixedPoint(1) / FixedPoint("0.4"); // Missing from the guide
            case MonsterAttackType::Golem:
                return FixedPoint(1) / FixedPoint("0.8");
            case MonsterAttackType::ZharTheMad:
                return FixedPoint(1) / FixedPoint("0.4"); // Missing from the guide
            case MonsterAttackType::Snotspill:
                return FixedPoint(1) / FixedPoint("0.4"); // Missing from the guide
            case MonsterAttackType::Viper:
                return FixedPoint(1) / FixedPoint("0.55");
            case MonsterAttackType::Mage:
                return FixedPoint(1) / FixedPoint("0.05");
            case MonsterAttackType::Balrog:
                return FixedPoint(1) / FixedPoint("0.35");
            case MonsterAttackType::Diablo:
                return FixedPoint(1) / FixedPoint("0.3");
            case MonsterAttackType::ENUM_END:
                break;
        }

        invalid_enum(MonsterAttackType, type);
    }
}
