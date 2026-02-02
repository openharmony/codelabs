import relationalStore from '@ohos.data.relationalStore';
import type common from '@ohos.app.ability.common';
import { EventLog, EventLevel, EventType } from '../model/EventLog';
import { stablePayloadString } from '../model/EventLog';

export interface QueryOptions {
  type?: EventType;
  sinceTs?: number;
  limit?: number;
  offset?: number;
}

type PayloadObject = Record<string, string | number | boolean | null>;

function toEventLevel(value: string): EventLevel {
  switch (value) {
    case EventLevel.INFO:
      return EventLevel.INFO;
    case EventLevel.WARN:
      return EventLevel.WARN;
    case EventLevel.ERROR:
      return EventLevel.ERROR;
    default:
      return EventLevel.INFO;
  }
}

function toEventType(value: string): EventType {
  switch (value) {
    case EventType.BATTERY:
      return EventType.BATTERY;
    case EventType.NETWORK:
      return EventType.NETWORK;
    case EventType.STORAGE:
      return EventType.STORAGE;
    case EventType.SCREEN:
      return EventType.SCREEN;
    default:
      return EventType.BATTERY;
  }
}

export class EventDB {
  private static store: relationalStore.RdbStore | null = null;
  private static readonly DB_NAME = 'event_logs.db';
  private static readonly TABLE = 'event_logs';

  static async init(ctx: common.Context): Promise<void> {
    if (EventDB.store !== null) {
      return;
    }

    const config: relationalStore.StoreConfig = {
      name: EventDB.DB_NAME,
      securityLevel: relationalStore.SecurityLevel.S1,
    };

    const store = await new Promise<relationalStore.RdbStore>((resolve, reject) => {
      relationalStore.getRdbStore(
        ctx,
        config,
        (err: Error | null, rdbStore: relationalStore.RdbStore) => {
          if (err) {
            reject(err);
          } else {
            resolve(rdbStore);
          }
        }
      );
    });

    EventDB.store = store;
    await EventDB.createTableIfNeeded();
  }

  static isReady(): boolean {
    return EventDB.store !== null;
  }

  static async createTableIfNeeded(): Promise<void> {
    if (!EventDB.store) {
      throw new Error('EventDB not initialized');
    }

    const createSql = `
      CREATE TABLE IF NOT EXISTS ${EventDB.TABLE} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        type TEXT NOT NULL,
        level TEXT NOT NULL,
        timestamp INTEGER NOT NULL,
        message TEXT NOT NULL,
        payload TEXT,
        source TEXT
      )
    `;
    const indexSql = `CREATE INDEX IF NOT EXISTS idx_event_ts ON ${EventDB.TABLE}(timestamp)`;

    await EventDB.store.executeSql(createSql);
    await EventDB.store.executeSql(indexSql);
  }

  static async insert(event: EventLog): Promise<number> {
    if (!EventDB.store) {
      throw new Error('EventDB not initialized');
    }

    const values: relationalStore.ValuesBucket = {
      type: event.type,
      level: event.level,
      timestamp: event.timestamp,
      message: event.message,
      payload: stablePayloadString(event.payload as PayloadObject | undefined),
      source: event.source ?? '',
    };

    const rowId: number = await EventDB.store.insert(EventDB.TABLE, values);
    return rowId;
  }

  static async query(opts: QueryOptions = {}): Promise<EventLog[]> {
    if (!EventDB.store) {
      throw new Error('EventDB not initialized');
    }

    const predicates = new relationalStore.RdbPredicates(EventDB.TABLE);

    if (opts.type !== undefined) {
      predicates.equalTo('type', opts.type);
    }
    if (opts.sinceTs !== undefined) {
      predicates.greaterThan('timestamp', opts.sinceTs);
    }

    predicates.orderByDesc('timestamp');

    if (opts.limit !== undefined) {
      predicates.limitAs(opts.limit);
    }
    if (opts.offset !== undefined) {
      predicates.offsetAs(opts.offset);
    }

    const rs = await EventDB.store.query(predicates, [
      'id',
      'type',
      'level',
      'timestamp',
      'message',
      'payload',
      'source',
    ]);

    const result: EventLog[] = [];

    try {
      while (rs.goToNextRow()) {
        const payloadStr: string = rs.getString(rs.getColumnIndex('payload'));
        let payload: PayloadObject | undefined = undefined;

        if (payloadStr !== '') {
          try {
            payload = JSON.parse(payloadStr) as PayloadObject;
          } catch (_) {
            payload = undefined;
          }
        }

        result.push({
          id: rs.getLong(rs.getColumnIndex('id')),
          type: toEventType(rs.getString(rs.getColumnIndex('type'))),
          level: toEventLevel(rs.getString(rs.getColumnIndex('level'))),
          timestamp: rs.getLong(rs.getColumnIndex('timestamp')),
          message: rs.getString(rs.getColumnIndex('message')),
          payload,
          source: rs.getString(rs.getColumnIndex('source')) || undefined,
        });
      }
    } finally {
      rs.close();
    }

    return result;
  }

  static async deleteOlderThan(ts: number): Promise<number> {
    if (!EventDB.store) {
      throw new Error('EventDB not initialized');
    }

    const predicates = new relationalStore.RdbPredicates(EventDB.TABLE);
    predicates.lessThan('timestamp', ts);

    const deleted: number = await EventDB.store.delete(predicates);
    return deleted;
  }
}
