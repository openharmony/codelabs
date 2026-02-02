import type { EventLog } from '../model/EventLog';
import { stablePayloadString } from '../model/EventLog';
import { EventDB } from '../database/EventDB';

export interface IEventStore {
  insert(e: EventLog): Promise<number>;
}

class DBEventStore implements IEventStore {
  async insert(e: EventLog): Promise<number> {
    return EventDB.insert(e);
  }
}

export interface LoggerOptions {
  dedupWindowMs?: number; // 同 key 在窗口内只记录一次
  maxQueueSize?: number;
}

export class EventLogger {
  private store: IEventStore;
  private opt: Required<LoggerOptions>;
  private queue: EventLog[] = [];
  private flushing = false;
  private lastSeen: Map<string, number> = new Map();

  constructor(store?: IEventStore, opt?: LoggerOptions) {
    this.store = store ?? new DBEventStore();
    this.opt = {
      dedupWindowMs: opt?.dedupWindowMs ?? 30_000,
      maxQueueSize: opt?.maxQueueSize ?? 1000,
    };
  }

  async log(e: EventLog): Promise<void> {
    if (!e || !e.type || !e.level || !e.timestamp || !e.message) return;

    const key = this.keyOf(e);
    const last = this.lastSeen.get(key);
    if (last !== undefined && (e.timestamp - last) <= this.opt.dedupWindowMs) return;
    this.lastSeen.set(key, e.timestamp);

    if (this.queue.length >= this.opt.maxQueueSize) this.queue.shift();
    this.queue.push(e);

    await this.flush();
  }

  private keyOf(e: EventLog): string {
    return `${e.type}|${e.level}|${stablePayloadString(e.payload)}`;
  }

  private async flush(): Promise<void> {
    if (this.flushing) return;
    this.flushing = true;
    try {
      while (this.queue.length > 0) {
        const item = this.queue.shift()!;
        await this.store.insert(item);
      }
    } finally {
      this.flushing = false;
    }
  }
}
