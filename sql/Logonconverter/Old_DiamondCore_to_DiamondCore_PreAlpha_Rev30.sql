ALTER TABLE account
CHANGE COLUMN online `active_realm_id` int(11) unsigned NOT NULL DEFAULT '0';

ALTER TABLE realmlist
CHANGE COLUMN gamebuild `realmbuilds` varchar(64) NOT NULL DEFAULT '';

ALTER TABLE uptime
DROP COLUMN revision;