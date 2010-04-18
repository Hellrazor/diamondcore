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
SDName: instance_trial_of_the_crusader
SD%Complete: 100
SDComment:
SDCategory: Crusader Coliseum
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

/* Trial Of The Crusader encounters:
0 - Northrend Beasts
1 - Jaraxxus
2 - Faction Champions
3 - Twin Valkyr
4 - Anubarak
*/

#define SAY_GORMOK_INTRO        -1649000
#define SAY_JORMUNGAR_INTRO     -1649001
#define SAY_ICEHOWL_INTRO       -1649002
#define SAY_ICEHOWL_SLAIN       -1649003

static uint32 ChampHealers[2][4] =
{
    //Druid, Pala, Priest, Shammy
    {34469, 34465, 34466, 34470}, //Alliance
    {34459, 34445, 34447, 34444}  //Horde
};

static uint32 ChampRanged[2][5] =
{
    //Boomkin, Hunter, Mage, SPriest, Warlock
    {34460, 34467, 34468, 34473, 34474}, //Alliance
    {34451, 34448, 34449, 34441, 34450}  //Horde
};

static uint32 ChampMelee[2][5] =
{
    //DK, RetPala, Rogue, Enh.Shammy, Warrior
    {34461, 34471, 34472, 34463, 34475}, //Alliance
    {34458, 34456, 34454, 34455, 34453}  //Horde
};

/* Champions Coordinates
   r h h h r   0 4 5 6 2
   r m m m r   1 7 8 9 3
*/
static float ChampSpawnLocs[2][10][4] =
{
    {
        //Alliance (North Side)
        {610.74f, 143.20f, 395.14f, 3.14f}, {605.38f, 143.81f, 394.67f, 3.13f}, 
        {607.97f, 121.16f, 395.14f, 3.07f}, {602.02f, 122.15f, 395.66f, 2.97f}, 
        {611.24f, 138.07f, 395.13f, 2.90f}, {610.11f, 132.49f, 395.14f, 2.91f}, 
        {608.55f, 127.28f, 395.14f, 2.93f}, {605.62f, 138.90f, 394.65f, 3.06f}, 
        {604.59f, 133.98f, 394.66f, 2.89f}, {603.48f, 128.42f, 394.66f, 3.00f}
    },
    {
        //Horde (South Side)
        {520.48f, 120.54f, 395.14f, 0.26f}, {525.19f, 121.89f, 394.66f, 0.18f}, 
        {516.03f, 140.94f, 395.13f, 6.28f}, {521.19f, 141.24f, 394.67f, 0.00f}, 
        {519.24f, 124.74f, 395.14f, 0.28f}, {518.05f, 129.36f, 395.14f, 0.14f}, 
        {517.23f, 135.03f, 395.14f, 0.04f}, {523.98f, 125.91f, 394.66f, 0.25f}, 
        {522.65f, 130.43f, 394.67f, 0.18f}, {521.40f, 136.07f, 394.67f, 0.18f}
    }
};

struct DIAMOND_DLL_DECL instance_trial_of_the_crusader : public ScriptedInstance
{
    instance_trial_of_the_crusader(Map* pMap) : ScriptedInstance(pMap)
    {
        m_bIsRegularMode = pMap->IsRegularDifficulty();
        Initialize();
    };

    uint32 m_auiEncounter[MAX_ENCOUNTER];
    std::string strInstData;
    bool m_bIsRegularMode;

    uint64 mColiseumMasterGUID;
    uint64 mTirionGUID;
    uint64 mGatesGUID;
    int32 CloseGateTimer;
    uint64 mEntryDoorGUID;
    uint64 mGarroshGUID;
    uint64 mWrynnGUID;
   
    // Northrend Beasts
    uint64 mGormokGUID;
    uint64 mAcidmawGUID;
    uint64 mDreadscaleGUID;
    uint64 mIcehowlGUID;
    uint32 mBeastsEnrageTimer;
    bool AcidmawDead;
    bool DreadscaleDead;
    
    //Jaraxxus
    uint64 mJaraxxusGUID;
    uint64 mFizzlebangGUID;
    uint32 JaraxxusStage;
    uint32 JaraxxusTimer;
    
    //Faction Champions
    uint64 mChampionsGUID[10];
    uint64 mChampionsCacheGUID;
    uint32 mChampionsDead;
    uint32 mMaxChampions;
    uint32 mHChampRnd1;
    uint32 mHChampRnd2;
    uint32 mRChampRnd1;
    uint32 mRChampRnd2;
    uint32 mMChampRnd1;
    uint32 mMChampRnd2;
    uint32 mPlayerTeam;
    uint32 ChampionsTimer;
    uint32 ChampionsStage;

    //Twin Val'kyr
    uint64 mDarkbaneGUID;
    uint64 mLightbaneGUID;
    uint64 mEssenceGUID[4];

    void Initialize()
    {
        for(uint8 i=0; i<MAX_ENCOUNTER; ++i)
            m_auiEncounter[i] = NOT_STARTED;
            
        mGatesGUID = 0;
        mEntryDoorGUID = 0;
        mGarroshGUID = 0;
        mWrynnGUID = 0;
        mColiseumMasterGUID = 0;
        mTirionGUID = 0;

        //Northrend Beasts
        mBeastsEnrageTimer = 900000; //15 minutes
        mGormokGUID = 0;
        mAcidmawGUID = 0;
        mDreadscaleGUID = 0;
        mIcehowlGUID = 0;  
        CloseGateTimer = -1;
        AcidmawDead = false;
        DreadscaleDead = false;
        
        //Lord Jaraxxus
        mJaraxxusGUID = 0;
        mFizzlebangGUID = 0;
        JaraxxusTimer = 1; //first stage will be performed instantly
        JaraxxusStage = 0; //count backwards

        //Faction Champions
        mChampionsDead = 0;
        mMaxChampions = m_bIsRegularMode ? 6 : 10;
        mHChampRnd1 = urand(0,3);
        mHChampRnd2 = urand(1,3);
        mRChampRnd1 = urand(0,4);
        mRChampRnd2 = urand(1,4);
        mMChampRnd1 = urand(0,4);
        mMChampRnd2 = urand(1,4);
        mPlayerTeam = ALLIANCE;
        mChampionsCacheGUID = 0;
        ChampionsTimer = 1;
        ChampionsStage = 0;
        for(uint8 i=0; i<10; ++i) mChampionsGUID[i] = 0;

        //Twin Val'kyr
        mDarkbaneGUID = 0;
        mLightbaneGUID = 0;
        mEssenceGUID[0] = 0;
        mEssenceGUID[1] = 0;
        mEssenceGUID[2] = 0;
        mEssenceGUID[3] = 0;
    }

    void OpenDoor(uint64 guid)
    {
        if(!guid)
            return;
        GameObject* pGo = instance->GetGameObject(guid);
        if(pGo)
            pGo->SetGoState(GO_STATE_ACTIVE);
    }

    void CloseDoor(uint64 guid)
    {
        if(!guid)
            return;
        GameObject* pGo = instance->GetGameObject(guid);
        if(pGo)
            pGo->SetGoState(GO_STATE_READY);
    }

    void SayText(uint32 text_id, uint64 guid)
    {
        if(Creature *speaker = instance->GetCreature(guid))
            DoScriptText(text_id, speaker);
    }
    
/****************************************************
                 NORTHREND BEASTS
****************************************************/
    
    void SummonGormok()
    {
        if(mGormokGUID || !mColiseumMasterGUID)
            return;

        if(Creature *master = instance->GetCreature(mColiseumMasterGUID))
        {
            if(Creature *gormok = master->SummonCreature(34796, 564.11f, 265.47f, 397.09f, 4.72f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000))
            {
                mGormokGUID = gormok->GetGUID();
                gormok->GetMotionMaster()->MovePoint(0, 563.94f, 173.63f, 394.34f);
            }
        }

        if(mTirionGUID)
            SayText(SAY_GORMOK_INTRO, mTirionGUID);
    }
    
    void SummonAcidmawAndDreadscale()
    {
        if(!mColiseumMasterGUID)
            return;

        if(Creature *master = instance->GetCreature(mColiseumMasterGUID))
        {
            if(!mAcidmawGUID)
            {
                if(Creature *acidmaw = master->SummonCreature(35144, 548.57f, 172.59f, 394.68f, 4.63f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000))
                {
                    mAcidmawGUID = acidmaw->GetGUID();
                    acidmaw->SetInCombatWithZone();
                    AcidmawDead = false;
                }
            }

            if(!mDreadscaleGUID)
            {
                if(Creature *dreadscale = master->SummonCreature(34799, 564.11f, 265.47f, 397.09f, 4.72f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000))
                {
                    mDreadscaleGUID = dreadscale->GetGUID();
                    dreadscale->SetInCombatWithZone();
                    dreadscale->GetMotionMaster()->MovePoint(0, 579.90f, 172.35f, 394.68f);
                    DreadscaleDead = false;
                }
            }
        }

        if(mTirionGUID)
            SayText(SAY_JORMUNGAR_INTRO, mTirionGUID);
    }
    
    void SummonIcehowl()
    {
        if(mIcehowlGUID || !mColiseumMasterGUID)
            return;

        if(Creature *master = instance->GetCreature(mColiseumMasterGUID))
        {
            if(Creature *icehowl = master->SummonCreature(34797, 564.11f, 265.47f, 397.09f, 4.72f, TEMPSUMMON_DEAD_DESPAWN, 60000))
            {
                mIcehowlGUID = icehowl->GetGUID();
                icehowl->SetInCombatWithZone();
                icehowl->GetMotionMaster()->MovePoint(0, 564.04f, 139.98f, 393.84f);
            }
        }

        if(mTirionGUID)
            SayText(SAY_ICEHOWL_INTRO, mTirionGUID);
    }
    
    void DespawnBeasts()
    {
        Creature *boss;
        if(mGormokGUID)
        {
            if(boss = instance->GetCreature(mGormokGUID))
                boss->AddObjectToRemoveList();
            mGormokGUID = 0;
        }
        if(mAcidmawGUID)
        {
            if(boss = instance->GetCreature(mAcidmawGUID))
                boss->AddObjectToRemoveList();
            mAcidmawGUID = 0;
        }
        if(mDreadscaleGUID)
        {
            if(boss = instance->GetCreature(mDreadscaleGUID))
                boss->AddObjectToRemoveList();
            mDreadscaleGUID = 0;
        }
        if(mIcehowlGUID)
        {
            if(boss = instance->GetCreature(mIcehowlGUID))
                boss->AddObjectToRemoveList();
            mIcehowlGUID = 0;
        }
    }
    
/****************************************************
                  LORD JARAXXUS
****************************************************/
    
    void JaraxxusIntro()
    {
        switch(JaraxxusStage)
        {
            //(1)summon gnome and walk to the center
            case 9:
                if(Creature *master = instance->GetCreature(mColiseumMasterGUID))
                {
                    if(Creature *gnome = master->SummonCreature(35458, 564.11f, 220.47f, 397.09f, 4.72f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                    {
                        mFizzlebangGUID = gnome->GetGUID();
                        gnome->GetMotionMaster()->MovePoint(0, 563.94f, 150.55f, 394.3f);
                    }
                }
                JaraxxusTimer = 22000;
                break;
            case 8: //Thank you, Highlord...
                SayText(-1649008, mFizzlebangGUID);
                JaraxxusTimer = 10000;
                break;
            case 7: //prepare for oblivion!
                SayText(-1649009, mFizzlebangGUID);
                JaraxxusTimer = 5000;
                break;
            case 6: //*summon boss*  A-HA! I've done it!
                if(Creature *gnome = instance->GetCreature(mFizzlebangGUID))
                {
                    if(Creature *boss = gnome->SummonCreature(34780, 564.04f, 139.98f, 393.84f, 4.70f, TEMPSUMMON_DEAD_DESPAWN, 5000))
                    {
                        mJaraxxusGUID = boss->GetGUID();
                        boss->addUnitState(UNIT_STAT_STUNNED);
                        boss->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    }
                    DoScriptText(-1649010, gnome);
                }                   
                JaraxxusTimer = 14000;
                break;
            case 5: //Triffling gnome!
                SayText(-1649006, mJaraxxusGUID);
                JaraxxusTimer = 4000;
                break;
            case 4: //But I'm in charge here...
                SayText(-1649011, mFizzlebangGUID);
                JaraxxusTimer = 2000;
                break;
            case 3: //* kill gnome* Insignificant gnat!
                SayText(-1649007, mJaraxxusGUID);
                if(Creature *gnome = instance->GetCreature(mFizzlebangGUID))
                    gnome->DealDamage(gnome, gnome->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                JaraxxusTimer = 4000;
                break;
            case 2: //Quickly, heroes...
                SayText(-1649012, mTirionGUID);
                JaraxxusTimer = 7000;
                break;
            case 1: //You face Jaraxxus...
                if(Creature *boss = instance->GetCreature(mJaraxxusGUID))
                {
                    DoScriptText(-1649005, boss);
                    boss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    boss->clearUnitState(UNIT_STAT_STUNNED);
                    boss->SetInCombatWithZone();
                }
                JaraxxusTimer = 1000;
                break;
        }
    }
    
    void DespawnJaraxxus()
    {
        if(mJaraxxusGUID)
        {
            if(Creature *boss = instance->GetCreature(mJaraxxusGUID))
                boss->ForcedDespawn();
            mJaraxxusGUID = 0;
        }

        if(mFizzlebangGUID)
        {
            if(Creature *gnome = instance->GetCreature(mFizzlebangGUID))
                gnome->ForcedDespawn();
            mFizzlebangGUID = 0;
        }
    }

/****************************************************
                FACTION CHAMPIONS
****************************************************/

    void ChampionsIntro()
    {
        switch(ChampionsStage)
        {
            case 9:
                if(mPlayerTeam == ALLIANCE)
                    SayText(-1649020, mWrynnGUID);
                else
                    SayText(-1648016, mGarroshGUID);
                ChampionsTimer = 10000;
                break;
            case 8:
                SayText(-1649024, mTirionGUID);
                ChampionsStage = 1; //make it the last event
                ChampionsTimer = 1000;
                break;
            //Garrosh (Treacherous Alliance dogs!...)
            case 7:
                SayText(-1649013, mGarroshGUID);
                ChampionsTimer = 10000;
                break;
            //Wrynn (The Alliance doesn't need the help...)
            case 6:
                SayText(-1649017, mWrynnGUID);
                ChampionsTimer = 7500;
                break;
            //Tirion (Everyone, calm down!)
            case 5:
                SayText(-1649021, mTirionGUID);
                ChampionsTimer = 14000;
                break;
            //Tirion (The next battle will be...)
            case 4:
                SayText(-1649022, mTirionGUID);
                ChampionsTimer = 11000;
                break;
            //Garrosh / Wrynn
            case 3:
                if(mPlayerTeam == ALLIANCE)
                    SayText(-1649014, mGarroshGUID);
                else
                    SayText(-1649018, mWrynnGUID);
                ChampionsTimer = 16000;
                break;
            //Tirion (Very well. I will allow it.)
            case 2:
                SayText(-1649023, mTirionGUID);
                ChampionsTimer = 6500;
                break;
            //Garrosh / Wrynn (Battlecry) - Summon
            case 1:
                if(mPlayerTeam == ALLIANCE)
                    SayText(-1649015, mGarroshGUID);
                else
                    SayText(-1649019, mWrynnGUID);  
                SummonChampions();
                ChampionsTimer = 1000;
                break;
        }
    }

    void SummonChampions()
    {
        Unit *master = instance->GetCreature(mColiseumMasterGUID);
        if(!master)
            return;

        uint8 team = (mPlayerTeam==ALLIANCE) ? 1 : 0;
        uint8 coordsIdx = 0;
        uint8 guidIdx = 0;
        uint8 entryIdx = 0;
        uint8 r = m_bIsRegularMode ? 2 : 4;
        uint8 hm = m_bIsRegularMode ? 2 : 3;
        //Ranged
        entryIdx = mRChampRnd1;
        for(coordsIdx=0; coordsIdx < r; ++coordsIdx)
        {
            if(Unit *champ = master->SummonCreature(ChampRanged[team][entryIdx],
                ChampSpawnLocs[team][coordsIdx][0],
                ChampSpawnLocs[team][coordsIdx][1],
                ChampSpawnLocs[team][coordsIdx][2],
                ChampSpawnLocs[team][coordsIdx][3],
                TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000))
            {
                mChampionsGUID[guidIdx++] = champ->GetGUID();
            }
            entryIdx = (entryIdx + mRChampRnd2) % 5;
        }
        //Healers
        entryIdx = mHChampRnd1;
        for(coordsIdx=4; coordsIdx < hm+4; ++coordsIdx)
        {
            if(Unit *champ = master->SummonCreature(ChampHealers[team][entryIdx],
                ChampSpawnLocs[team][coordsIdx][0],
                ChampSpawnLocs[team][coordsIdx][1],
                ChampSpawnLocs[team][coordsIdx][2],
                ChampSpawnLocs[team][coordsIdx][3],
                TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000))
            {
                mChampionsGUID[guidIdx++] = champ->GetGUID();
            }
            entryIdx = (entryIdx + mHChampRnd2) % 4;
        }
        //Melee
        entryIdx = mMChampRnd1;
        for(coordsIdx=7; coordsIdx < hm+7; ++coordsIdx)
        {
            if(Unit *champ = master->SummonCreature(ChampMelee[team][entryIdx],
                ChampSpawnLocs[team][coordsIdx][0],
                ChampSpawnLocs[team][coordsIdx][1],
                ChampSpawnLocs[team][coordsIdx][2],
                ChampSpawnLocs[team][coordsIdx][3],
                TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000))
            {
                mChampionsGUID[guidIdx++] = champ->GetGUID();
            }
            entryIdx = (entryIdx + mMChampRnd2) % 5;
        }
        
        mChampionsDead = 0;
    }
    
    void SetChampionsInCombat()
    {
        uint8 i;
        for(i=0; i<10; ++i)
        {
            if(mChampionsGUID[i])
            {
                if(Creature *champ = instance->GetCreature(mChampionsGUID[i]))
                    champ->SetInCombatWithZone();
            }
        }
    }

    void DespawnChampions()
    {
        uint8 i;
        for(i=0; i<10; ++i)
        {
            if(mChampionsGUID[i])
            {
                if(Creature *champ = instance->GetCreature(mChampionsGUID[i]))
                    champ->AddObjectToRemoveList();
                mChampionsGUID[i] = 0;
            }
        }
    }

/****************************************************
                    TWIN VAL'KYR
****************************************************/
    void SummonValkyr()
    {
        if(!mColiseumMasterGUID)
            return;

        if(Creature *master = instance->GetCreature(mColiseumMasterGUID))
        {
            //Eydis Darkbane
            if(Creature *darkbane = master->SummonCreature(34496, 563.11f, 260.0f, 397.09f, 4.72f, TEMPSUMMON_DEAD_DESPAWN, 5000))
            {
                mDarkbaneGUID = darkbane->GetGUID();
                darkbane->GetMotionMaster()->MovePoint(0, 548.57f, 172.59f, 394.68f);
            }
            //Fjola Lightbane
            if(Creature *lightbane = master->SummonCreature(34497, 565.11f, 260.0f, 397.09f, 4.72f, TEMPSUMMON_DEAD_DESPAWN, 5000))
            {
                mLightbaneGUID = lightbane->GetGUID();
                lightbane->GetMotionMaster()->MovePoint(0, 579.90f, 172.35f, 394.68f);
            }
            //Dark Essence
            if(Creature *portal = master->SummonCreature(34567, 586.89f, 116.63f, 394.34f, 0.0, TEMPSUMMON_CORPSE_DESPAWN, 5000))
                mEssenceGUID[0] = portal->GetGUID();
            if(Creature *portal = master->SummonCreature(34567, 540.61f, 163.14f, 394.64f, 0.0, TEMPSUMMON_CORPSE_DESPAWN, 5000))
                mEssenceGUID[1] = portal->GetGUID();
            //Light Essence
            if(Creature *portal = master->SummonCreature(34568, 540.23f, 116.63f, 394.34f, 0.0, TEMPSUMMON_CORPSE_DESPAWN, 5000))
                mEssenceGUID[2] = portal->GetGUID();
            if(Creature *portal = master->SummonCreature(34568, 587.19f, 163.04f, 394.64f, 0.0, TEMPSUMMON_CORPSE_DESPAWN, 5000))
                mEssenceGUID[3] = portal->GetGUID();
        }

        SayText(-1649025, mTirionGUID);
    }

    void DespawnValkyr()
    {
        if(mDarkbaneGUID)
        {
            if(Creature *darkbane = instance->GetCreature(mDarkbaneGUID))
                darkbane->ForcedDespawn();
            mDarkbaneGUID = 0;
        }

        if(mLightbaneGUID)
        {
            if(Creature *lightbane = instance->GetCreature(mLightbaneGUID))
                lightbane->ForcedDespawn();
            mLightbaneGUID = 0;
        }
    }

    void DespawnEssences()
    {
        for(int i=0; i<4; ++i)
        {
            if(mEssenceGUID[i])
            {
                if(Creature *essence = instance->GetCreature(mEssenceGUID[i]))
                    essence->ForcedDespawn();
                mEssenceGUID[i] = 0;
            }
        }
    }


/*=========================================================*/  

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case 34816: mColiseumMasterGUID = pCreature->GetGUID(); break;
            case 34996: mTirionGUID = pCreature->GetGUID(); break;
            case 34995: mGarroshGUID = pCreature->GetGUID(); break;
            case 34990: mWrynnGUID = pCreature->GetGUID(); break;
        }
    }

    void OnObjectCreate(GameObject *pGo)
    {
        switch(pGo->GetEntry())
        {
            case 195631: if(m_bIsRegularMode) mChampionsCacheGUID = pGo->GetGUID(); break;
            case 195632: if(!m_bIsRegularMode) mChampionsCacheGUID = pGo->GetGUID(); break;
            case 195647: mGatesGUID = pGo->GetGUID(); break;
            case 195589: mEntryDoorGUID = pGo->GetGUID(); break;
        }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        debug_log("DS: Instance Trial Of The Crusader: SetData received for type %u with data %u",uiType,uiData);

        switch(uiType)
        {
             case TYPE_NORTHREND_BEASTS:
                m_auiEncounter[0] = uiData;
                if(uiData==IN_PROGRESS)
                {
                    OpenDoor(mGatesGUID);
                    CloseDoor(mEntryDoorGUID);
                    CloseGateTimer = 20000;
                    SummonGormok();
                    mBeastsEnrageTimer = 900000;
                }
                else if(uiData==NOT_STARTED)
                {
                    OpenDoor(mEntryDoorGUID);
                    DespawnBeasts();
                }
                else if(uiData == DONE) OpenDoor(mEntryDoorGUID);
                break;
             case TYPE_JARAXXUS:
                m_auiEncounter[1] = uiData;
                if(uiData == IN_PROGRESS)
                {
                    OpenDoor(mGatesGUID);
                    CloseDoor(mEntryDoorGUID);
                    CloseGateTimer = 12000;
                    JaraxxusStage = 9;
                }
                if(uiData == NOT_STARTED)
                {
                    OpenDoor(mEntryDoorGUID);
                    DespawnJaraxxus();
                }
                if(uiData == DONE) OpenDoor(mEntryDoorGUID);
                break;
             case TYPE_FACTION_CHAMPIONS:
                m_auiEncounter[2] = uiData;
                if(uiData == IN_PROGRESS)
                {
                    CloseDoor(mEntryDoorGUID);
                    ChampionsStage = 7;
                }
                if(uiData == NOT_STARTED)
                {
                    OpenDoor(mEntryDoorGUID);
                    DespawnChampions();
                }
                if(uiData == DONE)
                {
                    OpenDoor(mEntryDoorGUID);
                    DoRespawnGameObject(mChampionsCacheGUID, DAY);
                    ChampionsStage = 9;
                }
                if(uiData == SPECIAL) //one champion died
                {
                    ++mChampionsDead;
                    if(mChampionsDead >= mMaxChampions) SetData(TYPE_FACTION_CHAMPIONS, DONE);
                }
                if(uiData == 5) SetChampionsInCombat();
                break;
             case TYPE_TWIN_VALKYR:
                m_auiEncounter[3] = uiData;
                if(uiData == IN_PROGRESS)
                {
                    CloseDoor(mEntryDoorGUID);
                    OpenDoor(mGatesGUID);
                    CloseGateTimer = 25000;
                    SummonValkyr();
                }
                if(uiData == NOT_STARTED)
                {
                    OpenDoor(mEntryDoorGUID);
                    DespawnValkyr();
                    DespawnEssences();
                }
                if(uiData == DONE)
                {
                    OpenDoor(mEntryDoorGUID);
                    DespawnEssences();
                }
                break;
             case TYPE_ANUBARAK:
                m_auiEncounter[4] = uiData;
                break;
            case DATA_GORMOK:
                if(uiData==DONE)
                {
                    OpenDoor(mGatesGUID);
                    CloseGateTimer = 10000;
                    SummonAcidmawAndDreadscale();
                }
                break;
            case DATA_ACIDMAW:
                AcidmawDead = (uiData==DONE);
                if(AcidmawDead)
                {
                    if(DreadscaleDead)
                    {
                        OpenDoor(mGatesGUID);
                        CloseGateTimer = 9000;
                        SummonIcehowl();
                    }
                    else
                    {
                        //buff dreadscale
                        if(Unit *dreadscale = instance->GetCreature(mDreadscaleGUID))
                            dreadscale->CastSpell(dreadscale, 68335, true);
                    }
                }
                break;
            case DATA_DREADSCALE:
                DreadscaleDead = (uiData==DONE);
                if(DreadscaleDead)
                {
                    if(AcidmawDead)
                    {
                        OpenDoor(mGatesGUID);
                        CloseGateTimer = 9000;
                        SummonIcehowl();
                    }
                    else
                    {
                        //buff acidmaw
                        if(Unit *acidmaw = instance->GetCreature(mAcidmawGUID))
                            acidmaw->CastSpell(acidmaw, 68335, true);
                    }
                }
                break;
            case DATA_BEASTS_ENRAGE:
                mBeastsEnrageTimer = uiData;
                break;
            case DATA_PLAYER_TEAM:
                mPlayerTeam = uiData;
                break;
             default:
                error_log("DS: Instance Trial of The Crusader: ERROR SetData = %u for type %u does not exist/not implemented.",uiType,uiData);
                break;
        }

        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                << m_auiEncounter[3] << " " << m_auiEncounter[4];

            strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_NORTHREND_BEASTS:  return m_auiEncounter[0];
            case TYPE_JARAXXUS:          return m_auiEncounter[1];
            case TYPE_FACTION_CHAMPIONS: return m_auiEncounter[2];
            case TYPE_TWIN_VALKYR:       return m_auiEncounter[3];
            case TYPE_ANUBARAK:          return m_auiEncounter[4];

            case DATA_BEASTS_ENRAGE:     return mBeastsEnrageTimer;
        }
        return 0;
    }

    uint64 GetData64(uint32 uiData)
    {
        switch(uiData)
        {
            case DATA_DARKBANE: return mDarkbaneGUID;
            case DATA_LIGHTBANE: return mLightbaneGUID;
        }
        return 0;
    }

    void Update(uint32 diff)
    {
        if(m_auiEncounter[0]==IN_PROGRESS && mBeastsEnrageTimer > diff)
            mBeastsEnrageTimer -= diff;

        if(CloseGateTimer > 0)
        {
            if(CloseGateTimer <= int32(diff))
            {
                CloseDoor(mGatesGUID);
                CloseGateTimer = -1;
            }
            else
                CloseGateTimer -= diff;
        }
        
        //Jaraxxus Intro
        if(JaraxxusStage > 0)
        {
            if(JaraxxusTimer < diff)
            {
                JaraxxusIntro();
                --JaraxxusStage;
            }
            else JaraxxusTimer -= diff;
        }

        //Champions Intro
        if(ChampionsStage > 0)
        {
            if(ChampionsTimer < diff)
            {
                ChampionsIntro();
                --ChampionsStage;
            }
            else ChampionsTimer -= diff;
        }
    }

    const char* Save()
    {
        return strInstData.c_str();
    }

    void Load(const char* chrIn)
    {
        if (!chrIn)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(chrIn);

        std::istringstream loadStream(chrIn);
        loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
            >> m_auiEncounter[4];

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            if (m_auiEncounter[i] == IN_PROGRESS)            // Do not load an encounter as "In Progress" - reset it instead.
                m_auiEncounter[i] = NOT_STARTED;

        OUT_LOAD_INST_DATA_COMPLETE;
    }

    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            if (m_auiEncounter[i] == IN_PROGRESS)
                return true;

        return false;
    }
};

InstanceData* GetInstanceData_instance_trial_of_the_crusader(Map* pMap)
{
    return new instance_trial_of_the_crusader(pMap);
}

void AddSC_instance_trial_of_the_crusader()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "instance_trial_of_the_crusader";
    newscript->GetInstanceData = &GetInstanceData_instance_trial_of_the_crusader;
    newscript->RegisterSelf();
}
