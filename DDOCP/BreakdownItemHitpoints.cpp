// BreakdownItemHitpoints.cpp
//
#include "stdafx.h"
#include "BreakdownItemHitpoints.h"

#include "GlobalSupportFunctions.h"

BreakdownItemHitpoints::BreakdownItemHitpoints(
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem,
        BreakdownItem * pCon) :
    BreakdownItem(Breakdown_Hitpoints, treeList, hItem),
    m_pConstitutionBreakdown(pCon)
{
    // need to know when total constitution changes
    m_pConstitutionBreakdown->AttachObserver(this);
}

BreakdownItemHitpoints::~BreakdownItemHitpoints()
{
}

// required overrides
CString BreakdownItemHitpoints::Title() const
{
    CString text;
    text = "Hitpoints";
    return text;
}

CString BreakdownItemHitpoints::Value() const
{
    CString value;

    value.Format(
            "%4d",
            (int)Total());
    return value;
}

void BreakdownItemHitpoints::CreateOtherEffects()
{
    // add class hit points
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();
        std::vector<size_t> classLevels = m_pCharacter->ClassLevels(MAX_LEVEL);
        for (size_t ci = Class_Unknown; ci < Class_Count; ++ci)
        {
            if (classLevels[ci] > 0)
            {
                std::string className = EnumEntryText((ClassType)ci, classTypeMap);
                className += " Levels";
                ActiveEffect classBonus(
                        Bonus_class,
                        className,
                        classLevels[ci],
                        ClassHitpoints((ClassType)ci),
                        "");        // no tree
                AddOtherEffect(classBonus);
            }
        }

        // bonus HP due to constitution
        // this has to be based on the final constitution value from the breakdown
        double con = m_pConstitutionBreakdown->Total();
        double bonus = BaseStatToBonus(con);
        if (bonus != 0)
        {
            ActiveEffect conBonus(
                    Bonus_ability,
                    "Constitution bonus",
                    MAX_LEVEL,  // con bonus applies for every level
                    bonus,
                    "");        // no tree
            AddOtherEffect(conBonus);
        }

        // add the false life bonus
        BreakdownItem *pBreakdown = FindBreakdown(Breakdown_FalseLife);
        pBreakdown->AttachObserver(this);
        bonus = pBreakdown->Total();
        if (bonus != 0)
        {
            ActiveEffect falseLifeBonus(
                    Bonus_falseLife,
                    "False Life",
                    1,
                    bonus,
                    "");        // no tree
            AddOtherEffect(falseLifeBonus);
        }
    }
}

bool BreakdownItemHitpoints::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    // see if this feat effect applies to us, if so add it
    if (effect.Type() == Effect_Hitpoints)
    {
        isUs = true;
    }
    return isUs;
}

void BreakdownItemHitpoints::UpdateClassChanged(
        Character * charData,
        ClassType classFrom,
        ClassType classTo,
        size_t level)
{
    BreakdownItem::UpdateClassChanged(charData, classFrom, classTo, level);
    // need to re-create other effects list (class hp)
    CreateOtherEffects();
    Populate();
}

void BreakdownItemHitpoints::UpdateTotalChanged(
        BreakdownItem * item,
        BreakdownType type)
{
    // total constitution has changed, update
    CreateOtherEffects();
    // do base class stuff also
    BreakdownItem::UpdateTotalChanged(item, type);
}
