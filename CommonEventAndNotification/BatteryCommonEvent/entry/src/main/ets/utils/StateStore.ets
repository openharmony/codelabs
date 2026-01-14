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

import { Settings } from '../model/Settings';
import type { BatteryEvent, BatterySnapshot } from '../model/BatteryEvent';

export class StateStore {
  // A very small in-memory store that can be used by pages/components without tight coupling to repository.
  private static _inst: StateStore | null = null;

  static get inst(): StateStore {
    if (!StateStore._inst) {
      StateStore._inst = new StateStore()
    }
    ;
    return StateStore._inst;
  }

  private settings: Settings = new Settings();
  private snapshot: BatterySnapshot | null = null;
  private history: BatteryEvent[] = [];

  setSettings(s: Settings): void {
    this.settings = s;
  }

  getSettings(): Settings {
    return this.settings;
  }

  setSnapshot(s: BatterySnapshot | null): void {
    this.snapshot = s;
  }

  getSnapshot(): BatterySnapshot | null {
    return this.snapshot ? this.snapshot.clone() : null;
  }

  setHistory(h: BatteryEvent[]): void {
    this.history = h.slice();
  }

  getHistory(): BatteryEvent[] {
    return this.history.slice();
  }
}
