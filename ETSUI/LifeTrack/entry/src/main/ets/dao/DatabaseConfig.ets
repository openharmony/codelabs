/*
 * Copyright (c) 2026 BIT-Group23
 * Licensed under the Apache License, Version 2.0 (the");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-20
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// DatabaseConfig.ets
import relationalStore from '@ohos.data.relationalStore';

// 数据库配置
export const DATABASE_CONFIG: relationalStore.StoreConfig = {
  name: 'CalendarApp.db',
  securityLevel: relationalStore.SecurityLevel.S1
};

// 日程表SQL
export const SQL_CREATE_EVENTS_TABLE = `
  CREATE TABLE IF NOT EXISTS calendar_events (
    id TEXT PRIMARY KEY,
    title TEXT NOT NULL,
    description TEXT,
    start_time INTEGER NOT NULL,
    end_time INTEGER NOT NULL,
    all_day INTEGER DEFAULT 0,
    location TEXT,
    calendar_id TEXT,
    color TEXT,
    reminder INTEGER DEFAULT 0,
    repeat_rule TEXT,
    completed INTEGER DEFAULT 0,
    created_time INTEGER NOT NULL,
    updated_time INTEGER NOT NULL
  )
`;

// 创建索引提高查询性能
export const SQL_CREATE_INDEXES = [
  `CREATE INDEX IF NOT EXISTS idx_start_time ON calendar_events(start_time)`,
  `CREATE INDEX IF NOT EXISTS idx_calendar_id ON calendar_events(calendar_id)`,
  `CREATE INDEX IF NOT EXISTS idx_completed ON calendar_events(completed)`
];

// 步数记录表SQL
export const SQL_CREATE_STEP_TABLE = `
  CREATE TABLE IF NOT EXISTS step_records (
    id TEXT PRIMARY KEY,
    date TEXT NOT NULL UNIQUE,  -- 日期唯一，每天只能有一条记录
    steps INTEGER DEFAULT 0,
    calories REAL DEFAULT 0.0,
    distance REAL DEFAULT 0.0,
    created_time INTEGER NOT NULL,
    updated_time INTEGER NOT NULL
  )
`;

// 步数记录表索引
export const SQL_CREATE_STEP_INDEXES = [
  `CREATE INDEX IF NOT EXISTS idx_step_date ON step_records(date)`,
  `CREATE INDEX IF NOT EXISTS idx_steps ON step_records(steps)`
];