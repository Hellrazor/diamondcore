/*
 * Copyright (C) 2010 DiamondCore <http://diamondcore.eu/>
 *
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

#ifndef __SCRIPT_CALLS_H
#define __SCRIPT_CALLS_H

#include "Common.h"
#include "ObjectMgr.h"
#include "DBCEnums.h"

class Creature;
class CreatureAI;
class GameObject;
class Item;
class Player;
class Quest;
class SpellCastTargets;
class Map;
class InstanceData;

bool LoadScriptingModule(char const* libName = "");
void UnloadScriptingModule();

typedef void(DIAMOND_IMPORT * scriptCallScriptsInit) (const ObjectMgr::ScriptNameMap &scriptNames);
typedef void(DIAMOND_IMPORT * scriptCallScriptsFree) ();
typedef char const* (DIAMOND_IMPORT * scriptCallScriptsVersion) ();

typedef bool(DIAMOND_IMPORT * scriptCallGossipHello) (Player *player, Creature *_Creature );
typedef bool(DIAMOND_IMPORT * scriptCallGOGossipHello) (Player *player, GameObject *_GO);
typedef bool(DIAMOND_IMPORT * scriptCallQuestAccept) (Player *player, Creature *_Creature, Quest const *);
typedef bool(DIAMOND_IMPORT * scriptCallGossipSelect)(Player *player, Creature *_Creature, uint32 sender, uint32 action);
typedef bool(DIAMOND_IMPORT * scriptCallGOGossipSelect)(Player *player, GameObject *_GO, uint32 sender, uint32 action);
typedef bool(DIAMOND_IMPORT * scriptCallGossipSelectWithCode)( Player *player, Creature *_Creature, uint32 sender, uint32 action, const char* sCode );
typedef bool(DIAMOND_IMPORT * scriptCallGOGossipSelectWithCode)( Player *player, GameObject *_GO, uint32 sender, uint32 action, const char* sCode );
typedef bool(DIAMOND_IMPORT * scriptCallQuestSelect)( Player *player, Creature *_Creature, Quest const* );
typedef bool(DIAMOND_IMPORT * scriptCallQuestComplete)(Player *player, Creature *_Creature, Quest const*);
typedef uint32(DIAMOND_IMPORT * scriptCallNPCDialogStatus)( Player *player, Creature *_Creature);
typedef uint32(DIAMOND_IMPORT * scriptCallGODialogStatus)( Player *player, GameObject * _GO);
typedef bool(DIAMOND_IMPORT * scriptCallChooseReward)( Player *player, Creature *_Creature, Quest const*, uint32 opt );
typedef bool(DIAMOND_IMPORT * scriptCallItemHello)( Player *player, Item *, Quest const*);
typedef bool(DIAMOND_IMPORT * scriptCallGOHello)( Player *player, GameObject * );
typedef bool(DIAMOND_IMPORT * scriptCallAreaTrigger)( Player *player, AreaTriggerEntry const* );
typedef bool(DIAMOND_IMPORT * scriptCallItemQuestAccept)(Player *player, Item *, Quest const*);
typedef bool(DIAMOND_IMPORT * scriptCallGOQuestAccept)(Player *player, GameObject *, Quest const*);
typedef bool(DIAMOND_IMPORT * scriptCallGOChooseReward)(Player *player, GameObject *, Quest const*, uint32 opt );
typedef bool(DIAMOND_IMPORT * scriptCallItemUse) (Player *player, Item *_Item, SpellCastTargets const& targets);
typedef bool(DIAMOND_IMPORT * scriptCallEffectDummyGameObj) (Unit *caster, uint32 spellId, SpellEffectIndex effIndex, GameObject *gameObjTarget);
typedef bool(DIAMOND_IMPORT * scriptCallEffectDummyCreature) (Unit *caster, uint32 spellId, SpellEffectIndex effIndex, Creature *crTarget);
typedef bool(DIAMOND_IMPORT * scriptCallEffectDummyItem) (Unit *caster, uint32 spellId, SpellEffectIndex effIndex, Item *itemTarget);
typedef bool(DIAMOND_IMPORT * scriptCallEffectAuraDummy) (const Aura* pAura, bool apply);
typedef CreatureAI* (DIAMOND_IMPORT * scriptCallGetAI) ( Creature *_Creature );
typedef InstanceData* (DIAMOND_IMPORT * scriptCallCreateInstanceData) (Map *map);

typedef struct
{
    scriptCallScriptsInit ScriptsInit;
    scriptCallScriptsFree ScriptsFree;
    scriptCallScriptsVersion ScriptsVersion;

    scriptCallGossipHello GossipHello;
    scriptCallGOGossipHello GOGossipHello;
    scriptCallGOChooseReward GOChooseReward;
    scriptCallQuestAccept QuestAccept;
    scriptCallGossipSelect GossipSelect;
    scriptCallGOGossipSelect GOGossipSelect;
    scriptCallGossipSelectWithCode GossipSelectWithCode;
    scriptCallGOGossipSelectWithCode GOGossipSelectWithCode;
    scriptCallQuestSelect QuestSelect;
    scriptCallQuestComplete QuestComplete;
    scriptCallNPCDialogStatus NPCDialogStatus;
    scriptCallGODialogStatus GODialogStatus;
    scriptCallChooseReward ChooseReward;
    scriptCallItemHello ItemHello;
    scriptCallGOHello GOHello;
    scriptCallAreaTrigger scriptAreaTrigger;
    scriptCallItemQuestAccept ItemQuestAccept;
    scriptCallGOQuestAccept GOQuestAccept;
    scriptCallItemUse ItemUse;
    scriptCallEffectDummyGameObj  EffectDummyGameObj;
    scriptCallEffectDummyCreature EffectDummyCreature;
    scriptCallEffectDummyItem     EffectDummyItem;
    scriptCallEffectAuraDummy     EffectAuraDummy;
    scriptCallGetAI GetAI;
    scriptCallCreateInstanceData CreateInstanceData;

    DIAMOND_LIBRARY_HANDLE hScriptsLib;
}_ScriptSet,*ScriptsSet;

extern ScriptsSet Script;
#endif
