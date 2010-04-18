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

#define BEASTS          1001
#define JARAXXUS        1002
#define CHAMPIONS       1003
#define TWINS           1004
#define ANUBARAK        1005
#define COMPLETED       1006

bool GossipHello_npc_crusader_announcer(Player *player, Creature *creature)
{
    ScriptedInstance *pInstance = (ScriptedInstance *) creature->GetInstanceData();
    if(!pInstance)
        return true;
    if(pInstance->IsEncounterInProgress())
        return true;
    Difficulty diff = creature->GetMap()->GetDifficulty();

    if(diff!=RAID_DIFFICULTY_10MAN_NORMAL && diff!=RAID_DIFFICULTY_25MAN_NORMAL)
        return true;

    if(pInstance->GetData(TYPE_NORTHREND_BEASTS) != DONE)
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Northrend Beasts", GOSSIP_SENDER_MAIN, BEASTS);
    else if(pInstance->GetData(TYPE_JARAXXUS) != DONE)
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Lord Jaraxxus", GOSSIP_SENDER_MAIN, JARAXXUS);
    else if(pInstance->GetData(TYPE_FACTION_CHAMPIONS) != DONE)
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Faction Champions", GOSSIP_SENDER_MAIN, CHAMPIONS);
    else if(pInstance->GetData(TYPE_TWIN_VALKYR) != DONE)
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Twin Val'kyr", GOSSIP_SENDER_MAIN, TWINS);
    else
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Anub'arak", GOSSIP_SENDER_MAIN, ANUBARAK);


    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());

    return true;
}

bool GossipSelect_npc_crusader_announcer(Player *player, Creature *creature, uint32 sender, uint32 action)
{
    if(sender != GOSSIP_SENDER_MAIN)
        return true;
    ScriptedInstance *pInstance = (ScriptedInstance *) creature->GetInstanceData();
    if(!pInstance)
        return true;

    switch(action)
    {
    case BEASTS:
        pInstance->SetData(TYPE_NORTHREND_BEASTS, IN_PROGRESS);
        break;
    case JARAXXUS:
        pInstance->SetData(TYPE_JARAXXUS, IN_PROGRESS);
        break;
    case CHAMPIONS:
        pInstance->SetData(DATA_PLAYER_TEAM, player->GetTeam());
        pInstance->SetData(TYPE_FACTION_CHAMPIONS, IN_PROGRESS);
        break;
    case TWINS:
        pInstance->SetData(TYPE_TWIN_VALKYR, IN_PROGRESS);
        break;
    case ANUBARAK:
        player->NearTeleportTo(564.0f, 132.0f, 380.0f, 0.0f);
        break;
    }

    player->CLOSE_GOSSIP_MENU();
    return true;
}

void AddSC_npc_crusader_announcer()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "npc_crusader_announcer";
    newscript->pGossipHello = &GossipHello_npc_crusader_announcer;
    newscript->pGossipSelect = &GossipSelect_npc_crusader_announcer;
    newscript->RegisterSelf();
}
