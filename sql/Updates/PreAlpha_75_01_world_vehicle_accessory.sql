/*
Navicat MySQL Data Transfer

Source Server         : Local
Source Server Version : 50141
Source Host           : localhost:3306
Source Database       : world

Target Server Type    : MYSQL
Target Server Version : 50141
File Encoding         : 65001

Date: 2010-05-03 19:47:26
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for `vehicle_accessory`
-- ----------------------------
DROP TABLE IF EXISTS `vehicle_accessory`;
CREATE TABLE `vehicle_accessory` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `accessory_entry` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `seat_id` tinyint(1) NOT NULL DEFAULT '0',
  `minion` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `description` text NOT NULL,
  PRIMARY KEY (`entry`,`seat_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;

-- ----------------------------
-- Records of vehicle_accessory
-- ----------------------------
INSERT INTO `vehicle_accessory` VALUES ('28782', '28768', '0', '0', 'Acherus Deathcharger');
INSERT INTO `vehicle_accessory` VALUES ('28312', '28319', '7', '1', 'Wintergrasp Siege Engine');
INSERT INTO `vehicle_accessory` VALUES ('32627', '32629', '7', '1', 'Wintergrasp Siege Engine');
INSERT INTO `vehicle_accessory` VALUES ('32930', '32933', '0', '1', 'Kologarn');
INSERT INTO `vehicle_accessory` VALUES ('32930', '32934', '1', '1', 'Kologarn');
INSERT INTO `vehicle_accessory` VALUES ('33109', '33167', '1', '1', 'Salvaged Demolisher');
INSERT INTO `vehicle_accessory` VALUES ('33060', '33067', '7', '1', 'Salvaged Siege Engine');
INSERT INTO `vehicle_accessory` VALUES ('33113', '33114', '0', '1', 'Flame Leviathan');
INSERT INTO `vehicle_accessory` VALUES ('33113', '33114', '1', '1', 'Flame Leviathan');
INSERT INTO `vehicle_accessory` VALUES ('33113', '33114', '2', '1', 'Flame Leviathan');
INSERT INTO `vehicle_accessory` VALUES ('33113', '33114', '3', '1', 'Flame Leviathan');
INSERT INTO `vehicle_accessory` VALUES ('33113', '33139', '7', '1', 'Flame Leviathan');
INSERT INTO `vehicle_accessory` VALUES ('33114', '33142', '1', '1', 'Overload Control Device');
INSERT INTO `vehicle_accessory` VALUES ('33114', '33143', '2', '1', 'Leviathan Defense Turret');
INSERT INTO `vehicle_accessory` VALUES ('33214', '33218', '1', '1', 'Mechanolift 304-A');
INSERT INTO `vehicle_accessory` VALUES ('35637', '34705', '0', '0', 'Marshal Jacob Alerius\' Mount');
INSERT INTO `vehicle_accessory` VALUES ('35633', '34702', '0', '0', 'Ambrose Boltspark\'s Mount');
INSERT INTO `vehicle_accessory` VALUES ('35768', '34701', '0', '0', 'Colosos\' Mount');
INSERT INTO `vehicle_accessory` VALUES ('34658', '34657', '0', '0', 'Jaelyne Evensong\'s Mount');
INSERT INTO `vehicle_accessory` VALUES ('35636', '34703', '0', '0', 'Lana Stouthammer\'s Mount');
INSERT INTO `vehicle_accessory` VALUES ('35638', '35572', '0', '0', 'Mokra the Skullcrusher\'s Mount');
INSERT INTO `vehicle_accessory` VALUES ('35635', '35569', '0', '0', 'Eressea Dawnsinger\'s Mount');
INSERT INTO `vehicle_accessory` VALUES ('35640', '35571', '0', '0', 'Runok Wildmane\'s Mount');
INSERT INTO `vehicle_accessory` VALUES ('35641', '35570', '0', '0', 'Zul\'tore\'s Mount');
INSERT INTO `vehicle_accessory` VALUES ('35634', '35617', '0', '0', 'Deathstalker Visceri\'s Mount');
