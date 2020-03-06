#pragma once
#include "diabloexe/diabloexe.h"
#include "engine/enginemain.h"
#include "missile/missileenums.h"
#include "spellenums.h"

namespace FAWorld
{
    class SpellData
    {
    public:
        SpellData(SpellId id) : mId(id), mSpellData(Engine::EngineMain::get()->exe().getSpellsDataTable().at((uint8_t)id)) {}

        SpellId id() const { return mId; }

        bool canCastInTown() const { return mSpellData.mTownSpell; }

        int32_t manaCost() const { return mSpellData.mManaCost; }

        const std::string& soundEffect() const { return mSpellData.mSoundEffect; }

        DiabloExe::SpellData::SpellType getType() const { return mSpellData.mType; }

        std::vector<MissileId> missiles() const
        {
            std::vector<MissileId> ret;
            for (auto missileId : mSpellData.mMissiles)
            {
                if (missileId != 0)
                    ret.push_back((MissileId)missileId);
            }
            return ret;
        }

        int getFrameIndex() const
        {
            static const int spellFrameLUT[] = {
                1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 28, 13, 12, 18, 16, 14, 18, 19, 11, 20, 15, 21, 23, 24, 25, 22, 26, 29, 37, 38, 39, 42, 41, 40, 10, 36, 30,
            };
            return spellFrameLUT[int(mId)] - 1;
        }

        const std::string& name() const { return mSpellData.mNameText; }

        constexpr static const SpellId spellbookLUT[4][7] = {
            {SpellId::null, SpellId::firebolt, SpellId::cbolt, SpellId::hbolt, SpellId::heal, SpellId::healother, SpellId::flame},
            {SpellId::resurrect, SpellId::firewall, SpellId::telekinesis, SpellId::lightning, SpellId::town, SpellId::flash, SpellId::stone},
            {SpellId::rndteleport, SpellId::manashield, SpellId::element, SpellId::fireball, SpellId::wave, SpellId::chain, SpellId::guardian},
            {SpellId::nova, SpellId::golem, SpellId::teleport, SpellId::apoca, SpellId::bonespirit, SpellId::flare, SpellId::etherealize}};

        enum class SpellHighlightFrame
        {
            blank = 26,
            highlight = 42,
            scroll = 43,
            staff = 44,
            skill = 45,
            spell = 46,
            hotkeyF5 = 47,
            hotkeyF6 = 48,
            hotkeyF7 = 49,
            hotkeyF8 = 50
        };

        // Temporary quirk to only allow implemented spells to be used.
        static bool isSpellImplemented(SpellId spell)
        {
            static const SpellId implementedSpells[] = {SpellId::firebolt, SpellId::firewall, SpellId::manashield, SpellId::town};
            for (auto sp : implementedSpells)
                if (spell == sp)
                    return true;
            return false;
        }

    private:
        SpellId mId;
        const DiabloExe::SpellData& mSpellData;
    };
}
