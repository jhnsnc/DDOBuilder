// DC.cpp
//
#include "StdAfx.h"
#include "DC.h"
#include "XmlLib\SaxWriter.h"
#include "Feat.h"
#include "GlobalSupportFunctions.h"

#define DL_ELEMENT DC

namespace
{
    const wchar_t f_saxElementName[] = L"DC";
    DL_DEFINE_NAMES(DC_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

DC::DC() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(DC_PROPERTIES)
}

DL_DEFINE_ACCESS(DC_PROPERTIES)

XmlLib::SaxContentElementInterface * DC::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(DC_PROPERTIES)

    return subHandler;
}

void DC::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(DC_PROPERTIES)
}

void DC::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(DC_PROPERTIES)
    writer->EndElement();
}

double DC::Amount(size_t tier) const
{
    double amount = 0;
    if (HasAmountVector())
    {
        ASSERT(tier <= m_AmountVector.size());
        amount = m_AmountVector.at(tier-1);     // 0 based vs 1 based
    }
    else
    {
        ASSERT(HasAmount());
        ASSERT(tier == 1);
        amount = Amount();
    }
    return amount;
}

bool DC::IncludesWeapon(WeaponType wt) const
{
    bool included = false;
    std::list<WeaponType>::const_iterator it = m_Weapon.begin();
    while (!included && it != m_Weapon.end())
    {
        if ((*it) == Weapon_All
                || (*it) == wt)
        {
            included = true;
        }
        ++it;
    }
    return included;
}

bool DC::IncludesSpellPower(SpellPowerType sp) const
{
    bool included = false;
    std::list<SpellPowerType>::const_iterator it = m_SpellPower.begin();
    while (!included && it != m_SpellPower.end())
    {
        if ((*it) == SpellPower_All
                || (*it) == sp)
        {
            included = true;
        }
        ++it;
    }
    return included;
}

bool DC::IncludesEnergy(EnergyType energy) const
{
    bool included = false;
    std::list<EnergyType>::const_iterator it = m_Energy.begin();
    while (!included && it != m_Energy.end())
    {
        if ((*it) == Energy_All
                || (*it) == energy)
        {
            included = true;
        }
        ++it;
    }
    return included;
}

bool DC::IncludesSkill(SkillType skill) const
{
    bool included = false;
    std::list<SkillType>::const_iterator it = m_Skill.begin();
    while (!included && it != m_Skill.end())
    {
        if ((*it) == Skill_All
                || (*it) == skill)
        {
            included = true;
        }
        ++it;
    }
    return included;
}

bool DC::VerifyObject(std::stringstream * ss) const
{
    bool ok = true;
    switch (m_Type)
    {
        case DC_Unknown:
            (*ss) << "Has unknown effect type\n";
            ok = false;
            break;
        case DC_AbilityBonus:
            if (!HasAbility())
            {
                (*ss) << "Ability effect missing ability field\n";
                ok = false;
            }
            else if (Ability() == Ability_Unknown)
            {
                (*ss) << "Ability effect has bad enum value\n";
                ok = false;
            }
            break;
        case DC_EnergyAbsorbance:
        case DC_EnergyResistance:
            if (m_Energy.size() == 0)
            {
                (*ss) << "Energy resistance/absorbance effect missing Energy field\n";
                ok = false;
            }
            else
            {
                std::list<EnergyType>::const_iterator it = m_Energy.begin();
                while (it != m_Energy.end())
                {
                    if ((*it) == Energy_Unknown)
                    {
                        (*ss) << "Energy resistance/absorbance effect has bad enum value\n";
                        ok = false;
                        break;
                    }
                    ++it;
                }
            }
            break;
        case DC_SaveBonus:
            if (!HasSave())
            {
                (*ss) << "SaveBonus effect missing save field\n";
                ok = false;
            }
            else if (Save() == Save_Unknown)
            {
                (*ss) << "Save effect has bad enum value\n";
                ok = false;
            }
            break;
        case DC_SkillBonus:
            if (m_Skill.size() == 0
                    && !HasAbility())
            {
                (*ss) << "Skill effect missing skill field\n";
                ok = false;
            }
            else
            {
                std::list<SkillType>::const_iterator it = m_Skill.begin();
                while (it != m_Skill.end())
                {
                    if ((*it) == Skill_Unknown)
                    {
                        (*ss) << "SkillBonus effect has bad enum value\n";
                        ok = false;
                        break;
                    }
                    ++it;
                }
                if ((HasAbility() && Ability() == Ability_Unknown))
                {
                        (*ss) << "SkillBonus effect has bad Ability enum value\n";
                        ok = false;
                }
            }
            break;
        case DC_TacticalDC:
            if (!HasTactical())
            {
                (*ss) << "TacticalDC effect missing tactical field\n";
                ok = false;
            }
            else if (Tactical() == Tactical_Unknown)
            {
                (*ss) << "Tactical effect has bad enum value\n";
                ok = false;
            }
            break;
        case DC_SpellDC:
            if (!HasSchool())
            {
                (*ss) << "SpellDC effect missing school field\n";
                ok = false;
            }
            else if (School() == SpellSchool_Unknown)
            {
                (*ss) << "School effect has bad enum value\n";
                ok = false;
            }
            break;
        case DC_SpellPower:
            if (m_SpellPower.size() == 0)
            {
                (*ss) << "SpellPower effect missing school field\n";
                ok = false;
            }
            else
            {
                std::list<SpellPowerType>::const_iterator it = m_SpellPower.begin();
                while (it != m_SpellPower.end())
                {
                    if ((*it) == SpellPower_Unknown)
                    {
                        (*ss) << "SpellPower effect has bad enum value\n";
                        ok = false;
                        break;
                    }
                    ++it;
                }
            }
            break;
        case DC_DRBypass:
            // must have a single DR value
            if (m_DR.size() == 0)
            {
                (*ss) << "DRBypass effect missing DR field\n";
                ok = false;
            }
            else if (m_DR.size() > 1)
            {
                (*ss) << "DRBypass effect can only handle a single value\n";
                ok = false;
            }
            // fall through to check weapon/weaponclass settings
        case DC_AttackBonus:
        case DC_Alacrity:
        case DC_CenteredWeapon:
        case DC_CriticalAttackBonus:
        case DC_CriticalMultiplier:
        case DC_CriticalRange:
        case DC_DamageBonus:
        case DC_Seeker:
        case DC_VorpalRange:
        case DC_WeaponBaseDamageBonus:
        case DC_WeaponDamageBonus:
        case DC_WeaponEnchantment:
        case DC_WeaponProficiency:
        case DC_WeaponOtherDamageBonus:
        case DC_WeaponOtherCriticalDamageBonus:
            {
                if (m_Weapon.size() == 0
                        && !HasWeaponClass()
                        && !HasDamageType())
                {
                    (*ss) << "Weapon effect missing Weapon/Class/DamageType field\n";
                    ok = false;
                }
                std::list<WeaponType>::const_iterator it = m_Weapon.begin();
                while (it != m_Weapon.end())
                {
                    if ((*it) == Weapon_Unknown)
                    {
                        (*ss) << "Weapon effect has bad enum value\n";
                        ok = false;
                        break;
                    }
                    ++it;
                }
                if (HasWeaponClass() && WeaponClass() == WeaponClass_Unknown)
                {
                    (*ss) << "WeaponClass effect has bad enum value\n";
                    ok = false;
                }
                if (HasDamageType() && DamageType() == WeaponDamage_Unknown)
                {
                    (*ss) << "DamageType effect has bad enum value\n";
                    ok = false;
                }
            }
            break;
        case DC_SpellLikeAbility:
            if (!HasSpellLikeAbility())
            {
                (*ss) << "SpellLikeAbility effect missing SpellLikeAbility field\n";
                ok = false;
            }
            else if (FindSpellByName(SpellLikeAbility()).Name() == "")
            {
                (*ss) << "SpellLikeAbility field missing from Spells.xml\n";
                ok = false;
            }
            break;
        case DC_Immunity:
            if (Immunity().size() == 0)
            {
                (*ss) << "Immunity effect missing Immunity field\n";
                ok = false;
            }
            break;
    }
    if (HasAmountVector())
    {
        // realize the vector to catch size/data mismatches
        std::vector<double> d = m_AmountVector;
    }
    if (HasFeat())
    {
        // verify the feat exists
        ::Feat feat = FindFeat(Feat());
        if (feat.Name() != Feat())
        {
            (*ss) << "Specified feat of \"" << Feat() << "\" not found \n";
            ok = false;
        }
    }
    return ok;
}

bool DC::operator==(const DC & other) const
{
    return (m_hasDisplayName == other.m_hasDisplayName)
            && (m_DisplayName == other.m_DisplayName)
            && (m_Type == other.m_Type)
            && (m_Bonus == other.m_Bonus)
            && (m_hasAmount == other.m_hasAmount)
            && (m_Amount == other.m_Amount)
            && (m_hasAmountVector == other.m_hasAmountVector)
            && (m_AmountVector == other.m_AmountVector)
            && (m_hasAmountPerLevel == other.m_hasAmountPerLevel)
            && (m_AmountPerLevel == other.m_AmountPerLevel)
            && (m_hasAmountPerAP == other.m_hasAmountPerAP)
            && (m_AmountPerAP == other.m_AmountPerAP)
            && (m_hasPercent == other.m_hasPercent)
            && (m_hasDiceRoll == other.m_hasDiceRoll)
            && (m_DiceRoll == other.m_DiceRoll)
            && (m_hasDivider == other.m_hasDivider)
            && (m_Divider == other.m_Divider)
            && (m_hasFeat == other.m_hasFeat)
            && (m_Feat == other.m_Feat)
            && (m_hasNoFailOn1 == other.m_hasNoFailOn1)
            && (m_NoFailOn1 == other.m_NoFailOn1)
            && (m_hasSpell == other.m_hasSpell)
            && (m_Spell == other.m_Spell)
            && (m_hasSpellLevel == other.m_hasSpellLevel)
            && (m_SpellLevel == other.m_SpellLevel)
            && (m_Stance == other.m_Stance)
            && (m_hasEnhancementTree == other.m_hasEnhancementTree)
            && (m_EnhancementTree == other.m_EnhancementTree)
            && (m_hasSpellLikeAbility == other.m_hasSpellLikeAbility)
            && (m_SpellLikeAbility == other.m_SpellLikeAbility)
            && (m_Immunity == other.m_Immunity)
            && (m_hasAbility == other.m_hasAbility)
            && (m_Ability == other.m_Ability)
            && (m_hasClass == other.m_hasClass)
            && (m_Class == other.m_Class)
            && (m_DR == other.m_DR)
            && (m_Energy == other.m_Energy)
            && (m_hasFavoredEnemy == other.m_hasFavoredEnemy)
            && (m_FavoredEnemy == other.m_FavoredEnemy)
            && (m_hasSave == other.m_hasSave)
            && (m_Save == other.m_Save)
            && (m_Skill == other.m_Skill)
            && (m_SpellPower == other.m_SpellPower)
            && (m_hasSchool == other.m_hasSchool)
            && (m_School == other.m_School)
            && (m_hasTactical == other.m_hasTactical)
            && (m_Tactical == other.m_Tactical)
            && (m_hasWeaponClass == other.m_hasWeaponClass)
            && (m_WeaponClass == other.m_WeaponClass)
            && (m_hasDamageType == other.m_hasDamageType)
            && (m_DamageType == other.m_DamageType)
            && (m_Weapon == other.m_Weapon);
}