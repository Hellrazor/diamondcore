DROP TABLE IF EXISTS addons;
DROP TABLE IF EXISTS channels;

-- ----------------------------
-- Table structure for `auctionhousebot`
-- ----------------------------
DROP TABLE IF EXISTS `auctionhousebot`;
CREATE TABLE `auctionhousebot` (
  `auctionhouse` int(11) NOT NULL DEFAULT '0' COMMENT 'mapID of the auctionhouse.',
  `name` char(25) DEFAULT NULL COMMENT 'Text name of the auctionhouse.',
  `minitems` int(11) DEFAULT '0' COMMENT 'This is the minimum number of items you want to keep in the auction house. a 0 here will make it the same as the maximum.',
  `maxitems` int(11) DEFAULT '0' COMMENT 'This is the number of items you want to keep in the auction house.',
  `mintime` int(11) DEFAULT '8' COMMENT 'Sets the minimum number of hours for an auction.',
  `maxtime` int(11) DEFAULT '24' COMMENT 'Sets the maximum number of hours for an auction.',
  `percentgreytradegoods` int(11) DEFAULT '0' COMMENT 'Sets the percentage of the Grey Trade Goods auction items',
  `percentwhitetradegoods` int(11) DEFAULT '27' COMMENT 'Sets the percentage of the White Trade Goods auction items',
  `percentgreentradegoods` int(11) DEFAULT '12' COMMENT 'Sets the percentage of the Green Trade Goods auction items',
  `percentbluetradegoods` int(11) DEFAULT '10' COMMENT 'Sets the percentage of the Blue Trade Goods auction items',
  `percentpurpletradegoods` int(11) DEFAULT '1' COMMENT 'Sets the percentage of the Purple Trade Goods auction items',
  `percentorangetradegoods` int(11) DEFAULT '0' COMMENT 'Sets the percentage of the Orange Trade Goods auction items',
  `percentyellowtradegoods` int(11) DEFAULT '0' COMMENT 'Sets the percentage of the Yellow Trade Goods auction items',
  `percentgreyitems` int(11) DEFAULT '0' COMMENT 'Sets the percentage of the non trade Grey auction items',
  `percentwhiteitems` int(11) DEFAULT '10' COMMENT 'Sets the percentage of the non trade White auction items',
  `percentgreenitems` int(11) DEFAULT '30' COMMENT 'Sets the percentage of the non trade Green auction items',
  `percentblueitems` int(11) DEFAULT '8' COMMENT 'Sets the percentage of the non trade Blue auction items',
  `percentpurpleitems` int(11) DEFAULT '2' COMMENT 'Sets the percentage of the non trade Purple auction items',
  `percentorangeitems` int(11) DEFAULT '0' COMMENT 'Sets the percentage of the non trade Orange auction items',
  `percentyellowitems` int(11) DEFAULT '0' COMMENT 'Sets the percentage of the non trade Yellow auction items',
  `minpricegrey` int(11) DEFAULT '100' COMMENT 'Minimum price of Grey items (percentage).',
  `maxpricegrey` int(11) DEFAULT '150' COMMENT 'Maximum price of Grey items (percentage).',
  `minpricewhite` int(11) DEFAULT '150' COMMENT 'Minimum price of White items (percentage).',
  `maxpricewhite` int(11) DEFAULT '250' COMMENT 'Maximum price of White items (percentage).',
  `minpricegreen` int(11) DEFAULT '800' COMMENT 'Minimum price of Green items (percentage).',
  `maxpricegreen` int(11) DEFAULT '1400' COMMENT 'Maximum price of Green items (percentage).',
  `minpriceblue` int(11) DEFAULT '1250' COMMENT 'Minimum price of Blue items (percentage).',
  `maxpriceblue` int(11) DEFAULT '1750' COMMENT 'Maximum price of Blue items (percentage).',
  `minpricepurple` int(11) DEFAULT '2250' COMMENT 'Minimum price of Purple items (percentage).',
  `maxpricepurple` int(11) DEFAULT '4550' COMMENT 'Maximum price of Purple items (percentage).',
  `minpriceorange` int(11) DEFAULT '3250' COMMENT 'Minimum price of Orange items (percentage).',
  `maxpriceorange` int(11) DEFAULT '5550' COMMENT 'Maximum price of Orange items (percentage).',
  `minpriceyellow` int(11) DEFAULT '5250' COMMENT 'Minimum price of Yellow items (percentage).',
  `maxpriceyellow` int(11) DEFAULT '6550' COMMENT 'Maximum price of Yellow items (percentage).',
  `minbidpricegrey` int(11) DEFAULT '70' COMMENT 'Starting bid price of Grey items as a percentage of the randomly chosen buyout price. Default: 70',
  `maxbidpricegrey` int(11) DEFAULT '100' COMMENT 'Starting bid price of Grey items as a percentage of the randomly chosen buyout price. Default: 100',
  `minbidpricewhite` int(11) DEFAULT '70' COMMENT 'Starting bid price of White items as a percentage of the randomly chosen buyout price. Default: 70',
  `maxbidpricewhite` int(11) DEFAULT '100' COMMENT 'Starting bid price of White items as a percentage of the randomly chosen buyout price. Default: 100',
  `minbidpricegreen` int(11) DEFAULT '80' COMMENT 'Starting bid price of Green items as a percentage of the randomly chosen buyout price. Default: 80',
  `maxbidpricegreen` int(11) DEFAULT '100' COMMENT 'Starting bid price of Green items as a percentage of the randomly chosen buyout price. Default: 100',
  `minbidpriceblue` int(11) DEFAULT '75' COMMENT 'Starting bid price of Blue items as a percentage of the randomly chosen buyout price. Default: 75',
  `maxbidpriceblue` int(11) DEFAULT '100' COMMENT 'Starting bid price of Blue items as a percentage of the randomly chosen buyout price. Default: 100',
  `minbidpricepurple` int(11) DEFAULT '80' COMMENT 'Starting bid price of Purple items as a percentage of the randomly chosen buyout price. Default: 80',
  `maxbidpricepurple` int(11) DEFAULT '100' COMMENT 'Starting bid price of Purple items as a percentage of the randomly chosen buyout price. Default: 100',
  `minbidpriceorange` int(11) DEFAULT '80' COMMENT 'Starting bid price of Orange items as a percentage of the randomly chosen buyout price. Default: 80',
  `maxbidpriceorange` int(11) DEFAULT '100' COMMENT 'Starting bid price of Orange items as a percentage of the randomly chosen buyout price. Default: 100',
  `minbidpriceyellow` int(11) DEFAULT '80' COMMENT 'Starting bid price of Yellow items as a percentage of the randomly chosen buyout price. Default: 80',
  `maxbidpriceyellow` int(11) DEFAULT '100' COMMENT 'Starting bid price of Yellow items as a percentage of the randomly chosen buyout price. Default: 100',
  `maxstackgrey` int(11) DEFAULT '0' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `maxstackwhite` int(11) DEFAULT '0' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `maxstackgreen` int(11) DEFAULT '3' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `maxstackblue` int(11) DEFAULT '2' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `maxstackpurple` int(11) DEFAULT '1' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `maxstackorange` int(11) DEFAULT '1' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `maxstackyellow` int(11) DEFAULT '1' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `buyerpricegrey` int(11) DEFAULT '1' COMMENT 'Multiplier to vendorprice when buying grey items from auctionhouse',
  `buyerpricewhite` int(11) DEFAULT '1' COMMENT 'Multiplier to vendorprice when buying white items from auctionhouse',
  `buyerpricegreen` int(11) DEFAULT '5' COMMENT 'Multiplier to vendorprice when buying green items from auctionhouse',
  `buyerpriceblue` int(11) DEFAULT '12' COMMENT 'Multiplier to vendorprice when buying blue items from auctionhouse',
  `buyerpricepurple` int(11) DEFAULT '15' COMMENT 'Multiplier to vendorprice when buying purple items from auctionhouse',
  `buyerpriceorange` int(11) DEFAULT '20' COMMENT 'Multiplier to vendorprice when buying orange items from auctionhouse',
  `buyerpriceyellow` int(11) DEFAULT '22' COMMENT 'Multiplier to vendorprice when buying yellow items from auctionhouse',
  `buyerbiddinginterval` int(11) DEFAULT '1' COMMENT 'Interval how frequently AHB bids on each AH. Time in minutes',
  `buyerbidsperinterval` int(11) DEFAULT '1' COMMENT 'number of bids to put in per bidding interval',
  PRIMARY KEY (`auctionhouse`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of auctionhousebot
-- ----------------------------
INSERT INTO `auctionhousebot` VALUES ('2', 'Alliance', '0', '0', '8', '24', '0', '27', '12', '10', '1', '0', '0', '0', '10', '30', '8', '2', '0', '0', '100', '150', '150', '250', '800', '1400', '1250', '1750', '2250', '4550', '3250', '5550', '5250', '6550', '70', '100', '70', '100', '80', '100', '75', '100', '80', '100', '80', '100', '80', '100', '0', '0', '3', '2', '1', '1', '1', '1', '1', '5', '12', '15', '20', '22', '1', '1');
INSERT INTO `auctionhousebot` VALUES ('6', 'Horde', '0', '0', '8', '24', '0', '27', '12', '10', '1', '0', '0', '0', '10', '30', '8', '2', '0', '0', '100', '150', '150', '250', '800', '1400', '1250', '1750', '2250', '4550', '3250', '5550', '5250', '6550', '70', '100', '70', '100', '80', '100', '75', '100', '80', '100', '80', '100', '80', '100', '0', '0', '3', '2', '1', '1', '1', '1', '1', '5', '12', '15', '20', '22', '1', '1');
INSERT INTO `auctionhousebot` VALUES ('7', 'Neutral', '0', '0', '8', '24', '0', '27', '12', '10', '1', '0', '0', '0', '10', '30', '8', '2', '0', '0', '100', '150', '150', '250', '800', '1400', '1250', '1750', '2250', '4550', '3250', '5550', '5250', '6550', '70', '100', '70', '100', '80', '100', '75', '100', '80', '100', '80', '100', '80', '100', '0', '0', '3', '2', '1', '1', '1', '1', '1', '5', '12', '15', '20', '22', '1', '1');

ALTER TABLE character_aura
CHANGE COLUMN effect_mask `effect_index` int(11) unsigned NOT NULL DEFAULT '0',
DROP COLUMN recalculate_mask,
CHANGE COLUMN amount0 `amount` int(11) NOT NULL DEFAULT '0',
DROP COLUMN amount1,
DROP COLUMN amount2,
DROP COLUMN base_amount0,
DROP COLUMN base_amount1,
DROP COLUMN base_amount2;

ALTER TABLE character_glyphs
ADD COLUMN `slot` tinyint(3) unsigned NOT NULL DEFAULT '0' AFTER `spec`,
CHANGE COLUMN glyph1 `glyph` int(11) unsigned NOT NULL DEFAULT '0',
DROP COLUMN glyph2,
DROP COLUMN glyph3,
DROP COLUMN glyph4,
DROP COLUMN glyph5,
DROP COLUMN glyph6;

ALTER TABLE character_queststatus_daily
DROP COLUMN time;

DROP TABLE IF EXISTS `character_queststatus_weekly`;
CREATE TABLE `character_queststatus_weekly` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `quest` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Quest Identifier',
  PRIMARY KEY (`guid`,`quest`),
  KEY `idx_guid` (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

DROP TABLE IF EXISTS `character_stats`;
CREATE TABLE `character_stats` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier, Low part',
  `maxhealth` int(10) unsigned NOT NULL DEFAULT '0',
  `maxpower1` int(10) unsigned NOT NULL DEFAULT '0',
  `maxpower2` int(10) unsigned NOT NULL DEFAULT '0',
  `maxpower3` int(10) unsigned NOT NULL DEFAULT '0',
  `maxpower4` int(10) unsigned NOT NULL DEFAULT '0',
  `maxpower5` int(10) unsigned NOT NULL DEFAULT '0',
  `maxpower6` int(10) unsigned NOT NULL DEFAULT '0',
  `maxpower7` int(10) unsigned NOT NULL DEFAULT '0',
  `strength` int(10) unsigned NOT NULL DEFAULT '0',
  `agility` int(10) unsigned NOT NULL DEFAULT '0',
  `stamina` int(10) unsigned NOT NULL DEFAULT '0',
  `intellect` int(10) unsigned NOT NULL DEFAULT '0',
  `spirit` int(10) unsigned NOT NULL DEFAULT '0',
  `armor` int(10) unsigned NOT NULL DEFAULT '0',
  `resHoly` int(10) unsigned NOT NULL DEFAULT '0',
  `resFire` int(10) unsigned NOT NULL DEFAULT '0',
  `resNature` int(10) unsigned NOT NULL DEFAULT '0',
  `resFrost` int(10) unsigned NOT NULL DEFAULT '0',
  `resShadow` int(10) unsigned NOT NULL DEFAULT '0',
  `resArcane` int(10) unsigned NOT NULL DEFAULT '0',
  `blockPct` float unsigned NOT NULL DEFAULT '0',
  `dodgePct` float unsigned NOT NULL DEFAULT '0',
  `parryPct` float unsigned NOT NULL DEFAULT '0',
  `critPct` float unsigned NOT NULL DEFAULT '0',
  `rangedCritPct` float unsigned NOT NULL DEFAULT '0',
  `spellCritPct` float unsigned NOT NULL DEFAULT '0',
  `attackPower` int(10) unsigned NOT NULL DEFAULT '0',
  `rangedAttackPower` int(10) unsigned NOT NULL DEFAULT '0',
  `spellPower` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

ALTER TABLE character_talent
CHANGE COLUMN spell `talent_id` int(11) unsigned NOT NULL,
ADD COLUMN `current_rank` tinyint(3) unsigned NOT NULL DEFAULT '0' AFTER talent_id;

ALTER TABLE characters
DROP COLUMN instance_id,
CHANGE COLUMN speccount `specCount` tinyint(3) unsigned NOT NULL DEFAULT '1',
CHANGE COLUMN activespec `activeSpec` tinyint(3) unsigned NOT NULL DEFAULT '0',
ADD COLUMN `deleteInfos_Account` int(11) unsigned DEFAULT NULL AFTER actionBars,
ADD COLUMN `deleteInfos_Name` varchar(12) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL AFTER deleteInfos_Account,
ADD COLUMN `deleteDate` timestamp NULL DEFAULT NULL AFTER deleteInfos_Name,
DROP COLUMN latency;

DROP TABLE IF EXISTS `cheaters`;
CREATE TABLE `cheaters` (
  `entry` bigint(20) NOT NULL AUTO_INCREMENT,
  `player` varchar(30) NOT NULL,
  `acctid` int(11) NOT NULL,
  `reason` varchar(255) NOT NULL DEFAULT 'unknown',
  `speed` float NOT NULL DEFAULT '0',
  `Val1` float NOT NULL DEFAULT '0',
  `Val2` int(10) unsigned NOT NULL DEFAULT '0',
  `count` int(11) NOT NULL DEFAULT '0',
  `Map` smallint(5) NOT NULL DEFAULT '-1',
  `Pos` varchar(255) NOT NULL DEFAULT '0',
  `Level` mediumint(9) NOT NULL DEFAULT '0',
  `first_date` datetime NOT NULL,
  `last_date` datetime NOT NULL,
  `Op` varchar(255) NOT NULL DEFAULT 'unknown',
  PRIMARY KEY (`entry`),
  KEY `idx_Count` (`count`),
  KEY `idx_Player` (`player`)
) ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS game_event_condition_save;
DROP TABLE IF EXISTS game_event_save;
DROP TABLE IF EXISTS data_backup;

ALTER TABLE group_member
CHANGE COLUMN leaderGuid `groupId` int(11) unsigned NOT NULL,
CHANGE COLUMN memberFlags `assistant` tinyint(1) unsigned NOT NULL;

ALTER TABLE groups
ADD COLUMN `mainTank` int(11) unsigned NOT NULL AFTER leaderGuid,
ADD COLUMN `mainAssistant` int(11) unsigned NOT NULL AFTER mainTank;

DROP TABLE IF EXISTS `instance_reset`;
CREATE TABLE `instance_reset` (
  `mapid` int(11) unsigned NOT NULL DEFAULT '0',
  `difficulty` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `resettime` bigint(40) NOT NULL DEFAULT '0',
  PRIMARY KEY (`mapid`,`difficulty`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of instance_reset
-- ----------------------------
INSERT INTO `instance_reset` VALUES ('249', '0', '1272081600');
INSERT INTO `instance_reset` VALUES ('249', '1', '1272081600');
INSERT INTO `instance_reset` VALUES ('269', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('309', '0', '1271736000');
INSERT INTO `instance_reset` VALUES ('409', '0', '1272081600');
INSERT INTO `instance_reset` VALUES ('469', '0', '1272081600');
INSERT INTO `instance_reset` VALUES ('509', '0', '1271736000');
INSERT INTO `instance_reset` VALUES ('531', '0', '1272081600');
INSERT INTO `instance_reset` VALUES ('532', '0', '1272081600');
INSERT INTO `instance_reset` VALUES ('533', '0', '1272081600');
INSERT INTO `instance_reset` VALUES ('533', '1', '1272081600');
INSERT INTO `instance_reset` VALUES ('534', '0', '1272081600');
INSERT INTO `instance_reset` VALUES ('540', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('542', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('543', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('544', '0', '1272081600');
INSERT INTO `instance_reset` VALUES ('545', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('546', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('547', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('548', '0', '1272081600');
INSERT INTO `instance_reset` VALUES ('550', '0', '1272081600');
INSERT INTO `instance_reset` VALUES ('552', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('553', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('554', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('555', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('556', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('557', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('558', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('560', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('564', '0', '1272081600');
INSERT INTO `instance_reset` VALUES ('565', '0', '1272081600');
INSERT INTO `instance_reset` VALUES ('568', '0', '1271736000');
INSERT INTO `instance_reset` VALUES ('574', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('575', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('576', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('578', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('580', '0', '1272081600');
INSERT INTO `instance_reset` VALUES ('585', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('595', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('598', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('599', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('600', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('601', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('602', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('603', '0', '1272081600');
INSERT INTO `instance_reset` VALUES ('603', '1', '1272081600');
INSERT INTO `instance_reset` VALUES ('604', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('608', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('615', '0', '1272081600');
INSERT INTO `instance_reset` VALUES ('615', '1', '1272081600');
INSERT INTO `instance_reset` VALUES ('616', '0', '1272081600');
INSERT INTO `instance_reset` VALUES ('616', '1', '1272081600');
INSERT INTO `instance_reset` VALUES ('619', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('624', '0', '1272081600');
INSERT INTO `instance_reset` VALUES ('624', '1', '1272081600');
INSERT INTO `instance_reset` VALUES ('631', '0', '1272081600');
INSERT INTO `instance_reset` VALUES ('631', '1', '1272081600');
INSERT INTO `instance_reset` VALUES ('631', '2', '1272081600');
INSERT INTO `instance_reset` VALUES ('631', '3', '1272081600');
INSERT INTO `instance_reset` VALUES ('632', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('649', '0', '1272081600');
INSERT INTO `instance_reset` VALUES ('649', '1', '1272081600');
INSERT INTO `instance_reset` VALUES ('649', '2', '1272081600');
INSERT INTO `instance_reset` VALUES ('649', '3', '1272081600');
INSERT INTO `instance_reset` VALUES ('650', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('658', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('668', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('724', '0', '1271563200');
INSERT INTO `instance_reset` VALUES ('724', '1', '1271563200');
INSERT INTO `instance_reset` VALUES ('724', '2', '1271563200');
INSERT INTO `instance_reset` VALUES ('724', '3', '1271563200');

ALTER TABLE item_instance
ADD COLUMN `text` longtext AFTER data;

DROP TABLE item_refund_instance;
DROP TABLE item_text;

ALTER TABLE pet_aura
CHANGE COLUMN effect_mask `effect_index` int(11) unsigned NOT NULL DEFAULT '0',
DROP COLUMN recalculate_mask,
CHANGE COLUMN amount0 `amount` int(11) NOT NULL DEFAULT '0',
DROP COLUMN amount1,
DROP COLUMN amount2,
DROP COLUMN base_amount0,
DROP COLUMN base_amount1,
DROP COLUMN base_amount2;

DROP TABLE IF EXISTS `saved_variables`;
CREATE TABLE `saved_variables` (
  `NextArenaPointDistributionTime` bigint(40) unsigned NOT NULL DEFAULT '0',
  `NextDailyQuestResetTime` bigint(40) unsigned NOT NULL DEFAULT '0',
  `NextWeeklyQuestResetTime` bigint(40) unsigned NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Variable Saves';

-- ----------------------------
-- Records of saved_variables
-- ----------------------------
INSERT INTO `saved_variables` VALUES ('0', '1271563200', '0');
INSERT INTO `saved_variables` VALUES ('0', '0', '1271822400');
