/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
export class CommonConstants {
  /**
   * 数据库配置
   */
  static readonly DATABASE_NAME = 'HabitTracker.db';
  static readonly HABIT_TABLE_NAME = 'habit';
  static readonly RECORD_TABLE_NAME = 'record';
  static readonly CATEGORY_TABLE_NAME = 'category';
  // 🆕 同学C新增：成就表名
  static readonly ACHIEVEMENT_TABLE_NAME = 'achievement';
  /**
   * 建表 SQL 语句
   * 包含字段: id (主键), name (名称), color (颜色), targetCount (目标次数)
   */
  static readonly CREATE_TABLE_SQL = `
    CREATE TABLE IF NOT EXISTS habit (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      name TEXT NOT NULL,
      color TEXT,
      targetCount INTEGER,
      categoryId INTEGER,
      reminderTime TEXT,  -- 🆕 新增：提醒时间字符串 (如 "08:00")
      reminderId INTEGER  -- 🆕 新增：系统提醒ID (用于取消)
    )
  `;
  static readonly CREATE_CATEGORY_TABLE_SQL = `
    CREATE TABLE IF NOT EXISTS category (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      name TEXT NOT NULL,
      color TEXT
    )
  `;
  /**
   * 页面标题
   */
  static readonly HOME_TITLE = '我的习惯';
  /**
   * 🆕 修改后的打卡记录表
   * 新增 mood (心情指数 1-4) 和 note (心得文字)
   */
  static readonly CREATE_RECORD_TABLE_SQL = `
    CREATE TABLE IF NOT EXISTS record (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      habitId INTEGER NOT NULL,
      createTime INTEGER NOT NULL,
      mood INTEGER DEFAULT 1,
      note TEXT
    )
  `;
  // 🆕 同学C新增：成就表建表语句
  // 字段说明:
  // icon: 勋章图标资源名
  // unlockCondition: 解锁逻辑标识 (如 total_10, streak_7)
  // isUnlocked: 0未解锁, 1已解锁
  static readonly CREATE_ACHIEVEMENT_TABLE_SQL = `
    CREATE TABLE IF NOT EXISTS achievement (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      name TEXT NOT NULL,
      icon TEXT,
      description TEXT,
      unlockCondition TEXT,
      isUnlocked INTEGER DEFAULT 0,
      unlockTime INTEGER DEFAULT 0
    )
  `;
}