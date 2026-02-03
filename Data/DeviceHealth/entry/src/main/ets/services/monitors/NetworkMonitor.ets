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

import type { INetworkProvider, NetworkStatus } from '../../providers/NetworkProvider';
import { DefaultNetworkProvider } from '../../providers/NetworkProvider';
import { EventLevel, EventLog, EventType, nowMs } from '../../model/EventLog';
import type { Emit } from './BatteryMonitor';

/** 显式 payload 接口（避免 any / untyped object literal） */
export interface NetworkPayload {
  connected: boolean;
  networkType?: string;
}

/** payload 实现类 */
export class NetworkPayloadImpl implements NetworkPayload {
  connected: boolean;
  networkType?: string;

  constructor(connected: boolean, networkType?: string) {
    this.connected = connected;
    this.networkType = networkType;
  }
}

export class NetworkMonitor {
  private provider: INetworkProvider;
  private emit: Emit | null = null;
  private lastConnected: boolean | null = null;

  constructor(provider?: INetworkProvider) {
    this.provider = provider ?? new DefaultNetworkProvider();
  }

  async start(emit: Emit): Promise<void> {
    this.emit = emit;

    const initStatus: NetworkStatus = await this.provider.getStatus();
    this.lastConnected = initStatus.connected;

    await this.provider.subscribe((status: NetworkStatus): void => {
      this.onStatus(status);
    });
  }

  async stop(): Promise<void> {
    await this.provider.unsubscribe();
    this.emit = null;
    this.lastConnected = null;
  }

  private onStatus(status: NetworkStatus): void {
    const prev: boolean | null = this.lastConnected;
    this.lastConnected = status.connected;

    if (prev === null || this.emit === null) {
      return;
    }

    if (prev && !status.connected) {
      this.emitEvent(
        EventLevel.WARN,
        '网络已断开',
        new NetworkPayloadImpl(false, status.networkType)
      );
    } else if (!prev && status.connected) {
      this.emitEvent(
        EventLevel.INFO,
        '网络已连接',
        new NetworkPayloadImpl(true, status.networkType)
      );
    }
  }

  private emitEvent(level: EventLevel, message: string, payload: NetworkPayload): void {
    if (this.emit === null) {
      return;
    }

    const event: EventLog = {
      type: EventType.NETWORK,
      level,
      timestamp: nowMs(),
      message,
      payload,
      source: 'NetworkMonitor',
    };

    this.emit(event);
  }
}
