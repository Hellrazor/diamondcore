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
SDName: boss_anubarak_trial
SD%Complete: 0
SDComment:
SDCategory:
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

#define SP_FREEZING_SLASH       66012

#define SP_LEECHING_SWARM       67630
#define SP_LEECHING_SWARM_DMG   66240
#define SP_LEECHING_SWARM_HEAL  66125
#define SP_ENRAGE               26662

#define SP_PENETRATING_COLD     66013
#define H_SP_PENETRATING_COLD   67700

#define CR_BURROWER             34607
#define CR_SCARAB               34605

static float SpawnLocs[2][4] =
{
    {710.64f,  76.66f, 142.13f, 0.8f},
    {695.05f, 191.45f, 142.12f, 4.9f}
};

struct DIAMOND_DLL_DECL boss_anubarak_trialAI : public ScriptedAI
{
    boss_anubarak_trialAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    int phase; //1 - ground, 2 - underground, 3 - swarm
    uint32 EnrageTimer;
    uint32 PhaseTimer;
    uint32 SummonTimerP1;
    uint32 SummonTimerP2;

    uint32 PenetratingColdTimer;
    uint32 FreezingSlashTimer;
    
    uint32 SwarmTickTimer;
    int32 SwarmDamage;
    int32 SwarmDamageTotal;
    
    float SpawnX[2];
    float SpawnY[2];

    void Reset()
    {
        phase = 1;
        PhaseTimer = 90000;
        EnrageTimer = 600000; //10 minutes
        SummonTimerP1 = 10000;
        SummonTimerP2 = 5000;
        PenetratingColdTimer = 10000;
        FreezingSlashTimer = 20000 + rand()%50000;
        SwarmTickTimer = 3000;
    }

    void JustReachedHome()
    {
        m_creature->SetVisibility(VISIBILITY_ON);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANUBARAK, NOT_STARTED);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANUBARAK, DONE);
    }

    void Aggro(Unit* pWho)
    {
        m_creature->SetInCombatWithZone();
        if(m_pInstance)
            m_pInstance->SetData(TYPE_ANUBARAK, IN_PROGRESS);
    }

    void JustSummoned(Creature *mob)
    {
        mob->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(phase==3)
        {
            if(PenetratingColdTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SP_PENETRATING_COLD : H_SP_PENETRATING_COLD) == CAST_OK)
                    PenetratingColdTimer = 25000;
            }
            else
                PenetratingColdTimer -= uiDiff;

            //leeching swarm, no adds and other spells
            if(SwarmTickTimer < uiDiff)
            {
                SwarmDamageTotal = 0;
                ThreatList const& tList = m_creature->getThreatManager().getThreatList();
                for(ThreatList::const_iterator i = tList.begin(); i!=tList.end(); ++i)
                {
                    Unit *plr = Unit::GetUnit((*m_creature),(*i)->getUnitGuid());
                    if(plr && plr->GetTypeId()==TYPEID_PLAYER && plr->isAlive())
                    {
                        SwarmDamage = plr->GetHealth() / 10;
                        if(SwarmDamage < 250) SwarmDamage = 250;
                        SwarmDamageTotal += SwarmDamage;
                        m_creature->CastCustomSpell(plr, SP_LEECHING_SWARM_DMG, &SwarmDamage, NULL, NULL, true);
                    }
                }
                m_creature->CastCustomSpell(m_creature, SP_LEECHING_SWARM_HEAL, &SwarmDamageTotal, NULL, NULL, true);
                
                SwarmTickTimer = 1000;
            }
            else
                SwarmTickTimer -= uiDiff;
        };

        if(phase==2)
        {
            //lots of adds, boss inactive (underground)
            if(SummonTimerP2 < uiDiff)
            {
                //summon small add
                int loc = irand(0,1);
                m_creature->SummonCreature(CR_SCARAB, SpawnLocs[loc][0], SpawnLocs[loc][1], SpawnLocs[loc][2], SpawnLocs[loc][3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
                SummonTimerP2 = 5000;
            }
            else
                SummonTimerP2 -= uiDiff;
        };

        if(phase==1)
        {
            //boss abilities, some adds
            if(PenetratingColdTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SP_PENETRATING_COLD : H_SP_PENETRATING_COLD) == CAST_OK)
                    PenetratingColdTimer = 25000;
            }
            else
                PenetratingColdTimer -= uiDiff;

            if(FreezingSlashTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SP_FREEZING_SLASH) == CAST_OK)
                    FreezingSlashTimer = 20000 + rand()%5000;
            }
            else
                FreezingSlashTimer -= uiDiff;

            if(SummonTimerP1 < uiDiff)
            {
                //summon 2 (heroic 4) big adds in 2 points
                m_creature->SummonCreature(CR_BURROWER, SpawnLocs[0][0], SpawnLocs[0][1], SpawnLocs[0][2], SpawnLocs[0][3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
                if(!m_bIsRegularMode)
                    m_creature->SummonCreature(CR_BURROWER, SpawnLocs[0][0], SpawnLocs[0][1], SpawnLocs[0][2], SpawnLocs[0][3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
                m_creature->SummonCreature(CR_BURROWER, SpawnLocs[1][0], SpawnLocs[1][1], SpawnLocs[1][2], SpawnLocs[1][3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
                if(!m_bIsRegularMode)
                    m_creature->SummonCreature(CR_BURROWER, SpawnLocs[1][0], SpawnLocs[1][1], SpawnLocs[1][2], SpawnLocs[1][3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
                SummonTimerP1 = 60000;
            }
            else
                SummonTimerP1 -= uiDiff;
        };
            
        if(phase!=3 && PhaseTimer < uiDiff)
        {
            if(phase==1)
            {
                //transition to phase 2
                m_creature->SetVisibility(VISIBILITY_OFF);
                
                phase = 2;
                PhaseTimer = 60000;
                SummonTimerP2 = 10000;
            }
            else //if(phase==2)
            {
                //transition to phase 1
                m_creature->SetVisibility(VISIBILITY_ON);
                               
                phase = 1;
                PhaseTimer = 90000;
                SummonTimerP1 = 10000;
            };
        }
        else
            PhaseTimer -= uiDiff;
        
        if(phase!=3 && (m_creature->GetHealthPercent() < 30.0f))
        {
            //transition to phase 3
            m_creature->SetVisibility(VISIBILITY_ON);
            //DoCast(m_creature, SP_LEECHING_SWARM);
            phase = 3;
        };
        
        if(EnrageTimer < uiDiff)
        {
            DoCast(m_creature, SP_ENRAGE);
            EnrageTimer = 30000;
        }
        else
            EnrageTimer -= uiDiff;

        if(phase!=2)
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_anubarak_trial(Creature* pCreature)
{
    return new boss_anubarak_trialAI(pCreature);
}

void AddSC_boss_anubarak_trial()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_anubarak_trial";
    newscript->GetAI = &GetAI_boss_anubarak_trial;
    newscript->RegisterSelf();
}
