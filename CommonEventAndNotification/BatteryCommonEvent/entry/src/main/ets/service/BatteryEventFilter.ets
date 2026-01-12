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
import { RecordMode, Settings } from '../model/Settings';
import type { BatterySnapshot } from '../model/BatteryEvent';

export interface FilterResult {
  accept: boolean;
  reason: string;
}

export class BatteryEventFilter {
  shouldRecord(settings: Settings, current: BatterySnapshot, last: BatterySnapshot | null): FilterResult {
    if (!settings) {
      return { accept: true, reason: 'no settings' };
    }

    if (settings.recordMode === RecordMode.SOC_ONLY) {
      if (!last) {
        return { accept: true, reason: 'first event' };
      }
      if (current.soc !== last.soc) {
        return { accept: true, reason: 'soc changed' };
      }
      return { accept: false, reason: 'soc unchanged' };
    }

    if (settings.recordMode === RecordMode.PLUG_ONLY) {
      if (!last) {
        return { accept: true, reason: 'first event' };
      }
      if (current.pluggedType !== last.pluggedType) {
        return { accept: true, reason: 'plug changed' };
      }
      return { accept: false, reason: 'plug unchanged' };
    }

    if (settings.thresholds.enabled) {
      const t = settings.thresholds;

      if (last && Math.abs(current.soc - last.soc) >= t.minSocDelta) {
        return { accept: true, reason: `soc delta >= ${t.minSocDelta}` };
      }
      if (isFinite(current.temperatureC) && current.temperatureC >= t.maxTempC) {
        return { accept: true, reason: `temp >= ${t.maxTempC}` };
      }
      if (isFinite(current.voltageV) && current.voltageV <= t.minVoltV) {
        return { accept: true, reason: `volt <= ${t.minVoltV}` };
      }

      if (last && (current.chargeState !== last.chargeState ||
        current.healthState !== last.healthState ||
        current.pluggedType !== last.pluggedType)) {
        return { accept: true, reason: 'state changed' };
      }
      return { accept: false, reason: 'threshold not met' };
    }

    return { accept: true, reason: 'record all' };
  }
}
