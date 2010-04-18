/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_TRIAL_OF_THE_CRUSADER_H
#define DEF_TRIAL_OF_THE_CRUSADER_H

enum
{
    MAX_ENCOUNTER          = 5,

    TYPE_NORTHREND_BEASTS  = 1,
    TYPE_JARAXXUS          = 2,
    TYPE_FACTION_CHAMPIONS = 3,
    TYPE_TWIN_VALKYR       = 4,
    TYPE_ANUBARAK          = 5,

    DATA_GORMOK            = 7,
    DATA_ACIDMAW           = 8,
    DATA_DREADSCALE        = 9,
    DATA_ICEHOWL           = 10,
    DATA_BEASTS_ENRAGE     = 11,
    DATA_PLAYER_TEAM       = 12,

    DATA_DARKBANE          = 12,
    DATA_LIGHTBANE         = 13
};

enum
{
    TYPE_DARKBANE_GUID  = 1,
    TYPE_LIGHTBANE_GUID = 2
};

#define ROOM_CENTER_X   564.04f
#define ROOM_CENTER_Y   139.98f
#define ROOM_CENTER_Z   393.84f

#endif

//coordinates:
//gates center:
//563.94 173.63 394.34 4.68 
//gates right: (dark)
//579.90 172.35 394.68 4.80
//gates left: (light)
//548.57 172.59 394.68 4.63
//room center:
//564.04 139.98 393.84 4.70

//summon point:
//564.11 265.47 397.09 4.72
