ALTER TABLE account
CHANGE COLUMN online `active_realm_id` int(11) unsigned NOT NULL DEFAULT '0';

ALTER TABLE realmlist
CHANGE COLUMN gamebuild `realmbuilds` varchar(64) NOT NULL DEFAULT '',
ADD COLUMN `realmflags` tinyint(3) unsigned NOT NULL DEFAULT '2' COMMENT 'Supported masks: 0x1 (invalid, not show in realm list), 0x2 (offline, set by mangosd), 0x4 (show version and build), 0x20 (new players), 0x40 (recommended)' AFTER icon;

ALTER TABLE uptime
DROP COLUMN revision;