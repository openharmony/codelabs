/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
// File: entry/src/main/ets/common/database/Tables.ts

// 表名常量
export const TABLE_CONTACT = 'contact';
export const TABLE_EVENT = 'event';

// 联系人建表语句
// ✅ 优化：根据项目文档 F6/F10 需求，补充了 privacy 相关的字段 (is_secret, image_uri)
// 这样您在测试隐私锁功能时就不需要再次卸载应用了
export const SQL_CREATE_CONTACT = `
  CREATE TABLE IF NOT EXISTS ${TABLE_CONTACT} (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    phone TEXT,
    email TEXT,
    relation TEXT,
    address TEXT,
    remark TEXT,
    image_uri TEXT,
    is_secret INTEGER DEFAULT 0
  )
`;

// 日程建表语句
export const SQL_CREATE_EVENT = `
  CREATE TABLE IF NOT EXISTS ${TABLE_EVENT} (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    title TEXT NOT NULL,
    description TEXT,
    start_time INTEGER,
    end_time INTEGER,
    is_all_day INTEGER,
    reminder_id INTEGER DEFAULT -1
  )
`;
// 👆 ✅ 核心修复：增加了 reminder_id 字段