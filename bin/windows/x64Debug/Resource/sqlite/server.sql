/*
Navicat SQLite Data Transfer

Source Server         : MySQLite
Source Server Version : 30714
Source Host           : :0

Target Server Type    : SQLite
Target Server Version : 30714
File Encoding         : 65001

Date: 2016-10-31 10:24:22
*/

PRAGMA foreign_keys = OFF;

-- ----------------------------
-- Table structure for server
-- ----------------------------
DROP TABLE IF EXISTS "main"."server";
CREATE TABLE "server" (
"server_id"  INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
"realmd"  INTEGER DEFAULT 0,
"version"  INTEGER,
"server_type"  INTEGER,
"workload"  INTEGER,
"ipAddr"  TEXT(128),
"port"  INTEGER
);
