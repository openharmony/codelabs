import type { AudioRecord } from '../model/AudioRecord';

export type RecordStoreListener = (records: AudioRecord[]) => void;

class RecordStore {
  private records: AudioRecord[] = [];
  private listeners: Set<RecordStoreListener> = new Set();

  getRecords(): AudioRecord[] {
    return this.records;
  }

  addRecord(record: AudioRecord): void {
    this.records = [record, ...this.records];
    this.notify();
  }

  removeRecord(id: string): void {
    this.records = this.records.filter(item => item.id !== id);
    this.notify();
  }

  updateName(id: string, name: string): void {
    let changed = false;
    const updatedRecords: AudioRecord[] = this.records.map((item) => {
      if (item.id !== id) {
        return item;
      }
      if (item.name === name) {
        return item;
      }
      changed = true;
      const updated: AudioRecord = {
        id: item.id,
        name: name,
        path: item.path,
        durationMs: item.durationMs,
        sizeBytes: item.sizeBytes,
        createdAt: item.createdAt,
        format: item.format,
        sampleRate: item.sampleRate,
        channels: item.channels
      };
      return updated;
    });
    if (changed) {
      this.records = updatedRecords;
      this.notify();
    }
  }

  clear(): void {
    this.records = [];
    this.notify();
  }

  subscribe(listener: RecordStoreListener): () => void {
    this.listeners.add(listener);
    listener(this.records);
    return () => this.listeners.delete(listener);
  }

  private notify(): void {
    this.listeners.forEach(listener => listener(this.records));
  }
}

export const recordStore = new RecordStore();
