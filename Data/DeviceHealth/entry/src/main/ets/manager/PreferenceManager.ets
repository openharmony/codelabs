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

/**
 * 偏好设置管理器（类型安全版）
 * 修复了 ArkTS 严格类型检查问题
 */

import preferences from '@ohos.data.preferences';
import { DEFAULT_SETTINGS, SettingsKeys } from '../model/UserSettings';
import common from '@ohos.app.ability.common';
import { MonitorCoreService, MonitorOptions } from '../services/MonitorCoreService';

/**
 * 配置值类型
 * 明确定义所有可能的配置值类型
 */
type SettingValue = number | boolean | string;

export class PreferenceManager {
  private static instance: PreferenceManager;
  private dataPreferences: preferences.Preferences | null = null;
  private readonly PREFERENCE_NAME = 'deviceHealthSettings';

  private constructor() {}

  public static getInstance(): PreferenceManager {
    if (!PreferenceManager.instance) {
      PreferenceManager.instance = new PreferenceManager();
    }
    return PreferenceManager.instance;
  }

  public async init(context: common.UIAbilityContext): Promise<void> {
    try {
      this.dataPreferences = await preferences.getPreferences(context, this.PREFERENCE_NAME);
      console.info('[PreferenceManager] 初始化成功');
      await this.loadSettings();
      await this.updateMonitor();
    } catch (error) {
      console.error('[PreferenceManager] 初始化失败:', JSON.stringify(error));
    }
  }

  private async loadSettings(): Promise<void> {
    if (!this.dataPreferences) {
      console.error('[PreferenceManager] Preferences 未初始化');
      return;
    }

    try {
      /**
       * 类型安全的数据读取
       * 使用明确的类型断言，避免 any/unknown
       */
      const batteryThreshold = await this.dataPreferences.get(
        SettingsKeys.BATTERY_THRESHOLD,
        DEFAULT_SETTINGS.batteryThreshold
      ) as number;

      const storageThreshold = await this.dataPreferences.get(
        SettingsKeys.STORAGE_THRESHOLD,
        DEFAULT_SETTINGS.storageThreshold
      ) as number;

      const isNetworkMonitorOn = await this.dataPreferences.get(
        SettingsKeys.NETWORK_MONITOR,
        DEFAULT_SETTINGS.isNetworkMonitorOn
      ) as boolean;

      const isLogEncryptionOn = await this.dataPreferences.get(
        SettingsKeys.LOG_ENCRYPTION,
        DEFAULT_SETTINGS.isLogEncryptionOn
      ) as boolean;

      const enableNotifications = await this.dataPreferences.get(
        SettingsKeys.NOTIFICATIONS,
        DEFAULT_SETTINGS.enableNotifications
      ) as boolean;

      /**
       * 同步到 AppStorage
       * 使用 SettingValue 类型确保类型安全
       */
      AppStorage.setOrCreate<number>(SettingsKeys.BATTERY_THRESHOLD, batteryThreshold);
      AppStorage.setOrCreate<number>(SettingsKeys.STORAGE_THRESHOLD, storageThreshold);
      AppStorage.setOrCreate<boolean>(SettingsKeys.NETWORK_MONITOR, isNetworkMonitorOn);
      AppStorage.setOrCreate<boolean>(SettingsKeys.LOG_ENCRYPTION, isLogEncryptionOn);
      AppStorage.setOrCreate<boolean>(SettingsKeys.NOTIFICATIONS, enableNotifications);

      console.info('[PreferenceManager] 设置加载完成');
    } catch (error) {
      console.error('[PreferenceManager] 加载设置失败:', JSON.stringify(error));
    }
  }

  /**
   * 保存单个配置项（类型安全版）
   * @param key 配置项键名
   * @param value 配置项值（明确的联合类型）
   */
  public async saveValue(key: string, value: SettingValue): Promise<void> {
    if (!this.dataPreferences) {
      console.error('[PreferenceManager] Preferences 未初始化');
      return;
    }

    try {
      /**
       * 使用 preferences.ValueType 确保类型正确
       * ValueType = number | string | boolean | Array<number> | Array<string> | Array<boolean>
       */
      await this.dataPreferences.put(key, value as preferences.ValueType);
      await this.dataPreferences.flush();

      /**
       * 根据值的实际类型更新 AppStorage
       * 使用类型守卫确保类型安全
       */
      if (typeof value === 'number') {
        AppStorage.set<number>(key, value);
      } else if (typeof value === 'boolean') {
        AppStorage.set<boolean>(key, value);
      } else if (typeof value === 'string') {
        AppStorage.set<string>(key, value);
      }

      await this.updateMonitor();
      console.info(`[PreferenceManager] 保存成功: ${key} = ${value}`);
    } catch (error) {
      console.error('[PreferenceManager] 保存失败:', JSON.stringify(error));
    }
  }

  // /**
  //  * 批量保存设置（类型安全版）
  //  * @param settings 设置对象，使用 Record 明确类型
  //  */
  // public async saveAllSettings(settings: Record<string, SettingValue>): Promise<void> {
  //   const keys = Object.keys(settings);
  //   for (let i = 0; i < keys.length; i++) {
  //     const key = keys[i];
  //     const value = settings[key];
  //     await this.saveValue(key, value);
  //   }
  // }

  /**
   * 重置为默认设置（类型安全版）
   */
  public async resetToDefaults(): Promise<void> {
    try {
      // 并行执行所有重置操作，明确类型
      await Promise.all([
        this.saveValue(SettingsKeys.BATTERY_THRESHOLD, DEFAULT_SETTINGS.batteryThreshold),
        this.saveValue(SettingsKeys.STORAGE_THRESHOLD, DEFAULT_SETTINGS.storageThreshold),
        this.saveValue(SettingsKeys.NETWORK_MONITOR, DEFAULT_SETTINGS.isNetworkMonitorOn),
        this.saveValue(SettingsKeys.LOG_ENCRYPTION, DEFAULT_SETTINGS.isLogEncryptionOn),
        this.saveValue(SettingsKeys.NOTIFICATIONS, DEFAULT_SETTINGS.enableNotifications)
      ]);

      await this.updateMonitor();
      console.info('[PreferenceManager] 已重置为默认设置');
    } catch (error) {
      console.error('[PreferenceManager] 重置失败:', JSON.stringify(error));
    }
  }

  public async updateMonitor(): Promise<void> {
    try{
      if (!this.dataPreferences) {
        return;
      }
      const batteryThreshold = AppStorage.get<number>(SettingsKeys.BATTERY_THRESHOLD);
      const storageThreshold = AppStorage.get<number>(SettingsKeys.STORAGE_THRESHOLD) ?? 3.5;
      const isNetworkMonitorOn = AppStorage.get<boolean>(SettingsKeys.NETWORK_MONITOR);

      const monitorOptions: MonitorOptions = new MonitorOptions();

      monitorOptions.enableBattery = true;
      monitorOptions.enableNetwork = isNetworkMonitorOn;
      monitorOptions.enableStorage = true;
      monitorOptions.enableScreen = true;

      monitorOptions.batteryLowPercent = batteryThreshold;
      monitorOptions.storageLowBytes = storageThreshold * 1024 * 1024 * 1024;

      await MonitorCoreService.startAll(monitorOptions);
    } catch (error) {
      console.error('[PreferenceManager] 配置更新失败！');
    }
  }
}