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
SDName:
SD%Complete: 0
SDComment:
SDCategory:
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

/**********************************************************
                    GORMOK THE IMPALER
**********************************************************/

#define SP_IMPALE       66331
#define H_SP_IMPALE     67477
#define SP_STOMP        66330
#define H_SP_STOMP      67649
#define SP_RISING_ANGER 66636
#define SP_FIRE_BOMB    66318

#define CR_SNOBOLD     34800
#define CR_FIRE_BOMB   34854

struct DIAMOND_DLL_DECL boss_gormokAI : public ScriptedAI
{
    boss_gormokAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    uint32 SnoboldsLaunched;
    uint32 MaxSnobolds;
    uint32 SnoboldTimer;
    uint32 ImpaleTimer;
    uint32 StompTimer;
    uint32 FireBombTimer;

    void Reset()
    {
        SnoboldsLaunched = 0;
        MaxSnobolds = m_bIsRegularMode ? 4 : 5;
        SnoboldTimer = urand(15000, 30000);
        ImpaleTimer = 10000;
        StompTimer = urand(20000, 25000);
        FireBombTimer = 10000 + rand()%8000;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NORTHREND_BEASTS, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(DATA_BEASTS_ENRAGE, 900000);
    }

    void JustDied(Unit *killer)
    {
        if (m_pInstance)
            m_pInstance->SetData(DATA_GORMOK, DONE);
    }

    void JustSummoned(Unit *snobold)
    {
        if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
        {
            snobold->AddThreat(target, 1000000.0f);
            snobold->NearTeleportTo(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(ImpaleTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SP_IMPALE : H_SP_IMPALE);
            ImpaleTimer = 10000;
        }
        else
            ImpaleTimer -= uiDiff;

        if(StompTimer < uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SP_STOMP : H_SP_STOMP);
            StompTimer = urand(20000, 25000);
        }
        else
            StompTimer -= uiDiff;

        if(FireBombTimer < uiDiff)
        {
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
            {
                if(Creature *bomb = m_creature->SummonCreature(CR_FIRE_BOMB, 
                    target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, 
                    TEMPSUMMON_TIMED_DESPAWN, 62000))
                {
                    bomb->CastSpell(bomb, SP_FIRE_BOMB, false);
                    bomb->addUnitState(UNIT_STAT_ROOT);
                    bomb->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    bomb->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
            }
            FireBombTimer = 10000 + rand()%8000;
        }
        else
            FireBombTimer -= uiDiff;

        if((SnoboldsLaunched < MaxSnobolds) && (SnoboldTimer < uiDiff))
        {
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
            {
                if(Creature *snobold = m_creature->SummonCreature(CR_SNOBOLD, 
                    target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, 
                    TEMPSUMMON_DEAD_DESPAWN, 3000))
                    snobold->AddThreat(target, 1000000.0f);
                ++SnoboldsLaunched;
                DoCast(m_creature, SP_RISING_ANGER, true);
            }
            SnoboldTimer = 30000 + rand()%15000;
        }
        else
            SnoboldTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_gormok(Creature* pCreature)
{
    return new boss_gormokAI(pCreature);
}

/**********************************************************
                   ACIDMAW & DREADSCALE
**********************************************************/

#define SP_ACIDIC_SPEW      66818
#define SP_PARALYTIC_BITE   66824
#define H_SP_PARALYTIC_BITE 67612
#define SP_SLIME_POOL       66883
#define SP_ACID_SPIT        66880
#define H_SP_ACID_SPIT      67606
#define SP_SWEEP            66794
#define SP_PARALYTIC_SPRAY  66901

struct DIAMOND_DLL_DECL boss_acidmawAI : public ScriptedAI
{
    boss_acidmawAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    bool mobile;
    uint32 MobileTimer;

    uint32 SweepTimer;
    uint32 AcidSpitTimer;
    uint32 ParalyticSprayTimer;
    uint32 BiteTimer;
    uint32 SpewTimer;

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NORTHREND_BEASTS, NOT_STARTED);
    }

    void Reset()
    {
        MobileTimer = 60000;
        SweepTimer = 25000 + rand()%15000;
        AcidSpitTimer = 4000 + rand()%4000;
        ParalyticSprayTimer = 15000 + rand()%15000;
        BiteTimer = 20000 + rand()%10000;
        SpewTimer = 15000 + rand()%10000;
        mobile = false;
        SetCombatMovement(false);
    }

    void Aggro(Unit* pWho)
    {
    }
 
    void JustDied(Unit *killer)
    {
        if (m_pInstance)
            m_pInstance->SetData(DATA_ACIDMAW, DEAD);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(MobileTimer < uiDiff)
        {
            mobile = !mobile;
            SetCombatMovement(mobile);
            MobileTimer = 60000;
            if(mobile)
            {
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
            }
            else
            {
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveIdle();
            }
        }
        else
            MobileTimer -= uiDiff;

        if(mobile)
        {
            if(BiteTimer < uiDiff)
            {
                DoCast(m_creature->getVictim(), m_bIsRegularMode ? SP_PARALYTIC_BITE : H_SP_PARALYTIC_BITE);
                BiteTimer = 20000 + rand()%10000;
            }
            else
                BiteTimer -= uiDiff;

            if(SpewTimer < uiDiff)
            {
                DoCast(m_creature->getVictim(), SP_ACIDIC_SPEW);
                SpewTimer = 25000 + rand()%15000;
            }
            else
                 SpewTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
        else
        {
            if(ParalyticSprayTimer < uiDiff)
            {
                if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
                {
                    DoCast(target, SP_PARALYTIC_SPRAY, true);
                    DoCast(target, 66823, true);
                }
                ParalyticSprayTimer = m_bIsRegularMode ? 25000 + rand()%10000 : 15000 + rand()%5000;
            }
            else
                ParalyticSprayTimer -= uiDiff;

            if(SweepTimer < uiDiff)
            {
                DoCast(m_creature, SP_SWEEP);
                SweepTimer = 25000 + rand()%15000;
            }
            else
                SweepTimer -= uiDiff;

            if(AcidSpitTimer < uiDiff)
            {
                if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(target, m_bIsRegularMode ? SP_ACID_SPIT : H_SP_ACID_SPIT);
                AcidSpitTimer = m_bIsRegularMode ? 3000 + rand()%3000 : 2000 + rand()%2000;
            }
            else
                AcidSpitTimer -= uiDiff;
        } 
    }
};

CreatureAI* GetAI_boss_acidmaw(Creature* pCreature)
{
    return new boss_acidmawAI(pCreature);
}

#define SP_BURNING_BITE     66879
#define H_SP_BURNING_BITE   67624
#define SP_MOLTEN_SPEW      66821
#define SP_FIRE_SPIT        66796
#define H_SP_FIRE_SPIT      67632
#define SP_BURNING_SPRAY    66902

struct DIAMOND_DLL_DECL boss_dreadscaleAI : public ScriptedAI
{
    boss_dreadscaleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    bool mobile;
    uint32 MobileTimer;
    uint32 SweepTimer;
    uint32 FireSpitTimer;
    uint32 BiteTimer;
    uint32 SpewTimer;
    uint32 BurningSprayTimer;

    void Reset()
    {
        MobileTimer = 60000;
        mobile = true;
        SweepTimer = 25000 + rand()%15000;
        FireSpitTimer = 4000 + rand()%4000;
        BiteTimer = 20000 + rand()%10000;
        SpewTimer = 15000 + rand()%10000;
        BurningSprayTimer = 15000 + rand()%15000;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NORTHREND_BEASTS, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
    }

    void JustDied(Unit *killer)
    {
        if (m_pInstance)
            m_pInstance->SetData(DATA_DREADSCALE, DEAD);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(MobileTimer < uiDiff)
        {
            mobile = !mobile;
            SetCombatMovement(mobile);
            MobileTimer = 60000;
            if(mobile)
            {
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
            }
            else
            {
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveIdle();
            }
        }
        else
            MobileTimer -= uiDiff;

        if(mobile)
        {
            if(BiteTimer < uiDiff)
            {
                DoCast(m_creature->getVictim(), m_bIsRegularMode ? SP_BURNING_BITE : H_SP_BURNING_BITE);
                BiteTimer = 20000 + rand()%10000;
            }
            else
                BiteTimer -= uiDiff;

            if(SpewTimer < uiDiff)
            {
                DoCast(m_creature->getVictim(), SP_MOLTEN_SPEW);
                SpewTimer = 25000 + rand()%15000;
            }
            else
                SpewTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
        else
        {
            if(BurningSprayTimer < uiDiff)
            {
                if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
                {
                    DoCast(target, SP_BURNING_SPRAY, true);
                    DoCast(target, 66869, true);
                }
                BurningSprayTimer = m_bIsRegularMode ? 25000 + rand()%10000 : 15000 + rand()%5000;
            }
            else
                BurningSprayTimer -= uiDiff;

            if(SweepTimer < uiDiff)
            {
                DoCast(m_creature, SP_SWEEP);
                SweepTimer = 25000 + rand()%15000;
            }
            else
                SweepTimer -= uiDiff;

            if(FireSpitTimer < uiDiff)
            {
                if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(target, m_bIsRegularMode ? SP_FIRE_SPIT : H_SP_FIRE_SPIT);
                FireSpitTimer = m_bIsRegularMode ? 3000 + rand()%3000 : 2000 + rand()%2000;
            }
            else
                FireSpitTimer -= uiDiff; 
        } 
    }
};

CreatureAI* GetAI_boss_dreadscale(Creature* pCreature)
{
    return new boss_dreadscaleAI(pCreature);
}

/**********************************************************
                          ICEHOWL
**********************************************************/

#define SP_ENRAGE           26662
#define SP_FEROCIOUS_BUTT   66770
#define H_SP_FEROCIOUS_BUTT 67655
#define SP_WHIRL            67345
#define H_SP_WHIRL          67663
#define SP_MASSIVE_CRASH    66683
#define H_SP_MASSIVE_CRASH  67660
#define SP_ARCTIC_BREATH    66689
#define H_SP_ARCTIC_BREATH  67650

struct DIAMOND_DLL_DECL boss_icehowlAI : public ScriptedAI
{
    boss_icehowlAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    uint32 EnrageTimer;
    uint32 CrashTimer;
    uint32 BreathTimer;
    uint32 WhirlTimer;

    void Reset()
    {
        if (m_pInstance)
            EnrageTimer = m_pInstance->GetData(DATA_BEASTS_ENRAGE);
        else 
            EnrageTimer = 300000;

        CrashTimer = 45000;
        BreathTimer = 25000;
        WhirlTimer = 15000;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NORTHREND_BEASTS, NOT_STARTED);
    }

    void JustDied(Unit *killer)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NORTHREND_BEASTS, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        if(EnrageTimer < uiDiff)
        {
            DoCast(m_creature, SP_ENRAGE);
            EnrageTimer = 30000;
        }
        else
            EnrageTimer -= uiDiff;

        if(CrashTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SP_MASSIVE_CRASH : H_SP_MASSIVE_CRASH);
            CrashTimer = 45000;
        }
        else
            CrashTimer -= uiDiff;

        if(BreathTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SP_ARCTIC_BREATH : H_SP_ARCTIC_BREATH);
            BreathTimer = 45000;
        }
        else
            BreathTimer -= uiDiff;

        if(WhirlTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SP_WHIRL : H_SP_WHIRL);
            WhirlTimer = 45000;
        }
        else
            WhirlTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_icehowl(Creature* pCreature)
{
    return new boss_icehowlAI(pCreature);
}

void AddSC_northrend_beasts()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_gormok";
    newscript->GetAI = &GetAI_boss_gormok;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_acidmaw";
    newscript->GetAI = &GetAI_boss_acidmaw;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_dreadscale";
    newscript->GetAI = &GetAI_boss_dreadscale;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_icehowl";
    newscript->GetAI = &GetAI_boss_icehowl;
    newscript->RegisterSelf();

}
