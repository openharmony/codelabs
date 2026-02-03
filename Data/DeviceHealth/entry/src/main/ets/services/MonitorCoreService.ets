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

import type common from '@ohos.app.ability.common';
import { EventDB } from '../database/EventDB';
import { EventLogger, LoggerOptions, IEventStore } from './EventLogger';
import type { EventLog } from '../model/EventLog';
import hilog from '@ohos.hilog';

import { BatteryMonitor } from './monitors/BatteryMonitor';
import { NetworkMonitor } from './monitors/NetworkMonitor';
import { StorageMonitor } from './monitors/StorageMonitor';
import { ScreenMonitor } from './monitors/ScreenMonitor';

import type { IBatteryProvider } from '../providers/BatteryProvider';
import type { INetworkProvider } from '../providers/NetworkProvider';
import type { IStorageProvider } from '../providers/StorageProvider';
import type { IScreenProvider } from '../providers/ScreenProvider';
import { EventHubManager } from './EventHubManager';

export interface BatteryPayload {
  soc: number;
}

export interface StoragePayload {
  freePercentage: number;
}

export class MonitorOptions {
  enableBattery?: boolean;
  enableNetwork?: boolean;
  enableStorage?: boolean;
  enableScreen?: boolean;

  batteryLowPercent?: number;
  storageLowBytes?: number;
  storagePollIntervalMs?: number;

  logger?: LoggerOptions;

  batteryProvider?: IBatteryProvider;
  networkProvider?: INetworkProvider;
  storageProvider?: IStorageProvider;
  screenProvider?: IScreenProvider;

  eventStore?: IEventStore;
}

class DefaultMonitorOptions extends MonitorOptions {
  enableBattery: boolean = true;
  enableNetwork: boolean = true;
  enableStorage: boolean = true;
  enableScreen: boolean = true;

  batteryLowPercent: number = 20;
  storageLowBytes: number = 3.5*1024 * 1024 * 1024;
  storagePollIntervalMs: number =5_000;

  logger?: LoggerOptions = undefined;

  batteryProvider?: IBatteryProvider = undefined;
  networkProvider?: INetworkProvider = undefined;
  storageProvider?: IStorageProvider = undefined;
  screenProvider?: IScreenProvider = undefined;

  eventStore?: IEventStore = undefined;
}

export class MonitorCoreService {
  private static readonly LOG_DOMAIN: number = 0x0d001;
  private static readonly LOG_TAG: string = 'MonitorCoreService';

  private static ctx: common.Context | null = null;
  private static logger: EventLogger | null = null;

  private static battery: BatteryMonitor | null = null;
  private static network: NetworkMonitor | null = null;
  private static storage: StorageMonitor | null = null;
  private static screen: ScreenMonitor | null = null;

  private static initPromise: Promise<void> | null = null;

  static async init(ctx: common.Context, opt: MonitorOptions = new DefaultMonitorOptions()): Promise<void> {
    // 防止并发 init
    if (MonitorCoreService.initPromise !== null) {
      return MonitorCoreService.initPromise;
    }

    MonitorCoreService.initPromise = (async () => {
      MonitorCoreService.ctx = ctx;
      hilog.info(MonitorCoreService.LOG_DOMAIN, MonitorCoreService.LOG_TAG, '%{public}s', 'init() enter');

      if (opt.eventStore === undefined) {
        await EventDB.init(ctx);
      }
      MonitorCoreService.logger = new EventLogger(opt.eventStore, opt.logger);
      hilog.info(MonitorCoreService.LOG_DOMAIN, MonitorCoreService.LOG_TAG, '%{public}s', 'init() done');
    })();

    return MonitorCoreService.initPromise;
  }

  static async startAll(opt: MonitorOptions = new DefaultMonitorOptions()): Promise<void> {
    if (MonitorCoreService.ctx === null) {
      throw new Error('Call MonitorCoreService.init(context) first');
    }
    if (MonitorCoreService.logger === null) {
      MonitorCoreService.logger = new EventLogger(opt.eventStore, opt.logger);
    }

    hilog.info(MonitorCoreService.LOG_DOMAIN, MonitorCoreService.LOG_TAG, '%{public}s', 'startAll() called');

    const enableBattery: boolean = opt.enableBattery ?? true;
    const enableNetwork: boolean = opt.enableNetwork ?? true;
    const enableStorage: boolean = opt.enableStorage ?? true;
    const enableScreen: boolean = opt.enableScreen ?? true;

    if (enableBattery) {
      if (MonitorCoreService.battery === null) {
        const low: number = opt.batteryLowPercent ?? 20;
        MonitorCoreService.battery = new BatteryMonitor(low, opt.batteryProvider);
        hilog.info(
          MonitorCoreService.LOG_DOMAIN,
          MonitorCoreService.LOG_TAG,
          '%{public}s%{public}d',
          'BatteryMonitor start(), lowPercent=',
          low
        );
      }
      await MonitorCoreService.battery.start((e: EventLog): void => MonitorCoreService.safeLog(e));
    } else if (MonitorCoreService.battery !== null) {
      await MonitorCoreService.battery.stop();
    }

    if (enableNetwork) {
      if (MonitorCoreService.network === null) {
        MonitorCoreService.network = new NetworkMonitor(opt.networkProvider);
        hilog.info(MonitorCoreService.LOG_DOMAIN, MonitorCoreService.LOG_TAG, '%{public}s', 'NetworkMonitor start()');
      }
      await MonitorCoreService.network.start((e: EventLog): void => MonitorCoreService.safeLog(e));
    } else if (MonitorCoreService.network !== null) {
      await MonitorCoreService.network.stop();
    }

    if (enableStorage) {
      if (MonitorCoreService.storage === null) {
        const lowBytes: number = opt.storageLowBytes ?? 4*1024 * 1024 * 1024;
        const intervalMs: number = opt.storagePollIntervalMs ?? 5_000;

        MonitorCoreService.storage = new StorageMonitor(lowBytes, intervalMs, opt.storageProvider);
        hilog.info(
          MonitorCoreService.LOG_DOMAIN,
          MonitorCoreService.LOG_TAG,
          '%{public}s%{public}d%{public}s%{public}d',
          'StorageMonitor start(), lowBytes=',
          lowBytes,
          ', intervalMs=',
          intervalMs
        );
      }
      await MonitorCoreService.storage.start((e: EventLog): void => MonitorCoreService.safeLog(e));
    } else if (MonitorCoreService.storage !== null) {
      await MonitorCoreService.storage.stop();
    }

    if (enableScreen) {
      if (MonitorCoreService.screen === null) {
        MonitorCoreService.screen = new ScreenMonitor(opt.screenProvider);
        hilog.info(MonitorCoreService.LOG_DOMAIN, MonitorCoreService.LOG_TAG, '%{public}s', 'ScreenMonitor start()');
      }
      await MonitorCoreService.screen.start((e: EventLog): void => MonitorCoreService.safeLog(e));
    } else if (MonitorCoreService.screen !== null) {
      await MonitorCoreService.screen.stop();
    }
  }

  static async stopAll(): Promise<void> {
    hilog.info(MonitorCoreService.LOG_DOMAIN, MonitorCoreService.LOG_TAG, '%{public}s', 'stopAll() called');
    const tasks: Array<Promise<void>> = [];

    if (MonitorCoreService.battery !== null) {
      const m = MonitorCoreService.battery;
      MonitorCoreService.battery = null;
      tasks.push(m.stop().catch(() => {}));
    }

    if (MonitorCoreService.network !== null) {
      const m = MonitorCoreService.network;
      MonitorCoreService.network = null;
      tasks.push(m.stop().catch(() => {}));
    }

    if (MonitorCoreService.storage !== null) {
      const m = MonitorCoreService.storage;
      MonitorCoreService.storage = null;
      tasks.push(m.stop().catch(() => {}));
    }

    if (MonitorCoreService.screen !== null) {
      const m = MonitorCoreService.screen;
      MonitorCoreService.screen = null;
      tasks.push(m.stop().catch(() => {}));
    }

    await Promise.all(tasks);
    hilog.info(MonitorCoreService.LOG_DOMAIN, MonitorCoreService.LOG_TAG, '%{public}s', 'stopAll() done');
  }

  private static safeLog(e: EventLog): void {
    // 先打到 hilog，方便在 logcat 里实时看到“电量/网络/存储/亮屏”等变化
    let payloadStr: string = '';
    try {
      payloadStr = e.payload ? JSON.stringify(e.payload) : '';
    } catch (_) {
      payloadStr = '';
    }

    hilog.info(
      MonitorCoreService.LOG_DOMAIN,
      MonitorCoreService.LOG_TAG,
      '[event] type=%{public}s level=%{public}s msg=%{public}s payload=%{public}s',
      e.type,
      e.level,
      e.message,
      payloadStr
    );

    const eventHub = MonitorCoreService.ctx?.eventHub;
    if (eventHub) {
      if (e.type === 'BATTERY' && e.level === 'WARN') {
        const payload = e.payload as BatteryPayload;
        const soc = payload?.soc;
        if (soc !== undefined) {
          eventHub.emit(EventHubManager.EVENT_LOW_BATTERY, soc);
        }
      }
      if (e.type === 'STORAGE' && e.level === 'WARN') {
        const payload = e.payload as StoragePayload;
        const freePercentage = payload?.freePercentage;
        if (freePercentage !== undefined) {
          eventHub.emit(EventHubManager.EVENT_LOW_STORAGE, freePercentage);
        }
      }
    }

    const lg = MonitorCoreService.logger;
    if (lg === null) return;
    lg.log(e).catch(() => {});
  }
}
