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

export enum ChargeState {
  UNKNOWN = 0,
  CHARGING = 1,
  DISCHARGING = 2,
  FULL = 3
}

export enum HealthState {
  UNKNOWN = 0,
  GOOD = 1,
  OVERHEAT = 2,
  DEAD = 3,
  OVERVOLTAGE = 4,
  UNSPECIFIED_FAILURE = 5,
  COLD = 6
}

export enum PluggedType {
  NONE = 0,
  AC = 1,
  USB = 2,
  WIRELESS = 3
}

export function chargeStateText(v: number): string {
  switch (v) {
    case ChargeState.DISCHARGING:
      return 'Discharging';
    case ChargeState.CHARGING:
      return 'Charging';
    case ChargeState.FULL:
      return 'Full';
    default:
      return 'Unknown';
  }
}

export function healthStateText(v: number): string {
  switch (v) {
    case HealthState.GOOD:
      return 'Good';
    case HealthState.OVERHEAT:
      return 'Overheat';
    case HealthState.DEAD:
      return 'Dead';
    case HealthState.OVERVOLTAGE:
      return 'Overvoltage';
    case HealthState.UNSPECIFIED_FAILURE:
      return 'Failure';
    case HealthState.COLD:
      return 'Cold';
    default:
      return 'Unknown';
  }
}

export function pluggedTypeText(v: number): string {
  switch (v) {
    case PluggedType.AC:
      return 'AC';
    case PluggedType.USB:
      return 'USB';
    case PluggedType.WIRELESS:
      return 'Wireless';
    default:
      return 'None';
  }
}
