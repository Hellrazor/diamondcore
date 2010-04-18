/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: faction_champions
SD%Complete: 0
SDComment:
SDCategory: Crusader Coliseum
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

#define AI_MELEE    0
#define AI_RANGED   1
#define AI_HEALER   2

#define SP_ANTI_AOE     68595
#define SP_PVP_TRINKET  65547

struct DIAMOND_DLL_DECL boss_faction_championAI : public ScriptedAI
{
    boss_faction_championAI(Creature *pCreature, uint32 aitype) : ScriptedAI(pCreature) 
    {
        pInstance = (ScriptedInstance *) pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        mAIType = aitype;
        Init();
    }
    
    ScriptedInstance *pInstance;
    bool m_bIsRegularMode;
    uint32 mAIType;
    
    uint32 ThreatTimer;
    uint32 CCTimer;

    void Init()
    {
        CCTimer = rand()%10000;
        ThreatTimer = 5000;
    }
    
    float CalculateThreat(float distance, float armor, uint32 health)
    {
        float dist_mod = (mAIType == AI_MELEE) ? 15.0f/(15.0f + distance) : 1.0f;
        float armor_mod = (mAIType == AI_MELEE) ? armor / 16635.0f : 0.0f;
        float eh = (health+1) * (1.0f + armor_mod);
        return dist_mod * 30000.0f / eh;
    }
    
    /*void UpdateThreat()
    {
        ThreatList const& tList = m_creature->getThreatManager().getThreatList();
        ThreatList::const_iterator itr;
        bool empty = true;
        for(itr = tList.begin(); itr!=tList.end(); ++itr)
        {
            Unit* pUnit = Unit::GetUnit((*m_creature), (*itr)->getUnitGuid());
            if (pUnit && m_creature->getThreatManager().getThreat(pUnit))
            {
                if(pUnit->GetTypeId()==TYPEID_PLAYER)
                {
                    float threat = CalculateThreat(m_creature->GetDistance2d(pUnit), (float)pUnit->GetArmor(), pUnit->GetHealth());
                    m_creature->getThreatManager().modifyThreatPercent(pUnit, -100);
                    m_creature->AddThreat(pUnit, 1000000.0f * threat);
                    empty = false;
                }
            }
        }
        if(empty)
            EnterEvadeMode();
    }*/

    void UpdatePower()
    {
        if(m_creature->getPowerType() == POWER_MANA)
            m_creature->ModifyPower(POWER_MANA, m_creature->GetMaxPower(POWER_MANA) / 3);
        //else if(m_creature->getPowerType() == POWER_ENERGY)
        //    m_creature->ModifyPower(POWER_ENERGY, 100);
    }
    
    void RemoveCC()
    {
        m_creature->RemoveSpellsCausingAura(SPELL_AURA_MOD_STUN);
        m_creature->RemoveSpellsCausingAura(SPELL_AURA_MOD_FEAR);
        m_creature->RemoveSpellsCausingAura(SPELL_AURA_MOD_ROOT);
        m_creature->RemoveSpellsCausingAura(SPELL_AURA_MOD_PACIFY);
        m_creature->RemoveSpellsCausingAura(SPELL_AURA_MOD_CONFUSE);
        //DoCast(m_creature, SP_PVP_TRINKET);
    }
    
    void JustDied(Unit *killer)
    {
        if(pInstance)
            pInstance->SetData(TYPE_FACTION_CHAMPIONS, SPECIAL);
    }
    
    void Aggro(Unit *who)
    {
        DoCast(m_creature, SP_ANTI_AOE, true);
        if(pInstance)
            pInstance->SetData(TYPE_FACTION_CHAMPIONS, 5);
    }
    
    void Reset()
    {
        if(pInstance)
            pInstance->SetData(TYPE_FACTION_CHAMPIONS, NOT_STARTED);
    }

    Creature* SelectRandomFriendlyMissingBuff(uint32 spell)
    {
        std::list<Creature *> lst = DoFindFriendlyMissingBuff(40.0f, spell);
        std::list<Creature *>::const_iterator itr = lst.begin();
        if(lst.empty()) 
            return NULL;
        advance(itr, rand()%lst.size());
        return (*itr);
    }

    Unit* SelectEnemyCaster(bool casting)
    {
        ThreatList const& tList = m_creature->getThreatManager().getThreatList();
        ThreatList::const_iterator iter;
        for(iter = tList.begin(); iter!=tList.end(); ++iter)
        {
            Unit *target;
            if(target = Unit::GetUnit((*m_creature),(*iter)->getUnitGuid()))
                if(target->getPowerType() == POWER_MANA)
                    return target;
        }
        return NULL;
    }

    uint32 EnemiesInRange(float distance)
    {
        ThreatList const& tList = m_creature->getThreatManager().getThreatList();
        ThreatList::const_iterator iter;
        uint32 count = 0;
        for(iter = tList.begin(); iter!=tList.end(); ++iter)
        {
            Unit *target;
            if(target = Unit::GetUnit((*m_creature),(*iter)->getUnitGuid()))
                if(m_creature->GetDistance2d(target) < distance)
                    ++count;
        }
        return count;
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);

            if(mAIType == AI_MELEE)
                DoStartMovement(pWho);
            else
                DoStartMovement(pWho, 20.0f);
        }
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if(ThreatTimer < uiDiff)
        {
            UpdatePower();
            //UpdateThreat();
            ThreatTimer = 4000;
        }
        else
            ThreatTimer -= uiDiff;
        
        if(CCTimer < uiDiff)
        {
            RemoveCC();
            CCTimer = 10000+rand()%5000;
        }
        else
            CCTimer -= uiDiff;

        if(mAIType == AI_MELEE)
            DoMeleeAttackIfReady();
    }
};

/********************************************************************
                            HEALERS
********************************************************************/

#define SP_LIFEBLOOM        66093
#define H_SP_LIFEBLOOM      67957
#define SP_NOURISH          66066
#define H_SP_NOURISH        67965
#define SP_REGROWTH         66067
#define H_SP_REGROWTH       67968
#define SP_REJUVENATION     66065
#define H_SP_REJUVENATION   67971
#define SP_TRANQUILITY      66086
#define H_SP_TRANQUILITY    67974
#define SP_BARKSKIN         65860 //1 min cd
#define SP_THORNS           66068
#define SP_NATURE_GRASP     66071 //1 min cd, self buff

struct DIAMOND_DLL_DECL boss_fc_resto_druidAI : public boss_faction_championAI
{
    boss_fc_resto_druidAI(Creature *pCreature) : boss_faction_championAI(pCreature, AI_HEALER) {Init();}
    
    uint32 GCDTimer;
    uint32 GraspTimer;
    uint32 TranquilityTimer;
    uint32 BarkSkinTimer;
    
    void Init()
    {
        GCDTimer = 2500;
        BarkSkinTimer = 3000;
        GraspTimer = 5000 + rand()%40000;
        TranquilityTimer = 20000 + rand()%20000;
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        if(GraspTimer < uiDiff)
        {
            DoCast(m_creature, SP_NATURE_GRASP);
            GraspTimer = 60000 + rand()%10000;
        }
        else
            GraspTimer -= uiDiff;
        
        if(TranquilityTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SP_TRANQUILITY : H_SP_TRANQUILITY) == CAST_OK)
                TranquilityTimer = 600000;
        }
        else
            TranquilityTimer -= uiDiff;

        if(BarkSkinTimer < uiDiff)
        {
            if(m_creature->GetHealth()*2 < m_creature->GetMaxHealth())
            {
                if (DoCastSpellIfCan(m_creature, SP_BARKSKIN) == CAST_OK)
                    BarkSkinTimer = 60000;
            }
            else
                BarkSkinTimer = 5000;
        }
        else
            BarkSkinTimer -= uiDiff;
        
        if(GCDTimer < uiDiff)
        {
            switch(urand(0,4))
            {
                case 0:
                    if(Unit *target = DoSelectLowestHpFriendly(40.0f))
                        DoCastSpellIfCan(target, m_bIsRegularMode ? SP_LIFEBLOOM : H_SP_LIFEBLOOM);
                    break;
                case 1:
                    if(Unit *target = DoSelectLowestHpFriendly(40.0f))
                        DoCastSpellIfCan(target, m_bIsRegularMode ? SP_NOURISH : H_SP_NOURISH);
                    break;
                case 2:
                    if(Unit *target = DoSelectLowestHpFriendly(40.0f))
                        DoCastSpellIfCan(target, m_bIsRegularMode ? SP_REGROWTH : H_SP_REGROWTH);
                    break;
                case 3:
                    if(Unit *target = DoSelectLowestHpFriendly(40.0f))
                        DoCastSpellIfCan(target, m_bIsRegularMode ? SP_REJUVENATION : H_SP_REJUVENATION);
                    break;
                case 4:
                    if(Creature *target = SelectRandomFriendlyMissingBuff(SP_THORNS))
                        DoCastSpellIfCan(target, SP_THORNS);
                    break;
            }
            GCDTimer = 2000;
        }
        else
            GCDTimer -= uiDiff;
        
        boss_faction_championAI::UpdateAI(uiDiff);
    }
};

#define SP_HEALING_WAVE         66055
#define H_SP_HEALING_WAVE       68115
#define SP_RIPTIDE              66053
#define H_SP_RIPTIDE            68118
#define SP_SPIRIT_CLEANSE       66056 //friendly only
#define SP_HEROISM              65983
#define SP_BLOODLUST            65980
#define SP_HEX                  66054
#define SP_EARTH_SHIELD         66063
#define SP_EARTH_SHOCK          65973
#define H_SP_EARTH_SHOCK        68100

struct DIAMOND_DLL_DECL boss_fc_resto_shammyAI : public boss_faction_championAI
{
    boss_fc_resto_shammyAI(Creature *pCreature) : boss_faction_championAI(pCreature, AI_HEALER) {Init();}
    
    uint32 HeroTimer;
    uint32 HexTimer;
    uint32 GCDTimer;
    
    void Init()
    {
        HeroTimer = 30000 + rand()%30000;
        HexTimer = 10000 + rand()%30000;
        GCDTimer = 2500;
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        if(HeroTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SP_HEROISM) == CAST_OK)
                HeroTimer = 600000;
        }
        else
            HeroTimer -= uiDiff;
        
        if(HexTimer < uiDiff)
        {
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,1))
                if (DoCastSpellIfCan(target, SP_HEX) == CAST_OK)
                    HexTimer = 20000 + rand()%20000;
        }
        else
            HexTimer -= uiDiff;
        
        if(GCDTimer < uiDiff)
        {
            switch(urand(0,5))
            {
                case 0: case 1:
                    if(Unit *target = DoSelectLowestHpFriendly(40.0f))
                        DoCastSpellIfCan(target, m_bIsRegularMode ? SP_HEALING_WAVE : H_SP_HEALING_WAVE);
                    break;
                case 2:
                    if(Unit *target = DoSelectLowestHpFriendly(40.0f))
                        DoCastSpellIfCan(target, m_bIsRegularMode ? SP_RIPTIDE : H_SP_RIPTIDE);
                    break;
                case 3:
                    if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
                        DoCastSpellIfCan(target, m_bIsRegularMode ? SP_EARTH_SHOCK : H_SP_EARTH_SHOCK);
                    break;
                case 4:
                    if(Unit *target = DoSelectLowestHpFriendly(40.0f))
                        DoCastSpellIfCan(target, SP_SPIRIT_CLEANSE);
                    break;
                case 5:
                    if(Unit *target = SelectRandomFriendlyMissingBuff(SP_EARTH_SHIELD))
                        DoCastSpellIfCan(target, SP_EARTH_SHIELD);
                    break;
            }
            GCDTimer = 2000;
        }
        else
            GCDTimer -= uiDiff;
        
        boss_faction_championAI::UpdateAI(uiDiff);
    }
};

#define SP_HAND_OF_FREEDOM    68757 //25 sec cd
#define SP_BUBBLE             66010 //5 min cd
#define SP_CLEANSE            66116
#define SP_FLASH_OF_LIGHT     66113
#define H_SP_FLASH_OF_LIGHT   68008
#define SP_HOLY_LIGHT         66112
#define H_SP_HOLY_LIGHT       68011
#define SP_HAND_OF_PROTECTION 66009
#define SP_HOLY_SHOCK         66114
#define H_SP_HOLY_SHOCK       68015
#define SP_HAND_OF_PROTECTION 66009
#define SP_HAMMER_OF_JUSTICE  66613

struct DIAMOND_DLL_DECL boss_fc_holy_pallyAI : public boss_faction_championAI
{
    boss_fc_holy_pallyAI(Creature *pCreature) : boss_faction_championAI(pCreature, AI_HEALER) {Init();}
    
    bool BubbleUsed;
    bool ProtectionUsed;
    uint32 FreedomTimer;
    uint32 GCDTimer;
    uint32 ShockTimer;
    uint32 HammerOfJusticeTimer;
    
    void Init()
    {
        GCDTimer = 3000;
        ShockTimer = 6000 + rand()%4000;
        BubbleUsed = false;
        ProtectionUsed = false;
        FreedomTimer = 10000 + rand()%15000;
        HammerOfJusticeTimer = 15000 + rand()%10000;
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
       
        //cast bubble at 20% hp
        if(!BubbleUsed && m_creature->GetHealth()*5 < m_creature->GetMaxHealth())
        {
            if (DoCastSpellIfCan(m_creature, SP_BUBBLE) == CAST_OK)
                BubbleUsed = true;
        }
        
        if(!ProtectionUsed)
        {
            if(Unit *target = DoSelectLowestHpFriendly(40.0f))
            {
                if(target->GetHealth() * 100 / target->GetMaxHealth() < 15)
                {
                    if (DoCastSpellIfCan(target, SP_HAND_OF_PROTECTION) == CAST_OK)
                        ProtectionUsed = true;
                }
            }
        }

        if(ShockTimer < uiDiff)
        {
            if(Unit *target = DoSelectLowestHpFriendly(40.0f))
            {
                if (DoCastSpellIfCan(target, m_bIsRegularMode ? SP_HOLY_SHOCK : H_SP_HOLY_SHOCK) == CAST_OK)
                    ShockTimer = 6000 + rand()%4000;
            }
            else
                ShockTimer = 1000;
        }
        else
            ShockTimer -=uiDiff;
        
        if(FreedomTimer < uiDiff)
        {   
            if(Unit *target = SelectRandomFriendlyMissingBuff(SP_HAND_OF_FREEDOM))
                if (DoCastSpellIfCan(target, SP_HAND_OF_FREEDOM) == CAST_OK)
                    FreedomTimer = 25000 + rand()%10000;
        }
        else
            FreedomTimer -= uiDiff;

        if(HammerOfJusticeTimer < uiDiff)
        {
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0))
            {
                if(m_creature->GetDistance2d(target) < 15)
                {
                    if (DoCastSpellIfCan(target, SP_HAMMER_OF_JUSTICE) == CAST_OK)
                        HammerOfJusticeTimer=40000+rand()%5000;
                }
                else
                    HammerOfJusticeTimer = 1000;
            }
        }
        else
            HammerOfJusticeTimer -= uiDiff;
        
        if(GCDTimer < uiDiff)
        {
            switch(urand(0,4))
            {
                case 0: case 1:
                    if(Unit *target = DoSelectLowestHpFriendly(40.0f))
                        DoCastSpellIfCan(target, m_bIsRegularMode ? SP_FLASH_OF_LIGHT : H_SP_FLASH_OF_LIGHT);
                    break;
                case 2: case 3:
                    if(Unit *target = DoSelectLowestHpFriendly(40.0f))
                        DoCastSpellIfCan(target, m_bIsRegularMode ? SP_HOLY_LIGHT : H_SP_HOLY_LIGHT);
                    break;
                case 4:
                    if(Unit *target = DoSelectLowestHpFriendly(40.0f))
                        DoCastSpellIfCan(target, SP_CLEANSE);
                    break;
            }
            GCDTimer = 2500;
        }
        else
            GCDTimer -= uiDiff;
        
        boss_faction_championAI::UpdateAI(uiDiff);
    }
};

#define SP_RENEW            66177
#define H_SP_RENEW          68035
#define SP_SHIELD           66099
#define H_SP_SHIELD         68032
#define SP_FLASH_HEAL       66104
#define H_SP_FLASH_HEAL     68023
#define SP_DISPEL           65546
#define SP_PSYCHIC_SCREAM   65543
#define SP_MANA_BURN        66100
#define H_SP_MANA_BURN      68026

struct DIAMOND_DLL_DECL boss_fc_holy_priestAI : public boss_faction_championAI
{
    boss_fc_holy_priestAI(Creature *pCreature) : boss_faction_championAI(pCreature, AI_HEALER) {Init();}
    
    uint32 GCDTimer;
    uint32 ScreamTimer;
    
    void Init()
    {
        GCDTimer = 2500;
        ScreamTimer = 5000;
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(ScreamTimer < uiDiff)
        {
            if(EnemiesInRange(10.0f) > 2)
            {
                if (DoCastSpellIfCan(m_creature, SP_PSYCHIC_SCREAM) == CAST_OK)
                    ScreamTimer = 30000;
            }
            else
                ScreamTimer = 3000;
        }
        else
            ScreamTimer -= uiDiff;
        
        if(GCDTimer < uiDiff)
        {
            switch(urand(0,5))
            {
                case 0:
                    if(Unit *target = DoSelectLowestHpFriendly(40.0f))
                        DoCastSpellIfCan(target, m_bIsRegularMode ? SP_RENEW : H_SP_RENEW);
                    break;
                case 1:
                    if(Unit *target = DoSelectLowestHpFriendly(40.0f))
                        DoCastSpellIfCan(target, m_bIsRegularMode ? SP_SHIELD : H_SP_SHIELD);
                    break;
                case 2: case 3:
                    if(Unit *target = DoSelectLowestHpFriendly(40.0f))
                        DoCastSpellIfCan(target, m_bIsRegularMode ? SP_FLASH_HEAL : H_SP_FLASH_HEAL);
                    break;
                case 4:
                    if(Unit *target = urand(0,1) ? SelectUnit(SELECT_TARGET_RANDOM,0) : DoSelectLowestHpFriendly(40.0f))
                        DoCast(target, SP_DISPEL);
                    break;
                case 5:
                    if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0))
                        DoCastSpellIfCan(target, m_bIsRegularMode ? SP_MANA_BURN : H_SP_MANA_BURN);
                    break;
            }
            GCDTimer = 2000;
        }
        else
            GCDTimer -= uiDiff;
        
        boss_faction_championAI::UpdateAI(uiDiff);
    }
};

/********************************************************************
                            RANGED
********************************************************************/

#define SP_SILENCE          65542
#define SP_VAMPIRIC_TOUCH   65490
#define H_SP_VAMPIRIC_TOUCH 68091
#define SP_SW_PAIN          65541
#define H_SP_SW_PAIN        68088
#define SP_MIND_FLAY        65488
#define H_SP_MIND_FLAY      68042
#define SP_MIND_BLAST       65492
#define H_SP_MIND_BLAST     68038
#define SP_HORROR           65545
#define SP_DISPERSION       65544
#define SP_SHADOWFORM       16592

struct DIAMOND_DLL_DECL boss_fc_shadow_priestAI : public boss_faction_championAI
{
    boss_fc_shadow_priestAI(Creature *pCreature) : boss_faction_championAI(pCreature, AI_RANGED) {Init();}
    
    uint32 SilenceTimer;
    uint32 PsychicScreamTimer;
    uint32 MindBlastTimer;
    uint32 GCDTimer;
    uint32 DispersionTimer;
    
    void Init()
    {
        SilenceTimer = 15000;
        PsychicScreamTimer = 8000;
        MindBlastTimer = 15000;
        GCDTimer = 3000;
        DispersionTimer = 1000;
    }

    void Aggro(Unit *who)
    {
        boss_faction_championAI::Aggro(who);
        DoCast(m_creature, SP_SHADOWFORM, true);
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        if(PsychicScreamTimer < uiDiff)
        {
            if(EnemiesInRange(10.0f) > 2)
            {
                if (DoCastSpellIfCan(m_creature, SP_PSYCHIC_SCREAM) == CAST_OK)
                    PsychicScreamTimer = 24000;
            }
            else
                PsychicScreamTimer = 3000;
        }
        else
            PsychicScreamTimer -= uiDiff;

        if(DispersionTimer < uiDiff)
        {
            if(m_creature->GetHealth()*5 < m_creature->GetMaxHealth())
            {
                if (DoCastSpellIfCan(m_creature, SP_DISPERSION) == CAST_OK)
                    DispersionTimer = 180000;
            }
            else
                DispersionTimer = 1000;
        }
        else
            DispersionTimer -= uiDiff;
        
        if(SilenceTimer < uiDiff)
        {
            if(Unit *target = SelectEnemyCaster(false))
                if (DoCastSpellIfCan(target, SP_SILENCE) == CAST_OK)
                    SilenceTimer = 45000;
        }
        else
            SilenceTimer -= uiDiff;

        if(MindBlastTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SP_MIND_BLAST : H_SP_MIND_BLAST) == CAST_OK)
                MindBlastTimer = 8000;
        }
        else
            MindBlastTimer -= uiDiff;
        
        if(GCDTimer < uiDiff)
        {
            switch(urand(0,4))
            {
                case 0: case 1:
                    DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SP_MIND_FLAY : H_SP_MIND_FLAY);
                    break;
                case 2:
                    if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0))
                        DoCastSpellIfCan(target, m_bIsRegularMode ? SP_VAMPIRIC_TOUCH : H_SP_VAMPIRIC_TOUCH);
                    break;
               case 3:
                    if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0))
                        DoCastSpellIfCan(target, m_bIsRegularMode ? SP_SW_PAIN : H_SP_SW_PAIN);
                    break;
               case 4:
                    if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0))
                        DoCastSpellIfCan(target, SP_DISPEL);
                    break;
            }
            GCDTimer = 2000;
        }
        else
            GCDTimer -= uiDiff;
        
        boss_faction_championAI::UpdateAI(uiDiff);
    }
};

#define SP_HELLFIRE             65816
#define H_SP_HELLFIRE           68146
#define SP_CORRUPTION           65810
#define H_SP_CORRUPTION         68134
#define SP_Curse_of_Agony       65814
#define H_SP_Curse_of_Agony     68137       
#define SP_Curse_of_Exhaustion  65815
#define SP_Fear                 65809 //8s
#define SP_Searing_Pain         65819
#define H_SP_Searing_Pain       68149
#define SP_Shadow_Bolt          65821
#define H_SP_Shadow_Bolt        68152
#define SP_Summon_Felhunter     67514
#define SP_Unstable_Affliction  65812
#define H_SP_Unstable_Affliction 68155 //15s

struct DIAMOND_DLL_DECL boss_fc_warlockAI : public boss_faction_championAI
{
    boss_fc_warlockAI(Creature *pCreature) : boss_faction_championAI(pCreature, AI_RANGED) {Init();}
    
    uint32 FearTimer;
    uint32 UnstableAfflictionTimer;
    uint32 HellfireTimer;
    uint32 GCDTimer;

    void Init()
    {
        FearTimer = 4000+rand()%1000;
        UnstableAfflictionTimer = 2000+rand()%1000;
        HellfireTimer = 15000;
        GCDTimer = 2500;
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(FearTimer < uiDiff)
        {
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0))
                if (DoCastSpellIfCan(target, SP_Fear) == CAST_OK)
                    FearTimer = 8000;
        }
        else
            FearTimer -= uiDiff;

        if(HellfireTimer < uiDiff)
        {
            if(EnemiesInRange(10.0f) > 2)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SP_HELLFIRE : H_SP_HELLFIRE) == CAST_OK)
                    HellfireTimer = 40000;
            }
            else
                HellfireTimer = 5000;
        }
        else
            HellfireTimer -= uiDiff;

        if(UnstableAfflictionTimer  < uiDiff)
        {
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0))
                if (DoCastSpellIfCan(target, m_bIsRegularMode ? SP_Unstable_Affliction : H_SP_Unstable_Affliction) == CAST_OK)
                    UnstableAfflictionTimer = 15000;
        }
        else
            UnstableAfflictionTimer  -= uiDiff;
        
        if(GCDTimer < uiDiff)
        {
            switch(urand(0,5))
            {
                case 0:
                    DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SP_Searing_Pain : H_SP_Searing_Pain);
                    break;
                case 1: case 2:
                    DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SP_Shadow_Bolt : H_SP_Shadow_Bolt);
                    break;
                case 3:
                    if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0))
                        DoCastSpellIfCan(target, m_bIsRegularMode ? SP_CORRUPTION : H_SP_CORRUPTION);
                    break;
                case 4:
                    if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0))
                        DoCastSpellIfCan(target, m_bIsRegularMode ? SP_Curse_of_Agony : H_SP_Curse_of_Agony);
                    break;
                case 5:
                    if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0))
                        DoCastSpellIfCan(target, SP_Curse_of_Exhaustion);
                    break;
             }
             GCDTimer = 2500;
        }
        else
            GCDTimer -= uiDiff;

        boss_faction_championAI::UpdateAI(uiDiff);
    }
};

#define SP_Arcane_Barrage   65799 //3s
#define H_SP_Arcane_Barrage 67995
#define SP_Arcane_Blast 65791
#define H_SP_Arcane_Blast 67998
#define SP_Arcane_Explosion 65800
#define H_SP_Arcane_Explosion 68001
#define SP_Blink  65793 //15s
#define SP_Counterspell 65790 //24s
#define SP_Frost_Nova 65792 //25s
#define SP_Frostbolt 65807
#define H_SP_Frostbolt 68004
#define SP_Ice_Block  65802 //5min
#define SP_Polymorph 65801 //15s

struct DIAMOND_DLL_DECL boss_fc_mageAI : public boss_faction_championAI
{
    boss_fc_mageAI(Creature *pCreature) : boss_faction_championAI(pCreature, AI_RANGED) {Init();}
    
    uint32 ArcaneBarrageTimer;
    uint32 BlinkTimer;
    uint32 CounterspellTimer;
    uint32 FrostNovaTimer;
    bool IceBlockUsed;
    uint32 PolymorphTimer;
    uint32 GCDTimer;

    void Init()
    {
        ArcaneBarrageTimer = 2000+rand()%1000;
        BlinkTimer =  8000+rand()%1000;
        CounterspellTimer = 5000+rand()%1000;
        FrostNovaTimer =  7000+rand()%2000;
        IceBlockUsed = false;
        PolymorphTimer = 3000+rand()%3000;
        GCDTimer = 2000;
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(CounterspellTimer < uiDiff)
        {
            if(Unit *target = SelectEnemyCaster(false))
                if (DoCastSpellIfCan(target, SP_Counterspell) == CAST_OK)
                    CounterspellTimer = 24000;
        }
        else
            CounterspellTimer -= uiDiff;

        if(BlinkTimer < uiDiff) // escape
        {
            if(m_creature->GetHealth()*2 < m_creature->GetMaxHealth() && EnemiesInRange(10.0f)>3)
            {
                DoCast(m_creature, SP_Frost_Nova, true);
                if (DoCastSpellIfCan(m_creature , SP_Blink) == CAST_OK)
                    BlinkTimer = 25000;
            }
            else
                BlinkTimer = 5000;
        }
        else
            BlinkTimer -= uiDiff;

        if(!IceBlockUsed && m_creature->GetHealth()*5 < m_creature->GetMaxHealth())
            if (DoCastSpellIfCan(m_creature, SP_Ice_Block) == CAST_OK)
                IceBlockUsed = true;

        if(PolymorphTimer < uiDiff)
        {
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,1))
                if (DoCastSpellIfCan(target, SP_Polymorph) == CAST_OK)
                    PolymorphTimer = 15000;
        }
        else
            PolymorphTimer -= uiDiff;
    
        if(GCDTimer < uiDiff)
        {
            switch(urand(0,2))
            {
                case 0:
                    DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SP_Arcane_Barrage : H_SP_Arcane_Barrage);
                    break;
                case 1:
                    DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SP_Arcane_Blast : H_SP_Arcane_Blast);
                    break;
                case 2:
                    if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0))
                        DoCastSpellIfCan(target, m_bIsRegularMode ? SP_Frostbolt : H_SP_Frostbolt);
                    break;
            }
            GCDTimer = 2500;
        }
        else
            GCDTimer -= uiDiff;

        boss_faction_championAI::UpdateAI(uiDiff);
    }
};


#define SP_AIMED_SHOT 65883
#define H_SP_AIMED_SHOT 67978//10s
#define SP_Call_Pet 67777//perm
#define SP_Deterrence 65871 //90s
#define SP_Disengage 65869 //30s
#define SP_EXPLOSIVE_SHOT   65866
#define H_SP_EXPLOSIVE_SHOT 67984 //6s
#define SP_Frost_Trap 65880 //30s
#define SP_SHOOT 65868 //1.7s
#define H_SP_SHOOT 67989
#define SP_Steady_Shot  65867 //3s
#define SP_WING_CLIP 66207 //6s
#define SP_Wyvern_Sting 65877 //60s

struct DIAMOND_DLL_DECL boss_fc_hunterAI : public boss_faction_championAI
{
    boss_fc_hunterAI(Creature *pCreature) : boss_faction_championAI(pCreature, AI_RANGED) {Init();}
    
    uint32 AimedShotTimer;
    uint32 CallPetTimer;
    uint32 DeterrenceTimer;
    uint32 DisengageTimer;
    uint32 ExplosiveShotTimer;
    uint32 FrostTrapTimer;
    uint32 AutoShootTimer;
    uint32 SteadyShotTimer;
    uint32 WingClipTimer;
    uint32 WyvernStingTimer;
    uint32 GCDTimer;

    void Init()
    {
        AimedShotTimer = 1000+rand()%2000;
        DeterrenceTimer = 20000;
        DisengageTimer = 10000+rand()%3000;
        ExplosiveShotTimer = 2000+rand()%3000;
        FrostTrapTimer = 11000+rand()%5000;
        SteadyShotTimer = 3200+rand()%2000;
        WingClipTimer =  6000+rand()%2000;
        WyvernStingTimer = 7000+rand()%3000;
        GCDTimer = 1000;
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(DisengageTimer < uiDiff)
        {
            if(EnemiesInRange(10.0f) > 3)
            {
                if (DoCastSpellIfCan(m_creature , SP_Disengage) == CAST_OK)
                    DisengageTimer = 30000;
            }
            else
                DisengageTimer = 3000;
        }
        else
            DisengageTimer -= uiDiff;

        if(DeterrenceTimer < uiDiff)
        {
            if(m_creature->GetHealth()*5 < m_creature->GetMaxHealth())
            {
                if (DoCastSpellIfCan(m_creature, SP_Deterrence) == CAST_OK)
                    DeterrenceTimer = 90000;
            }
            else
                DeterrenceTimer = 3000;
        }
        else
            DeterrenceTimer -= uiDiff;

        if(WyvernStingTimer < uiDiff)
        {
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,1))
                if (DoCastSpellIfCan(target, SP_Wyvern_Sting) == CAST_OK)
                    WyvernStingTimer = 60000;
        }
        else
            WyvernStingTimer -= uiDiff;

        if(FrostTrapTimer < uiDiff)
        {
            //if (DoCastSpellIfCan(m_creature, SP_Frost_Trap) == CAST_OK) // disabled, because of core bug
                FrostTrapTimer = 30000 + rand()%1000;
        }
        else
            FrostTrapTimer -= uiDiff;

        if(WingClipTimer < uiDiff)
        {
            if(m_creature->GetDistance2d(m_creature->getVictim()) < 5.0f)
                if (DoCastSpellIfCan(m_creature->getVictim(), SP_WING_CLIP) == CAST_OK)
                    WingClipTimer = 5000;
        }
        else
            WingClipTimer-= uiDiff;

        if(GCDTimer < uiDiff)
        {
            switch(urand(0,3))
            {
                case 0: case 1:
                    DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SP_SHOOT : H_SP_SHOOT);
                    break;
                case 2:
                    DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SP_EXPLOSIVE_SHOT : H_SP_EXPLOSIVE_SHOT);
                    break;
                case 3:
                    DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SP_AIMED_SHOT : H_SP_AIMED_SHOT);
                    break;
            }
            GCDTimer = 2500;
        }
        else
            GCDTimer -= uiDiff; 

        boss_faction_championAI::UpdateAI(uiDiff);
    }
};

#define SP_CYCLONE  65859 //6s
#define SP_Entangling_Roots 65857 //10s
#define SP_Faerie_Fire 65863
#define SP_Force_of_Nature 65861 //180s
#define SP_Insect_Swarm 65855
#define H_SP_Insect_Swarm 67942
#define SP_Moonfire  65856 //5s
#define H_SP_Moonfire 67945
#define SP_Starfire 65854
#define H_SP_Starfire 67948
#define SP_Wrath     65862
#define H_SP_Wrath     67952

struct DIAMOND_DLL_DECL boss_fc_boomkinAI : public boss_faction_championAI
{
    boss_fc_boomkinAI(Creature *pCreature) : boss_faction_championAI(pCreature, AI_RANGED) {Init();}
    
    uint32 BarkskinTimer;
    uint32 CycloneTimer;
    uint32 EntanglingRootsTimer;
    uint32 MoonfireTimer;
    uint32 FaerieFireTimer;
    uint32 GCDTimer;
    
    void Init()
    {
        BarkskinTimer = 6000;
        CycloneTimer = 5000;
        EntanglingRootsTimer = 2000+rand()%1000;
        MoonfireTimer = 1000+rand()%1000;
        FaerieFireTimer = 10000;
        GCDTimer = 1500;
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(BarkskinTimer < uiDiff)
        {
            if(m_creature->GetHealth()*2 < m_creature->GetMaxHealth())
            {
                if (DoCastSpellIfCan(m_creature, SP_BARKSKIN) == CAST_OK)
                    BarkskinTimer = 60000;
            }
            else
                BarkskinTimer = 3000;
        }
        else
            BarkskinTimer -= uiDiff;

        if(CycloneTimer < uiDiff)
        {
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,1))
                if (DoCastSpellIfCan(target, SP_CYCLONE) == CAST_OK)
                    CycloneTimer = 6000+rand()%2000;
        }
        else
            CycloneTimer -= uiDiff;

        if(EntanglingRootsTimer < uiDiff)
        {
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0))
                if (DoCastSpellIfCan(target, SP_Entangling_Roots) == CAST_OK)
                    EntanglingRootsTimer = 10000 + rand()%2000;
        }
        else
            EntanglingRootsTimer -= uiDiff;

        if(FaerieFireTimer < uiDiff)
        {
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0))
                if (DoCastSpellIfCan(target, SP_Faerie_Fire) == CAST_OK)
                    FaerieFireTimer = 10000 + rand()%8000;
        }
        else
            FaerieFireTimer -= uiDiff;

        if(GCDTimer < uiDiff)
        {
            switch(urand(0,6))
            {
                case 0: case 1:
                    DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SP_Moonfire : H_SP_Moonfire);
                    break;
                case 2:
                    if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0))
                        DoCastSpellIfCan(target,m_bIsRegularMode ? SP_Insect_Swarm : H_SP_Insect_Swarm);
                    break;
                case 3:
                    if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0))
                        DoCastSpellIfCan(m_creature->getVictim(),m_bIsRegularMode ? SP_Starfire : H_SP_Starfire);
                    break;
                case 4: case 5: case 6:
                    DoCastSpellIfCan(m_creature->getVictim(),m_bIsRegularMode ? SP_Wrath : H_SP_Wrath);
                    break;
            }
            GCDTimer = 2000;
        }
        else
            GCDTimer -= uiDiff; 
          
        boss_faction_championAI::UpdateAI(uiDiff);
    }
};

/********************************************************************
                            MELEE
********************************************************************/

#define SP_BLADESTORM           65947
#define SP_INTIMIDATING_SHOUT   65930
#define SP_MORTAL_STRIKE        65926
#define SP_CHARGE               68764
#define SP_DISARM               65935
#define SP_OVERPOWER            65924
#define SP_SUNDER_ARMOR         65936
#define SP_SHATTERING_THROW     65940
#define SP_RETALIATION          65932

struct DIAMOND_DLL_DECL boss_fc_warriorAI : public boss_faction_championAI
{
    boss_fc_warriorAI(Creature *pCreature) : boss_faction_championAI(pCreature, AI_MELEE) {Init();}
    
    uint32 BladestormTimer;
    uint32 FearTimer;
    uint32 MortalStrikeTimer;
    uint32 ChargeTimer;
    uint32 SunderArmorTimer;
    uint32 RetaliationTimer;
    uint32 ShatteringThrowTimer;
    uint32 DisarmTimer;
    
    void Init()
    {
        BladestormTimer = 20000 + rand()%10000;
        FearTimer = 10000 + rand()%10000;
        MortalStrikeTimer = 6000 + rand()%4000;
        ChargeTimer = 1000;
        SunderArmorTimer = 5000;
        RetaliationTimer = 30000 + rand()%8000;
        ShatteringThrowTimer =  10000 + rand()%30000;
        DisarmTimer = 5000 + rand()%20000;
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        if(BladestormTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SP_BLADESTORM) == CAST_OK)
                BladestormTimer = 90000 + rand()%5000;
        }
        else
            BladestormTimer -= uiDiff;
        
        if(FearTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SP_INTIMIDATING_SHOUT) == CAST_OK)
                FearTimer = 40000 + rand()%40000;
        }
        else
            FearTimer -= uiDiff;
        
        if(MortalStrikeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SP_MORTAL_STRIKE) == CAST_OK)
                MortalStrikeTimer = 6000 + rand()%2000;
        }
        else
            MortalStrikeTimer -= uiDiff;

        if(SunderArmorTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SP_SUNDER_ARMOR) == CAST_OK)
                SunderArmorTimer = 2000+rand()%5000;
        }
        else
            SunderArmorTimer -= uiDiff;

        if(ChargeTimer < uiDiff)
        {
            if(m_creature->IsInRange(m_creature->getVictim(), 8.0f, 25.0f, false))
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SP_CHARGE) == CAST_OK)
                    ChargeTimer = 12000;
            }
            else
                ChargeTimer = 2000;
        }
        else
            ChargeTimer -= uiDiff;

        if(RetaliationTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SP_RETALIATION) == CAST_OK)
                RetaliationTimer = 300000 + rand()%10000;
        }
        else
            RetaliationTimer -= uiDiff;

        if(ShatteringThrowTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SP_SHATTERING_THROW) == CAST_OK)
                ShatteringThrowTimer = 300000 + rand()%30000;
        }
        else
            ShatteringThrowTimer -= uiDiff;
        
        if(DisarmTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SP_DISARM) == CAST_OK)
                DisarmTimer = 60000 + rand()%6000;
        }
        else
            DisarmTimer -= uiDiff;
        
        boss_faction_championAI::UpdateAI(uiDiff);
    }
};

#define SP_Chains_of_Ice 66020 //8sec
#define SP_Death_Coil 66019    //5sec
#define H_SP_Death_Coil 67930
#define SP_Death_Grip  66017 //35sec
#define SP_Frost_Strike 66047  //6sec
#define H_SP_Frost_Strike 67936 
#define SP_Icebound_Fortitude 66023 //1min 
#define SP_Icy_Touch 66021  //8sec
#define H_SP_Icy_Touch 67939
#define SP_Strangulate 66018 //2min               

struct DIAMOND_DLL_DECL boss_fc_deathknightAI : public boss_faction_championAI
{
    boss_fc_deathknightAI(Creature *pCreature) : boss_faction_championAI(pCreature, AI_MELEE) {Init();}
    
    uint32 ChainsOfIceTimer;
    uint32 DeathCoilTimer;
    uint32 DeathGripTimer;
    uint32 FrostStrikeTimer;
    uint32 IceboundFortitudeTimer;
    uint32 IcyTouchTimer;
    uint32 StrangulateTimer;

    void Init()
    {
        ChainsOfIceTimer = 2000+rand()%3000;
        DeathCoilTimer = 3000+rand()%4000;
        DeathGripTimer = 1000+rand()%2000;
        FrostStrikeTimer = 4000+rand()%2000;
        IceboundFortitudeTimer = 20000;
        IcyTouchTimer = 6000+rand()%2000;
        StrangulateTimer = 6000+rand()%10000;
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        if(IceboundFortitudeTimer < uiDiff)
        {
            if(m_creature->GetHealth()*2 < m_creature->GetMaxHealth())
            {
                if (DoCastSpellIfCan(m_creature, SP_Icebound_Fortitude) == CAST_OK)
                    IceboundFortitudeTimer = 60000 + rand()%10000;
            }
            else
                IceboundFortitudeTimer = 5000;
        }
        else
            IceboundFortitudeTimer -= uiDiff;

        if(ChainsOfIceTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SP_Chains_of_Ice) == CAST_OK)
                ChainsOfIceTimer = 8000 + rand()%6000;
        }
        else
            ChainsOfIceTimer -= uiDiff;

        if(DeathCoilTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SP_Death_Coil : H_SP_Death_Coil) == CAST_OK)
                DeathCoilTimer = 6000 + rand()%4000;
        }
        else
            DeathCoilTimer -= uiDiff;

        if(StrangulateTimer < uiDiff)
        {
            if(Unit *target = SelectEnemyCaster(false))
                if (DoCastSpellIfCan(target, SP_Strangulate) == CAST_OK)
                    StrangulateTimer = 60000 + rand()%6000;
        }
        else
            StrangulateTimer -= uiDiff;

        if(FrostStrikeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SP_Frost_Strike : H_SP_Frost_Strike) == CAST_OK)
                FrostStrikeTimer = 5000 + rand()%5000;
        }
        else
            FrostStrikeTimer -= uiDiff;

        if(IcyTouchTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SP_Icy_Touch : H_SP_Icy_Touch) == CAST_OK)
                IcyTouchTimer = 8000 + rand()%4000;
        }
        else
            IcyTouchTimer -= uiDiff;

        if(DeathGripTimer < uiDiff)
        {
            if(m_creature->IsInRange(m_creature->getVictim(), 10.0f, 30.0f, false))
            {
                DoCast(m_creature->getVictim(), SP_Death_Grip);
                DeathGripTimer = 35000 + rand()%1000;
            }
            else
                DeathGripTimer = 3000;
        }
        else
            DeathGripTimer -= uiDiff;

        boss_faction_championAI::UpdateAI(uiDiff);
    }
};

#define SP_FAN_OF_KNIVES        52874 //correst is 65955 //2sec
#define SP_BLIND                43433 //correst is 65960 //2min
#define SP_CLOAK                65961 //90sec
#define SP_Blade_Flurry         65956 //2min
#define SP_SHADOWSTEP           66178 //30sec
#define SP_HEMORRHAGE            65897 //correct is65954
#define SP_EVISCERATE           71933 //correct is 65957
#define H_SP_EVISCERATE         71933 //correct is 68095                  

struct DIAMOND_DLL_DECL boss_fc_rogueAI : public boss_faction_championAI
{
    boss_fc_rogueAI(Creature *pCreature) : boss_faction_championAI(pCreature, AI_MELEE) {Init();}
    
    uint32 FanOfKnivesTimer;
    uint32 BlindTimer;
    uint32 CloakTimer;
    uint32 BladeFlurryTimer;
    uint32 ShadowstepTimer;
    uint32 HemorrhageTimer;
    uint32 EviscerateTimer;

    void Init()
    {
        FanOfKnivesTimer = 7000 + rand()%1000;
        BlindTimer = 6000 + rand()%3000;
        CloakTimer = 20000 + rand()%20000;
        BladeFlurryTimer = 10000 + rand()%1000;
        ShadowstepTimer = 9000 + rand()%2000;
        HemorrhageTimer = 5000 + rand()%1000;
        EviscerateTimer = 14000 + rand()%2000;
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(FanOfKnivesTimer < uiDiff)
        {
            if(EnemiesInRange(15.0f) > 2)
                if (DoCastSpellIfCan(m_creature->getVictim(), SP_FAN_OF_KNIVES) == CAST_OK)
                    FanOfKnivesTimer = 3000;
        }
        else
            FanOfKnivesTimer -= uiDiff;

        if(HemorrhageTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SP_HEMORRHAGE) == CAST_OK)
                HemorrhageTimer = 5000 + rand()%2000;
        }
        else
            HemorrhageTimer -= uiDiff;

        if(EviscerateTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SP_EVISCERATE : H_SP_EVISCERATE) == CAST_OK)
                EviscerateTimer = 7000 + rand()%3000;
        }
        else
            EviscerateTimer -= uiDiff;

        if(ShadowstepTimer < uiDiff)
        {
            if(m_creature->IsInRange(m_creature->getVictim(), 10.0f, 40.0f))
            {
                 if (DoCastSpellIfCan(m_creature->getVictim(), SP_SHADOWSTEP) == CAST_OK)
                    ShadowstepTimer = 30000 + rand()%6000;
            }
            else
                ShadowstepTimer = 2000;
        }
        else
            ShadowstepTimer -= uiDiff;

        if(BlindTimer < uiDiff)
        {
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,1))
            {
                if(m_creature->IsInRange(target, 0.0f, 15.0f, false))
                {
                    if (DoCastSpellIfCan(target, SP_BLIND) == CAST_OK)
                        BlindTimer = 120000 + rand()%6000;
                }
                else
                    BlindTimer = 2000;
            }
            else
                BlindTimer = 2000;
        }
        else
            BlindTimer -= uiDiff;

        if(CloakTimer < uiDiff) //If rogue really damaged...
        {
            if(m_creature->GetHealth()*2 < m_creature->GetMaxHealth())
            {
                if (DoCastSpellIfCan(m_creature, SP_CLOAK) == CAST_OK)  //...She will cloak.
                    CloakTimer = 90000;
            }
            else
                CloakTimer = 5000;
        }
        else
            CloakTimer -= uiDiff;

        if(BladeFlurryTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SP_Blade_Flurry) == CAST_OK)
                BladeFlurryTimer = 120000 + rand()%5000;
        }
        else
            BladeFlurryTimer -= uiDiff;

        boss_faction_championAI::UpdateAI(uiDiff);
    }
};

//#define SP_EARTH_SHOCK      65973
//#define H_SP_EARTH_SHOCK    68101
#define SP_LAVA_LASH        65974
#define SP_STORMSTRIKE      65970

struct DIAMOND_DLL_DECL boss_fc_enh_shammyAI : public boss_faction_championAI
{
    boss_fc_enh_shammyAI(Creature *pCreature) : boss_faction_championAI(pCreature, AI_MELEE) {Init();}
    
    uint32 HeroismTimer;
    uint32 ShockTimer;
    uint32 LavaLashTimer;
    uint32 StormstrikeTimer;

    void Init()
    {
        ShockTimer=3000+rand()%2000;       
        LavaLashTimer=4000+rand()%2000;
        StormstrikeTimer=5000+rand()%2000;
        HeroismTimer=5000+rand()%20000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(HeroismTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SP_HEROISM) == CAST_OK)
                HeroismTimer = 600000;
        }
        else
            HeroismTimer -= uiDiff;

        if(ShockTimer < uiDiff)
        {
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0))
                if (DoCastSpellIfCan(target, m_bIsRegularMode ? SP_EARTH_SHOCK : H_SP_EARTH_SHOCK) == CAST_OK)
                    ShockTimer = 6000 + rand()%1000;
        }
        else
            ShockTimer -= uiDiff;

        if(StormstrikeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SP_STORMSTRIKE) == CAST_OK)
                StormstrikeTimer = 8000 + rand()%4000;
        }
        else
            StormstrikeTimer -= uiDiff;

        if(LavaLashTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SP_LAVA_LASH) == CAST_OK)
                LavaLashTimer = 8000 + rand()%5000;
        }
        else
            LavaLashTimer -= uiDiff;

        boss_faction_championAI::UpdateAI(uiDiff);
    }
};

#define SP_Avenging_Wrath 66011 //3min cd
#define SP_Crusader_Strike 66003 //6sec cd
#define SP_Divine_Shield 66010 //5min cd
#define SP_Divine_Storm 66006 //10sec cd
#define SP_Hammer_of_Justice 66007 //40sec cd
#define SP_Hand_of_Protection 66009 //5min cd
#define SP_Judgement_of_Command 66005 //8sec cd
#define H_SP_Judgement_of_Command 68018
#define SP_REPENTANCE 66008 //60sec cd
#define SP_Seal_of_Command 66004 //no cd
#define H_SP_Seal_of_Command 68021

struct DIAMOND_DLL_DECL boss_fc_ret_pallyAI : public boss_faction_championAI
{
    boss_fc_ret_pallyAI(Creature *pCreature) : boss_faction_championAI(pCreature, AI_MELEE) {Init();}
    
    bool ShieldUsed;
    uint32 AvengingWrathTimer;
    uint32 CrusaderStrikeTimer;
    uint32 DivineShieldTimer;
    uint32 DivineStormTimer;
    uint32 HammerOfJusticeTimer;
    uint32 HandOfProtectionTimer;
    uint32 JudgementOfCommandTimer;
    uint32 RepentanceTimer;

    void Init()
    {
        AvengingWrathTimer = 7000 + rand()%3000;
        CrusaderStrikeTimer = 4000 + rand()%1000;
        DivineShieldTimer = 30000 + rand()%1000;
        DivineStormTimer = 5000 + rand()%2000;
        HammerOfJusticeTimer = 1000 + rand()%5000;
        HandOfProtectionTimer = 40000 + rand()%2000;
        JudgementOfCommandTimer = 6000;
        RepentanceTimer = 2000 + rand()%5000;
        ShieldUsed = false;
    }

    void Aggro(Unit *who)
    {
        boss_faction_championAI::Aggro(who);

        DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SP_Seal_of_Command : H_SP_Seal_of_Command);
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(RepentanceTimer < uiDiff)
        {
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,1))
                if (DoCastSpellIfCan(target, SP_REPENTANCE) == CAST_OK)
                    RepentanceTimer = 60000 + rand()%4000;
        }
        else
            RepentanceTimer -= uiDiff;

        if(CrusaderStrikeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SP_Crusader_Strike) == CAST_OK)
                CrusaderStrikeTimer = 6000 + rand()%1000;
        }
        else
            CrusaderStrikeTimer -= uiDiff;

        if(AvengingWrathTimer < uiDiff)  //Wrath+shield problem.
        {
            DoCast(m_creature, SP_Avenging_Wrath);
            AvengingWrathTimer = 300000 + rand()%5000;
        }
        else
            AvengingWrathTimer -= uiDiff;

        if(!ShieldUsed && m_creature->GetHealthPercent() < 5.0f)
        {
            DoCast(m_creature, SP_Divine_Shield);
            ShieldUsed = true;
        }
         
        if(DivineStormTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SP_Divine_Storm) == CAST_OK)
                DivineStormTimer = 10000 + rand()%1000;
        }
        else
            DivineStormTimer -= uiDiff;
        
        if(JudgementOfCommandTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SP_Judgement_of_Command : H_SP_Judgement_of_Command) == CAST_OK)
                JudgementOfCommandTimer = 8000 + rand()%1000;
        }
        else
            JudgementOfCommandTimer -= uiDiff;

        boss_faction_championAI::UpdateAI(uiDiff);
    }
};


/*========================================================*/
CreatureAI* GetAI_boss_fc_resto_druid(Creature *pCreature) {
    return new boss_fc_resto_druidAI (pCreature);
}
CreatureAI* GetAI_boss_fc_resto_shammy(Creature *pCreature) {
    return new boss_fc_resto_shammyAI (pCreature);
}
CreatureAI* GetAI_boss_fc_holy_pally(Creature *pCreature) {
    return new boss_fc_holy_pallyAI (pCreature);
}
CreatureAI* GetAI_boss_fc_holy_priest(Creature *pCreature) {
    return new boss_fc_holy_priestAI (pCreature);
}
CreatureAI* GetAI_boss_fc_shadow_priest(Creature *pCreature) {
    return new boss_fc_shadow_priestAI (pCreature);
}
CreatureAI* GetAI_boss_fc_warlock(Creature *pCreature) {
    return new boss_fc_warlockAI (pCreature);
}
CreatureAI* GetAI_boss_fc_mage(Creature *pCreature) {
    return new boss_fc_mageAI (pCreature);
}
CreatureAI* GetAI_boss_fc_hunter(Creature *pCreature) {
    return new boss_fc_hunterAI (pCreature);
}
CreatureAI* GetAI_boss_fc_boomkin(Creature *pCreature) {
    return new boss_fc_boomkinAI (pCreature);
}
CreatureAI* GetAI_boss_fc_warrior(Creature *pCreature) {
    return new boss_fc_warriorAI (pCreature);
}
CreatureAI* GetAI_boss_fc_deathknight(Creature *pCreature) {
    return new boss_fc_deathknightAI (pCreature);
}
CreatureAI* GetAI_boss_fc_rogue(Creature *pCreature) {
    return new boss_fc_rogueAI (pCreature);
}
CreatureAI* GetAI_boss_fc_enh_shammy(Creature *pCreature) {
    return new boss_fc_enh_shammyAI (pCreature);
}
CreatureAI* GetAI_boss_fc_ret_pally(Creature *pCreature) {
    return new boss_fc_ret_pallyAI (pCreature);
}

void AddSC_faction_champions()
{
    Script *newscript;
    
    newscript = new Script;
    newscript->Name = "boss_fc_resto_druid";
    newscript->GetAI = &GetAI_boss_fc_resto_druid;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_fc_resto_shammy";
    newscript->GetAI = &GetAI_boss_fc_resto_shammy;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_fc_holy_pally";
    newscript->GetAI = &GetAI_boss_fc_holy_pally;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_fc_holy_priest";
    newscript->GetAI = &GetAI_boss_fc_holy_priest;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_fc_shadow_priest";
    newscript->GetAI = &GetAI_boss_fc_shadow_priest;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_fc_mage";
    newscript->GetAI = &GetAI_boss_fc_mage;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_fc_warlock";
    newscript->GetAI = &GetAI_boss_fc_warlock;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_fc_hunter";
    newscript->GetAI = &GetAI_boss_fc_hunter;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_fc_boomkin";
    newscript->GetAI = &GetAI_boss_fc_boomkin;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_fc_warrior";
    newscript->GetAI = &GetAI_boss_fc_warrior;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_fc_deathknight";
    newscript->GetAI = &GetAI_boss_fc_deathknight;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_fc_rogue";
    newscript->GetAI = &GetAI_boss_fc_rogue;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_fc_enh_shammy";
    newscript->GetAI = &GetAI_boss_fc_enh_shammy;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_fc_ret_pally";
    newscript->GetAI = &GetAI_boss_fc_ret_pally;
    newscript->RegisterSelf();
}
