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