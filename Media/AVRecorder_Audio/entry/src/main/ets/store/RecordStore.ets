/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
