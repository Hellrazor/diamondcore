/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#include "precompiled.h"

//return closest GO in grid, with range from pSource
GameObject* GetClosestGameObjectWithEntry(WorldObject* pSource, uint32 uiEntry, float fMaxSearchRange)
{
    GameObject* pGo = NULL;

    CellPair pair(Diamond::ComputeCellPair(pSource->GetPositionX(), pSource->GetPositionY()));
    Cell cell(pair);
    cell.data.Part.reserved = ALL_DISTRICT;
    cell.SetNoCreate();

    Diamond::NearestGameObjectEntryInObjectRangeCheck go_check(*pSource, uiEntry, fMaxSearchRange);
    Diamond::GameObjectLastSearcher<Diamond::NearestGameObjectEntryInObjectRangeCheck> searcher(pSource, pGo, go_check);

    GridTypeVisitor<Diamond::GameObjectLastSearcher<Diamond::NearestGameObjectEntryInObjectRangeCheck> >::Grid go_searcher(searcher);

    cell.Visit(pair, go_searcher,*(pSource->GetMap()), *pSource, fMaxSearchRange);

    return pGo;
}

//return closest creature alive in grid, with range from pSource
Creature* GetClosestCreatureWithEntry(WorldObject* pSource, uint32 uiEntry, float fMaxSearchRange)
{
    Creature* pCreature = NULL;

    CellPair pair(Diamond::ComputeCellPair(pSource->GetPositionX(), pSource->GetPositionY()));
    Cell cell(pair);
    cell.data.Part.reserved = ALL_DISTRICT;
    cell.SetNoCreate();

    Diamond::NearestCreatureEntryWithLiveStateInObjectRangeCheck creature_check(*pSource, uiEntry, true, fMaxSearchRange);
    Diamond::CreatureLastSearcher<Diamond::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pSource, pCreature, creature_check);

    GridTypeVisitor<Diamond::CreatureLastSearcher<Diamond::NearestCreatureEntryWithLiveStateInObjectRangeCheck> >::Grid creature_searcher(searcher);

    cell.Visit(pair, creature_searcher,*(pSource->GetMap()), *pSource, fMaxSearchRange);

    return pCreature;
}

void GetGameObjectListWithEntryInGrid(std::list<GameObject*>& lList , WorldObject* pSource, uint32 uiEntry, float fMaxSearchRange)
{
    CellPair pair(Diamond::ComputeCellPair(pSource->GetPositionX(), pSource->GetPositionY()));
    Cell cell(pair);
    cell.data.Part.reserved = ALL_DISTRICT;
    cell.SetNoCreate();

    AllGameObjectsWithEntryInRange check(pSource, uiEntry, fMaxSearchRange);
    Diamond::GameObjectListSearcher<AllGameObjectsWithEntryInRange> searcher(pSource, lList, check);
    GridTypeVisitor<Diamond::GameObjectListSearcher<AllGameObjectsWithEntryInRange> >::Grid visitor(searcher);

    cell.Visit(pair, visitor, *(pSource->GetMap()), *pSource, fMaxSearchRange);
}

void GetCreatureListWithEntryInGrid(std::list<Creature*>& lList, WorldObject* pSource, uint32 uiEntry, float fMaxSearchRange)
{
    CellPair pair(Diamond::ComputeCellPair(pSource->GetPositionX(), pSource->GetPositionY()));
    Cell cell(pair);
    cell.data.Part.reserved = ALL_DISTRICT;
    cell.SetNoCreate();

    AllCreaturesOfEntryInRange check(pSource, uiEntry, fMaxSearchRange);
    Diamond::CreatureListSearcher<AllCreaturesOfEntryInRange> searcher(pSource, lList, check);
    GridTypeVisitor<Diamond::CreatureListSearcher<AllCreaturesOfEntryInRange> >::Grid visitor(searcher);

    cell.Visit(pair, visitor, *(pSource->GetMap()), *pSource, fMaxSearchRange);
}
