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

#define CR_DARK_ORB             34628
#define CR_LIGHT_ORB            34630

#define CR_DARK_ESSENCE         34567
#define CR_LIGHT_ESSENCE        34568

#define SP_DARK_ESSENCE         65684
#define SP_LIGHT_ESSENCE        65686

#define SP_UNLEASHED_DARK       67172
#define H_SP_UNLEASHED_DARK     67173
#define SP_UNLEASHED_LIGHT      65795
#define H_SP_UNLEASHED_LIGHT    67239

#define SP_POWER_OF_THE_TWINS   65916
#define SP_ENRAGE               64238

//Darkbane Spells
#define SP_SURGE_OF_DARKNESS    65768
#define H_SP_SURGE_OF_DARKNESS  67262
#define SP_SHIELD_OF_DARKNESS   65874 //175k damage
#define H_SP_SHIELD_OF_DARKNESS 67256 //700k damage
#define SP_DARK_VORTEX          66058
#define SP_TWIN_SPIKE_D         67309
#define H_SP_TWIN_SPIKE_D       67311
#define SP_TWINS_PACT_D         65875

//Lightbane Spells
#define SP_SURGE_OF_LIGHT       65766
#define H_SP_SURGE_OF_LIGHT     67270
#define SP_SHIELD_OF_LIGHTS     67259
#define H_SP_SHIELD_OF_LIGHTS   65858
#define SP_LIGHT_VORTEX         66046
#define SP_TWIN_SPIKE_L         67312
#define H_SP_TWIN_SPIKE_L       67314
#define SP_TWINS_PACT_L         65876

#define SAY_DARK_VORTEX     -1649026
#define SAY_LIGHT_VORTEX    -1649027
#define SAY_TWINS_PACT      -1649028

#define ROOM_RADIUS     55.0f

/**********************************************************
                    Fjola Lightbane
**********************************************************/

struct DIAMOND_DLL_DECL boss_fjolaAI : public ScriptedAI
{
    boss_fjolaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance *) pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    uint64 mDarkbaneGUID;
    uint32 OrbTimer;
    uint32 SpikeTimer;
    uint32 EnrageTimer;
    uint32 SpecialAbilityTimer;

    float summX, summY, summZ;

    void Reset()
    {
        OrbTimer = 5000;
        EnrageTimer = 360000; //6 minutes
        SpecialAbilityTimer = 120000;
        SpikeTimer = 15000;
        mDarkbaneGUID = 0;
    }

    void Aggro(Unit* pWho)
    {
        if(m_pInstance)
        {
            mDarkbaneGUID = m_pInstance->GetData64(TYPE_DARKBANE_GUID);
            if(Creature *eydis = m_pInstance->instance->GetCreature(mDarkbaneGUID))
                eydis->SetInCombatWithZone();
        }

        DoCast(m_creature, m_bIsRegularMode ? SP_SURGE_OF_LIGHT : H_SP_SURGE_OF_LIGHT);
    }

    void JustDied(Unit *killer)
    {
        if(m_pInstance) 
        {
            if(Creature *eydis = m_pInstance->instance->GetCreature(mDarkbaneGUID))
                if(eydis->isAlive())
                    eydis->DealDamage(eydis, eydis->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            m_pInstance->SetData(TYPE_TWIN_VALKYR, DONE);
        }
    }

    void JustReachedHome()
    {
        if(m_pInstance)
            m_pInstance->SetData(TYPE_TWIN_VALKYR, NOT_STARTED);
    }

    void DamageTaken(Unit *who, uint32 &dmg)
    {
        if(who->GetGUID() == m_creature->GetGUID())
            return;
 
        if(who->GetTypeId() == TYPEID_PLAYER)
        {
            if(who->HasAura(SP_LIGHT_ESSENCE))
                dmg /= 2;
            else if(who->HasAura(SP_DARK_ESSENCE))
                dmg += dmg/2;
        }

        if(m_pInstance)
        {
            if(Creature *eydis = m_pInstance->instance->GetCreature(mDarkbaneGUID))
                if(eydis->isAlive())
                    eydis->SetHealth(eydis->GetHealth() > dmg ? eydis->GetHealth()-dmg : 1);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_creature->getVictim()->GetGUID() == mDarkbaneGUID)
            EnterEvadeMode();

        if(OrbTimer < uiDiff)
        {
            float angle = rand_norm() * 2 * 3.1416;
            summX = ROOM_CENTER_X + ROOM_RADIUS * sinf(angle);
            summY = ROOM_CENTER_Y + ROOM_RADIUS * cosf(angle);
            summZ = 395.0f;
            m_creature->SummonCreature(CR_LIGHT_ORB, summX, summY, summZ, 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 30000);
            OrbTimer = 2500;
        }
        else
            OrbTimer -= uiDiff;
        
        if(SpecialAbilityTimer < uiDiff)
        {
            if(urand(0,1))
            {
                //Vortex
                DoCast(m_creature, SP_LIGHT_VORTEX);
                DoScriptText(SAY_LIGHT_VORTEX, m_creature);
            }
            else
            {
                //Twins Pact
                DoCast(m_creature, m_bIsRegularMode ? SP_SHIELD_OF_LIGHTS : H_SP_SHIELD_OF_LIGHTS, true);
                DoCast(m_creature, SP_TWINS_PACT_L);
                DoScriptText(SAY_TWINS_PACT, m_creature);
                if(Creature *eydis = m_pInstance->instance->GetCreature(mDarkbaneGUID))
                {
                    eydis->CastSpell(eydis, SP_POWER_OF_THE_TWINS, true);
                }
            }
            SpecialAbilityTimer = 120000;
        }
        else
            SpecialAbilityTimer -= uiDiff;

        if(SpikeTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SP_TWIN_SPIKE_L : H_SP_TWIN_SPIKE_L);
            SpikeTimer = 20000;
        }
        else
            SpikeTimer -= uiDiff;

        if(EnrageTimer < uiDiff)
        {
            DoCast(m_creature, SP_ENRAGE);
            EnrageTimer = 30000;
        }
        else
            EnrageTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_fjola(Creature* pCreature)
{
    return new boss_fjolaAI(pCreature);
}

/*######
## boss_eydis
######*/

struct DIAMOND_DLL_DECL boss_eydisAI : public ScriptedAI
{
    boss_eydisAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance *) pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    uint64 mLightbaneGUID;
    uint32 OrbTimer;
    uint32 SpikeTimer;
    uint32 EnrageTimer;
    uint32 SpecialAbilityTimer;

    float summX, summY, summZ;

    void Reset()
    {
        OrbTimer = 5000;
        EnrageTimer = 360000; //6 minutes
        SpecialAbilityTimer = 60000;
        SpikeTimer = 15000;
        mLightbaneGUID = 0;
    }
    
    void Aggro(Unit* pWho)
    {
        if(m_pInstance)
        {
            mLightbaneGUID = m_pInstance->GetData64(TYPE_LIGHTBANE_GUID);
            if(Creature *fjola = m_pInstance->instance->GetCreature(mLightbaneGUID))
                fjola->SetInCombatWithZone();
        }

        DoCast(m_creature, m_bIsRegularMode ? SP_SURGE_OF_DARKNESS : H_SP_SURGE_OF_DARKNESS);
    }

    void JustDied(Unit *killer)
    {
        if(m_pInstance)
        {
            if(Creature *fjola = m_pInstance->instance->GetCreature(mLightbaneGUID))
                if(fjola->isAlive())
                    fjola->DealDamage(fjola, fjola->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            m_pInstance->SetData(TYPE_TWIN_VALKYR, DONE);
        }
    }

    void JustReachedHome()
    {
        if(m_pInstance)
            m_pInstance->SetData(TYPE_TWIN_VALKYR, NOT_STARTED);
    }

    void DamageTaken(Unit *who, uint32 &dmg)
    {
        if(who->GetGUID() == m_creature->GetGUID()) return;
        
        if(who->GetTypeId() == TYPEID_PLAYER)
        {
            if(who->HasAura(SP_DARK_ESSENCE))
                dmg /= 2;
            else if(who->HasAura(SP_LIGHT_ESSENCE))
                dmg += dmg/2;
        }

        if(m_pInstance)
        {
            if(Creature *fjola = m_pInstance->instance->GetCreature(mLightbaneGUID))
                if(fjola->isAlive())
                    fjola->SetHealth(fjola->GetHealth() > dmg ? fjola->GetHealth()-dmg : 1);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_creature->getVictim()->GetGUID() == mLightbaneGUID) EnterEvadeMode();

        if(OrbTimer < uiDiff)
        {
            float angle = rand_norm() * 2 * 3.1416;
            summX = ROOM_CENTER_X + ROOM_RADIUS * sinf(angle);
            summY = ROOM_CENTER_Y + ROOM_RADIUS * cosf(angle);
            summZ = 395.0f;
            m_creature->SummonCreature(CR_DARK_ORB, summX, summY, summZ, 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 30000);
            OrbTimer = 2500;
        }
        else
            OrbTimer -= uiDiff;

        if(SpecialAbilityTimer < uiDiff)
        {
            if(urand(0,1))
            {
                //Vortex
                DoCast(m_creature, SP_DARK_VORTEX);
                DoScriptText(SAY_DARK_VORTEX, m_creature);
            }
            else
            {
                //Twins Pact
                DoCast(m_creature, m_bIsRegularMode ? SP_SHIELD_OF_DARKNESS, true : H_SP_SHIELD_OF_DARKNESS);
                DoCast(m_creature, SP_TWINS_PACT_D);
                DoScriptText(SAY_TWINS_PACT, m_creature);
                if(Creature *fjola = m_pInstance->instance->GetCreature(mLightbaneGUID))
                {
                    fjola->CastSpell(fjola, SP_POWER_OF_THE_TWINS, true);
                }
            }
            SpecialAbilityTimer = 120000;
        }
        else
            SpecialAbilityTimer -= uiDiff;

        if(SpikeTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SP_TWIN_SPIKE_D : H_SP_TWIN_SPIKE_D);
            SpikeTimer = 20000;
        }
        else
            SpikeTimer -= uiDiff;

        if(EnrageTimer < uiDiff)
        {
            DoCast(m_creature, SP_ENRAGE);
            EnrageTimer = 30000;
        }
        else
            EnrageTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_eydis(Creature* pCreature)
{
    return new boss_eydisAI(pCreature);
}

// Concentrated Light/Darkness
struct DIAMOND_DLL_DECL mob_valkyr_orbAI : public ScriptedAI
{
    mob_valkyr_orbAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();

        switch(pCreature->GetEntry())
        {
            case CR_DARK_ORB: spell = m_bIsRegularMode ? SP_UNLEASHED_DARK : H_SP_UNLEASHED_DARK; break;
            case CR_LIGHT_ORB: spell = m_bIsRegularMode ? SP_UNLEASHED_LIGHT : H_SP_UNLEASHED_LIGHT; break;
        }
        SetCombatMovement(false);    
        ChooseDirection();
    }

    uint32 spell;
    bool m_bIsRegularMode;
    float dstX, dstY, dstZ;

    void Reset()
    {
    }

    void AttackStart(Unit *pWho)
    {
        if(!pWho)
            return;

        //if(m_creature->Attack(pWho, false))
        if(m_creature->GetDistance2d(pWho) <= 3.0f)
        {
            m_creature->SetLevel(83);
            m_creature->SetInCombatWithZone();
            m_creature->CastSpell(pWho, spell, true);
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }

    void ChooseDirection()
    {
        float angle = rand_norm() * 2.0f * 3.1416f;
        dstX = ROOM_CENTER_X + ROOM_RADIUS * sinf(angle);
        dstY = ROOM_CENTER_Y + ROOM_RADIUS * cosf(angle);
        dstZ = 394.5f;

        m_creature->GetMotionMaster()->MovePoint(0, dstX, dstY, dstZ);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(abs(m_creature->GetPositionX() - dstX) < 2.0f && abs(m_creature->GetPositionY() - dstY) < 2.0f)
        {
            m_creature->GetMotionMaster()->MovementExpired();
            ChooseDirection();
        }
    }
};

CreatureAI* GetAI_mob_valkyr_orb(Creature *pCreature)
{
    return new mob_valkyr_orbAI(pCreature);
}

void AddSC_twin_valkyr()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_fjola";
    newscript->GetAI = &GetAI_boss_fjola;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_eydis";
    newscript->GetAI = &GetAI_boss_eydis;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_valkyr_orb";
    newscript->GetAI = &GetAI_mob_valkyr_orb;
    newscript->RegisterSelf();
}
