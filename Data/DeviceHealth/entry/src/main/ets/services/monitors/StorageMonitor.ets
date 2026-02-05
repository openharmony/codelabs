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

import type { IStorageProvider, StorageStatus } from '../../providers/StorageProvider';
import { DefaultStorageProvider } from '../../providers/StorageProvider';
import { EventLevel, EventLog, EventType, nowMs } from '../../model/EventLog';
import type { Emit } from './BatteryMonitor';

type TimerHandle = number;

export interface StoragePayload {
  freeBytes?: number;
  thresholdBytes?: number;
  reason?: string;
}

export class StoragePayloadImpl implements StoragePayload {
  freeBytes?: number;
  thresholdBytes?: number;
  reason?: string;

  constructor(freeBytes?: number, thresholdBytes?: number, reason?: string) {
    this.freeBytes = freeBytes;
    this.thresholdBytes = thresholdBytes;
    this.reason = reason;
  }
}

export class StorageMonitor {
  private provider: IStorageProvider;

  /** 用户设置的阈值（先写死也可以） */
  private lowBytes: number;

  private intervalMs: number;
  private emit: Emit | null = null;
  private timer: TimerHandle | null = null;

  /** 记录上一次是否 low，用于“跨越阈值才打印” */
  private lastLow: boolean | null = null;

  constructor(
    lowBytes: number = 4 * 1024 * 1024 * 1024,
    intervalMs: number = 5_000,                  // 5s 检查一次
    provider?: IStorageProvider
  ) {
    this.lowBytes = lowBytes;
    this.intervalMs = intervalMs;
    this.provider = provider ?? new DefaultStorageProvider();
  }

  async start(emit: Emit): Promise<void> {
    this.emit = emit;

    // 第一次 tick：立即根据当前状态输出（不会被 baseline 吃掉）
    await this.tick(true);

    if (this.timer === null) {
      this.timer = setInterval((): void => {
        this.tick(false).catch((): void => {});
      }, this.intervalMs);
    }
  }

  async stop(): Promise<void> {
    if (this.timer !== null) {
      clearInterval(this.timer);
      this.timer = null;
    }
    this.emit = null;
    this.lastLow = null;
  }

  private async tick(isFirst: boolean): Promise<void> {
    const status: StorageStatus = await this.provider.getStatus();
    this.onStatus(status, isFirst);
  }

  private onStatus(s: StorageStatus, isFirst: boolean): void {
    if (this.emit === null) return;

    // 读不到就只在首次提示一次，避免刷屏
    if (s.freeBytes === null) {
      if (isFirst) {
        this.emitEvent(
          EventLevel.INFO,
          '存储空间读取不可用，已降级',
          new StoragePayloadImpl(undefined, this.lowBytes, 'freeBytes_null')
        );
      }
      return;
    }

    const isLow: boolean = s.freeBytes < this.lowBytes;

    // 首次：直接输出一次当前状态（这是你之前缺的）
    if (this.lastLow === null) {
      this.lastLow = isLow;

      if (isLow) {
        this.emitEvent(
          EventLevel.WARN,
          '存储空间不足(首次检测)',
          new StoragePayloadImpl(s.freeBytes, this.lowBytes, 'first_low')
        );
      } else {
        // 如果你不想首次正常也打印，把这段删掉即可
        this.emitEvent(
          EventLevel.INFO,
          '存储空间正常(首次检测)',
          new StoragePayloadImpl(s.freeBytes, this.lowBytes, 'first_ok')
        );
      }
      return;
    }

    const prev: boolean = this.lastLow;
    this.lastLow = isLow;

    // 只在跨越阈值时打印
    if (!prev && isLow) {
      this.emitEvent(
        EventLevel.WARN,
        '存储空间不足',
        new StoragePayloadImpl(s.freeBytes, this.lowBytes, 'cross_down')
      );
    } else if (prev && !isLow) {
      this.emitEvent(
        EventLevel.INFO,
        '存储空间恢复',
        new StoragePayloadImpl(s.freeBytes, this.lowBytes, 'cross_up')
      );
    }
  }

  private emitEvent(level: EventLevel, message: string, payload: StoragePayload): void {
    if (this.emit === null) return;

    const log: EventLog = {
      type: EventType.STORAGE,
      level,
      timestamp: nowMs(),
      message,
      payload,
      source: 'StorageMonitor',
    };

    this.emit(log);
  }
}
