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
import { ChargeState, HealthState, PluggedType } from './BatteryEnums';

export interface BatteryEventRawParams {
  chargeState?: number;
  healthState?: number;
  pluggedType?: number;
  present?: boolean;
  soc?: number;
  technology?: string;
  temperature?: number; // tenths of degree C (often)
  voltage?: number;
  moduleName?: string;
  uevent?: string;
}

class RawParamsImpl implements BatteryEventRawParams {
  // Empty implementation to satisfy arkts-no-untyped-obj-literals when we need a default object.
}

export interface BatterySnapshotJson {
  timeMs: number;
  soc: number;
  chargeState: number;
  healthState: number;
  pluggedType: number;
  temperatureC: number;
  voltageV: number;
  technology: string;
  present: boolean;
}

export interface BatteryEventJson {
  id: string;
  timeMs: number;
  snapshot: BatterySnapshotJson;
  raw: BatteryEventRawParams;
}

export class BatterySnapshot {
  public timeMs: number = Date.now();
  public soc: number = -1;
  public chargeState: ChargeState = ChargeState.UNKNOWN;
  public healthState: HealthState = HealthState.UNKNOWN;
  public pluggedType: PluggedType = PluggedType.NONE;
  public temperatureC: number = NaN;
  public voltageV: number = NaN;
  public technology: string = '';
  public present: boolean = true;

  public clone(): BatterySnapshot {
    const s = new BatterySnapshot();
    s.timeMs = this.timeMs;
    s.soc = this.soc;
    s.chargeState = this.chargeState;
    s.healthState = this.healthState;
    s.pluggedType = this.pluggedType;
    s.temperatureC = this.temperatureC;
    s.voltageV = this.voltageV;
    s.technology = this.technology;
    s.present = this.present;
    return s;
  }

  public toJson(): BatterySnapshotJson {
    return {
      timeMs: this.timeMs,
      soc: this.soc,
      chargeState: this.chargeState,
      healthState: this.healthState,
      pluggedType: this.pluggedType,
      temperatureC: this.temperatureC,
      voltageV: this.voltageV,
      technology: this.technology,
      present: this.present
    };
  }

  public static fromJson(obj: BatterySnapshotJson | null): BatterySnapshot {
    const s = new BatterySnapshot();
    if (!obj) {
      return s;
    }

    if (typeof obj.timeMs === 'number') {
      s.timeMs = obj.timeMs;
    }
    if (typeof obj.soc === 'number') {
      s.soc = obj.soc;
    }
    if (typeof obj.chargeState === 'number') {
      s.chargeState = obj.chargeState as ChargeState;
    }
    if (typeof obj.healthState === 'number') {
      s.healthState = obj.healthState as HealthState;
    }
    if (typeof obj.pluggedType === 'number') {
      s.pluggedType = obj.pluggedType as PluggedType;
    }
    if (typeof obj.temperatureC === 'number') {
      s.temperatureC = obj.temperatureC;
    }
    if (typeof obj.voltageV === 'number') {
      s.voltageV = obj.voltageV;
    }
    if (typeof obj.technology === 'string') {
      s.technology = obj.technology;
    }
    if (typeof obj.present === 'boolean') {
      s.present = obj.present;
    }

    return s;
  }
}

export class BatteryEvent {
  public id: string;
  public timeMs: number;
  public snapshot: BatterySnapshot;
  public raw: BatteryEventRawParams;

  public constructor(id: string, timeMs: number, snapshot: BatterySnapshot, raw: BatteryEventRawParams) {
    this.id = id;
    this.timeMs = timeMs;
    this.snapshot = snapshot;
    this.raw = raw;
  }

  public toJson(): BatteryEventJson {
    return {
      id: this.id,
      timeMs: this.timeMs,
      snapshot: this.snapshot.toJson(),
      raw: this.raw
    };
  }

  public static fromJson(obj: BatteryEventJson | null): BatteryEvent | null {
    if (!obj) {
      return null;
    }
    if (typeof obj.id !== 'string' || typeof obj.timeMs !== 'number') {
      return null;
    }

    const snap = BatterySnapshot.fromJson(obj.snapshot);
    const raw: BatteryEventRawParams = obj.raw ? obj.raw : new RawParamsImpl();
    return new BatteryEvent(obj.id, obj.timeMs, snap, raw);
  }
}
