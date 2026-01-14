/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
import { Logger } from '../common/Logger';
import { BatteryEvent, BatterySnapshot, type BatteryEventRawParams } from '../model/BatteryEvent';
import { BatteryEventParser } from '../service/BatteryEventParser';
import { BatteryEventFilter } from '../service/BatteryEventFilter';
import { PreferencesStore } from '../data/PreferencesStore';
import { RdbStore } from '../data/RdbStore';
import { Settings } from '../model/Settings';
import type common from '@ohos.app.ability.common';

export interface RepositoryListener {
  onSnapshotUpdated(snapshot: BatterySnapshot): void;

  onHistoryUpdated(events: BatteryEvent[]): void;
}

export class BatteryEventRepository {
  private static _inst: BatteryEventRepository | null = null;

  static get inst(): BatteryEventRepository {
    if (!BatteryEventRepository._inst) {
      BatteryEventRepository._inst = new BatteryEventRepository();
    }
    return BatteryEventRepository._inst;
  }

  private parser: BatteryEventParser = new BatteryEventParser();
  private filter: BatteryEventFilter = new BatteryEventFilter();
  private settings: Settings = new Settings();
  private lastSnapshot: BatterySnapshot | null = null;
  private history: BatteryEvent[] = [];
  private listeners: Set<RepositoryListener> = new Set();
  private inited: boolean = false;
  private initPromise: Promise<void> | null = null;
  private useRdb: boolean = true;

  addListener(l: RepositoryListener): void {
    this.listeners.add(l);
  }

  removeListener(l: RepositoryListener): void {
    this.listeners.delete(l);
  }

  getSettings(): Settings {
    return this.settings;
  }

  getLastSnapshot(): BatterySnapshot | null {
    return this.lastSnapshot ? this.lastSnapshot.clone() : null;
  }

  getHistory(): BatteryEvent[] {
    return this.history.slice();
  }

  ensureInit(context: common.Context): Promise<void> {
    if (this.inited) {
      return Promise.resolve();
    }
    if (this.initPromise) {
      return this.initPromise;
    }

    this.initPromise = this.initInternal(context)
      .then(() => {
        this.inited = true;
      })
      .catch((e: Object) => {
        this.initPromise = null;
        Logger.e('Repository init failed', e);
        throw new Error('Repository init failed');
      });

    return this.initPromise;
  }

  async init(context: common.Context): Promise<void> {
    await this.ensureInit(context);
  }

  private async initInternal(context: common.Context): Promise<void> {
    await PreferencesStore.inst.init(context);

    if (this.useRdb) {
      try {
        await RdbStore.inst.init(context);
      } catch (e) {
        Logger.w('RDB init failed; fallback to prefs only', e as Object);
        this.useRdb = false;
      }
    }

    this.settings = await PreferencesStore.inst.loadSettings();
    const last = await PreferencesStore.inst.loadSnapshot();
    if (last) {
      this.lastSnapshot = last;
    }

    const cache = await PreferencesStore.inst.loadHistoryCache();
    if (cache.length > 0) {
      this.history = cache;
    }

    this.notifyHistory();
    if (this.lastSnapshot) {
      this.notifySnapshot(this.lastSnapshot);
    }

    Logger.i('Repository init done, history=%{public}d', this.history.length);
  }

  async updateSettings(newSettings: Settings): Promise<void> {
    this.settings = newSettings;
    await PreferencesStore.inst.saveSettings(newSettings);

    if (this.history.length > newSettings.maxHistory) {
      this.history = this.history.slice(0, newSettings.maxHistory);
      await PreferencesStore.inst.saveHistoryCache(this.history);
      this.notifyHistory();
    }
  }

  async onBatteryCommonEvent(raw: BatteryEventRawParams): Promise<void> {
    const snap = this.parser.parse(raw);
    const decision = this.filter.shouldRecord(this.settings, snap, this.lastSnapshot);

    this.lastSnapshot = snap.clone();
    this.notifySnapshot(this.lastSnapshot);
    await PreferencesStore.inst.saveSnapshot(this.lastSnapshot);

    if (!decision.accept) {
      Logger.d('Event dropped: %{public}s', decision.reason);
      return;
    }

    const id = `${snap.timeMs}_${Math.floor(Math.random() * 1000000)}`;
    const e = new BatteryEvent(id, snap.timeMs, snap.clone(), raw);

    this.history.unshift(e);
    if (this.history.length > this.settings.maxHistory) {
      this.history.pop();
    }

    await PreferencesStore.inst.saveHistoryCache(this.history);

    if (this.useRdb) {
      await RdbStore.inst.insertEvent(e);
    }

    this.notifyHistory();
    Logger.i('Recorded event, reason=%{public}s, history=%{public}d', decision.reason, this.history.length);
  }

  private notifySnapshot(s: BatterySnapshot): void {
    this.listeners.forEach(l => {
      try {
        l.onSnapshotUpdated(s);
      } catch (e) {
        Logger.w('listener snapshot failed', e as Object);
      }
    });
  }

  private notifyHistory(): void {
    const copy = this.history.slice();
    this.listeners.forEach(l => {
      try {
        l.onHistoryUpdated(copy);
      } catch (e) {
        Logger.w('listener history failed', e as Object);
      }
    });
  }
}
