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

import type { IBatteryProvider } from '../../providers/BatteryProvider';
import { DefaultBatteryProvider } from '../../providers/BatteryProvider';
import { EventLevel, EventLog, EventType, nowMs } from '../../model/EventLog';

export type Emit = (e: EventLog) => void;

/** 显式 payload 类型（避免 any / untyped object literal） */
export interface BatteryPayload {
  soc: number;
  threshold?: number;
}

/** 显式 payload 实现类 */
export class BatteryPayloadImpl implements BatteryPayload {
  soc: number;
  threshold?: number;

  constructor(soc: number, threshold?: number) {
    this.soc = soc;
    this.threshold = threshold;
  }
}

export class BatteryMonitor {
  private provider: IBatteryProvider;
  private lowPercent: number;
  private emit: Emit | null = null;
  private lastSoc: number | null = null;

  constructor(lowPercent: number = 20, provider?: IBatteryProvider) {
    this.lowPercent = lowPercent;
    this.provider = provider ?? new DefaultBatteryProvider();
  }

  async start(emit: Emit): Promise<void> {
    this.emit = emit;

    const soc: number | null = await this.provider.getSOC();
    if (soc !== null) {
      this.lastSoc = soc;
    }

    await this.provider.subscribe((newSoc: number): void => {
      this.onSoc(newSoc);
    });
  }

  async stop(): Promise<void> {
    await this.provider.unsubscribe();
    this.emit = null;
    this.lastSoc = null;
  }

  private onSoc(soc: number): void {
    const prev: number | null = this.lastSoc;
    this.lastSoc = soc;

    if (prev === null) {
      this.emitEvent(
        EventLevel.INFO,
        `电量：${soc}%`,
        new BatteryPayloadImpl(soc)
      );
      return;
    }

    // 只有穿越阈值才 WARN
    if (prev >= this.lowPercent && soc < this.lowPercent) {
      this.emitEvent(
        EventLevel.WARN,
        `低电量预警：${soc}%`,
        new BatteryPayloadImpl(soc, this.lowPercent)
      );
    } else {
      this.emitEvent(
        EventLevel.INFO,
        `电量变化：${soc}%`,
        new BatteryPayloadImpl(soc)
      );
    }
  }

  private emitEvent(level: EventLevel, message: string, payload: BatteryPayload): void {
    if (this.emit === null) {
      return;
    }

    const event: EventLog = {
      type: EventType.BATTERY,
      level,
      timestamp: nowMs(),
      message,
      payload,
      source: 'BatteryMonitor',
    };

    this.emit(event);
  }
}
