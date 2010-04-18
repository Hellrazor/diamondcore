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
SDName: trial_of_the_crusader
SD%Complete: 0
SDComment:
SDCategory: Crusader Coliseum
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

#define SP_FEL_FIREBALL     66532
#define H_SP_FEL_FIREBALL   66963
#define SP_LEGION_FLAME     66197
#define H_SP_LEGION_FLAME   68123
#define SP_FEL_LIGHTNING    66528
#define H_SP_FEL_LIGHTNING  67029
#define SP_NETHER_POWER     67108

#define SP_NETHER_PORTAL    66263

#define SP_ENRAGE       64238

#define CR_MISTRESS  34826
#define CR_INFERNAL  34815

class DIAMOND_DLL_DECL NetherPowerAura : public Aura
{
    public:
        NetherPowerAura(const SpellEntry *spell, SpellEffectIndex eff, int32 *bp, Unit *target, Unit *caster) : Aura(spell, eff, bp, target, caster, NULL) {}
};

struct DIAMOND_DLL_DECL boss_jaraxxusAI : public ScriptedAI
{
    boss_jaraxxusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    
    uint32 SummonTimer;
    bool SumTypeMistress;
    uint32 NetherPowerTimer;
    uint32 EnrageTimer;
    
    uint32 FlamesTimer;
    uint32 FireballTimer;
    uint32 LightningTimer;
    uint32 InfernoTimer;

    float VolcanoX[3];
    float VolcanoY[3];
    uint32 SummonTimer2;

    void Reset()
    {
        SumTypeMistress = true;
        SummonTimer = 15000;
        NetherPowerTimer = 40000;
        EnrageTimer = 600000; //10 minutes
        FlamesTimer = 30000;
        LightningTimer = urand(10000, 15000);
        FireballTimer = urand(15000, 25000);
        InfernoTimer = urand(40000, 50000);

        SummonTimer2 = -1;
    }

    void JustReachedHome()
    {
        DespawnMistress();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_JARAXXUS, NOT_STARTED);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_JARAXXUS, DONE);
    }

    void Aggro(Unit* pWho)
    {
    }

    void SummonInfernals()
    {
        //34813 - infernal volcano

        VolcanoX[0] = 564.04f + rand_norm()*30.0f - 15.0f;
        VolcanoY[0] = 139.98f + rand_norm()*30.0f - 15.0f;
        m_creature->SummonCreature(34813, VolcanoX[0], VolcanoY[0], 394.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10000);
        VolcanoX[1] = 564.04f + rand_norm()*30.0f - 15.0f;
        VolcanoY[1] = 139.98f + rand_norm()*30.0f - 15.0f;
        m_creature->SummonCreature(34813, VolcanoX[1], VolcanoY[1], 394.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10000);
        VolcanoX[2] = 564.04f + rand_norm()*30.0f - 15.0f;
        VolcanoY[2] = 139.98f + rand_norm()*30.0f - 15.0f;
        m_creature->SummonCreature(34813, VolcanoX[2], VolcanoY[2], 394.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10000);
    }

    void SummonMistress()
    {
        //summon portal 34825
        float px = 564.04f + rand_norm()*30.0f - 15.0f;
        float py = 139.98f + rand_norm()*30.0f - 15.0f;
        if(Creature *portal = m_creature->SummonCreature(34825, px, py, 394.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 20000))
        {
            portal->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            portal->CastSpell(portal, SP_NETHER_PORTAL, true);
        }
    }

    void PutMistressInCombat()
    {
        if(Creature *mistress = GetClosestCreatureWithEntry(m_creature, CR_MISTRESS, 80.0f))
            mistress->SetInCombatWithZone();
    }

    void DespawnMistress()
    {
        std::list<Creature *> lCreatureList;
        GetCreatureListWithEntryInGrid(lCreatureList, m_creature, CR_MISTRESS, 80.0f);

        std::list<Creature* >::iterator iter = lCreatureList.begin();
        for(iter = lCreatureList.begin(); iter != lCreatureList.end(); ++iter)
        {
            (*iter)->ForcedDespawn();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(EnrageTimer < uiDiff)
        {
            DoCast(m_creature, SP_ENRAGE, true);
            EnrageTimer = 30000;
        }
        else
            EnrageTimer -= uiDiff;
        
        if(FireballTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SP_FEL_FIREBALL : H_SP_FEL_FIREBALL);
            FireballTimer = urand(12000, 20000);
        }
        else
            FireballTimer -= uiDiff;

        if(NetherPowerTimer < uiDiff)
        {
            SpellEntry *spell = (SpellEntry *)GetSpellStore()->LookupEntry(SP_NETHER_POWER);
            if(m_creature->AddAura(new NetherPowerAura(spell, EFFECT_INDEX_0, NULL, m_creature, m_creature)))
                m_creature->GetAura(SP_NETHER_POWER, EFFECT_INDEX_0)->SetStackAmount(m_bIsRegularMode ? 5 : 10);
            NetherPowerTimer = 40000;
        }
        else
            NetherPowerTimer -= uiDiff;
        
        if(LightningTimer < uiDiff)
        {
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(target, m_bIsRegularMode ? SP_FEL_LIGHTNING : H_SP_FEL_LIGHTNING);
            LightningTimer = urand(6000, 12000);
        }
        else
            LightningTimer -= uiDiff;

        if(SummonTimer < uiDiff)
        {
            if(SumTypeMistress)
            {
                SummonMistress();
                SummonTimer2 = 9000;
            }
            else
            {
                SummonInfernals();
                SummonTimer2 = 8000;
            }
            SummonTimer = 60000;
        }
        else
            SummonTimer -= uiDiff;

        if(SummonTimer2 > 0)
        {
            if(SummonTimer2 < uiDiff)
            {
                if(SumTypeMistress)
                {
                    PutMistressInCombat();
                    SumTypeMistress = false;
                }
                else
                {
                    uint8 i;
                    for(i=0; i<3; ++i)
                    {
                        if(Creature *infernal = m_creature->SummonCreature(CR_INFERNAL, VolcanoX[i], VolcanoY[i], 394.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 12000))
                            infernal->SetInCombatWithZone();
                    }
                    SumTypeMistress = true;
                }
                SummonTimer2 = -1;
            }
            else
                SummonTimer2 -= uiDiff;
        }

        if(InfernoTimer < uiDiff)
        {
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(target, 66242);
            InfernoTimer = urand(60000, 85000);
        }
        else
            InfernoTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_jaraxxus(Creature* pCreature)
{
    return new boss_jaraxxusAI(pCreature);
}

void AddSC_boss_jaraxxus()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_jaraxxus";
    newscript->GetAI = &GetAI_boss_jaraxxus;
    newscript->RegisterSelf();
}
