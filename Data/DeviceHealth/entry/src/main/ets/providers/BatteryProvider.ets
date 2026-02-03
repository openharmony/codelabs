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


import batteryInfo from '@ohos.batteryInfo';

export type BatteryCallback = (soc: number) => void;

export interface IBatteryProvider {
  getSOC(): Promise<number | null>;
  subscribe(cb: BatteryCallback): Promise<void>;
  unsubscribe(): Promise<void>;
}

type TimerHandle = number;

export class DefaultBatteryProvider implements IBatteryProvider {
  private cb: BatteryCallback | null = null;
  private timer: TimerHandle | null = null;

  async getSOC(): Promise<number | null> {
    // ArkTS 允许：明确字段、明确类型
    const soc: number = batteryInfo.batterySOC;
    return soc >= 0 && soc <= 100 ? soc : null;
  }

  async subscribe(cb: BatteryCallback): Promise<void> {
    this.cb = cb;

    if (this.timer !== null) {
      return;
    }

    // ✅ 不做任何 any / unknown / as 转换
    this.timer = setInterval((): void => {
      this.getSOC()
        .then((soc: number | null): void => {
          if (soc !== null && this.cb !== null) {
            this.cb(soc);
          }
        })
        .catch((_e: Error): void => {});
    }, 2_000);
  }

  async unsubscribe(): Promise<void> {
    if (this.timer !== null) {
      clearInterval(this.timer);
      this.timer = null;
    }
    this.cb = null;
  }
}
