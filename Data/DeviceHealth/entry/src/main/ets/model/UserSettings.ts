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
 * 用户设置数据模型
 * 定义应用全局配置项的接口和默认值
 */

/**
 * 用户设置接口
 * 包含应用所有可配置的选项
 */
export interface UserSettings {
  /** 电池电量预警阈值（百分比），默认20% */
  batteryThreshold: number;

  /** 存储空间预警阈值（百分比），默认15% */
  storageThreshold: number;

  /** 是否开启网络监控 */
  isNetworkMonitorOn: boolean;

  /** 是否开启日志加密 */
  isLogEncryptionOn: boolean;

  /** 是否启用通知 */
  enableNotifications: boolean;
}

/**
 * 默认设置常量
 * 用于首次启动或重置配置时使用
 */
export const DEFAULT_SETTINGS: UserSettings = {
  batteryThreshold: 20,
  storageThreshold: 15,
  isNetworkMonitorOn: true,
  isLogEncryptionOn: false,
  enableNotifications: true
};

/**
 * 设置项的 Key 常量
 * 用于 Preferences 和 AppStorage 的键名管理
 */
export class SettingsKeys {
  static readonly BATTERY_THRESHOLD = 'batteryThreshold';
  static readonly STORAGE_THRESHOLD = 'storageThreshold';
  static readonly NETWORK_MONITOR = 'isNetworkMonitorOn';
  static readonly LOG_ENCRYPTION = 'isLogEncryptionOn';
  static readonly NOTIFICATIONS = 'enableNotifications';
}