#include "precompiled.h"
#include "vault_of_archavon.h"

#define SP_BURNING_FURY_AURA    66895
#define SP_BURNING_FURY_AURA2   68168
#define SP_BURNING_FURY_EFFECT  66721

#define SP_BURNING_BREATH       66665
#define H_SP_BURNING_BREATH     67328 //DBM
#define SP_BB_EFFECT            66670
#define H_SP_BB_EFFECT          67329

#define SP_METEOR_FISTS         66725 //DBM
#define H_SP_METEOR_FISTS       68161       
#define SP_METEOR_FISTS_EFF     66765
#define H_SP_METEOR_FISTS_EFF   67333

#define SP_CINDER       66684
#define H_SP_CINDER     67332

struct DIAMOND_DLL_DECL boss_koralonAI : public ScriptedAI
{
    boss_koralonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Regular = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* pInstance;
    bool Regular;
    uint32 m_uiEvadeCheckCooldown;

    uint32 BurningBreathTimer;
    uint32 MeteorFistsTimer;
    uint32 FlamesTimer;

    uint32 BBTickTimer;
    uint32 BBTicks;
    bool BB;

    void Reset()
    {
        m_uiEvadeCheckCooldown = 2000;
        BurningBreathTimer = 25000;
        MeteorFistsTimer = 47000;
        FlamesTimer = 15000;

        BB = false;

        if(pInstance) pInstance->SetData(TYPE_KORALON, NOT_STARTED);
    }

    void Aggro(Unit *who)
    {
        DoCast(m_creature, SP_BURNING_FURY_AURA);

        if(pInstance) pInstance->SetData(TYPE_KORALON, IN_PROGRESS);
    };

    void JustDied(Unit *killer)
    {
        if(pInstance) pInstance->SetData(TYPE_KORALON, DONE);
    };

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiEvadeCheckCooldown < diff)
        {
            if (m_creature->GetDistance2d(-218.95f, 103.41f) > 80.0f)
                EnterEvadeMode();
            m_uiEvadeCheckCooldown = 2000;
        }
        else
            m_uiEvadeCheckCooldown -= diff;

        if(BurningBreathTimer < diff)
        {
            DoCast(m_creature, Regular ? SP_BURNING_BREATH : H_SP_BURNING_BREATH);
            BurningBreathTimer = 45000;

            BB = true;
            BBTickTimer = 1000;
            BBTicks = 0;
        }
        else BurningBreathTimer -= diff;

        if(BB)
        {
            if(BBTickTimer < diff)
            {
                DoCast(NULL, Regular ? SP_BB_EFFECT : H_SP_BB_EFFECT, true);
                BBTickTimer = 1000;
                ++BBTicks;
                if(BBTicks > 2) BB = false;
            }
            else BBTickTimer -= diff;
        }

        if(FlamesTimer < diff)
        {
            int flames = Regular ? 3 : 5;
            int i;
            for(i=0; i< flames; ++i)
            {
                Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0);
                if(target) DoCast(target, Regular ? SP_CINDER : H_SP_CINDER);
            }
            FlamesTimer = 20000;
        }
        else FlamesTimer -= diff;

        if(MeteorFistsTimer < diff)
        {
            DoCast(m_creature->getVictim(), SP_METEOR_FISTS_EFF);
            MeteorFistsTimer = 45000;
        }
        else MeteorFistsTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_koralonAI(Creature* pCreature)
{
    return new boss_koralonAI(pCreature);
}

void AddSC_boss_koralon()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_koralon";
    newscript->GetAI = &GetAI_boss_koralonAI;
    newscript->RegisterSelf();
}
