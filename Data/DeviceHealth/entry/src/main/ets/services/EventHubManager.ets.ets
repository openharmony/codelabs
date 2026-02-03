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
 * 设置页面通用 UI 组件
 * 提供可复用的卡片式布局组件
 */

// @Component
// export struct SettingGroup {
//   ...
// }

/**
 * 事件管理中心
 * 功能：
 * 1. 监控 AppStorage 中电量与存储空间的变化
 * 2. 内部硬编码预警阈值，并执行自动弹窗逻辑
 */
import common from '@ohos.app.ability.common';
import DialogUtils from '../components/Notification/DialogUtils';

export class EventHubManager {
  private static instance: EventHubManager;
  private static ctx: common.Context | null = null;

  // 定义统一的事件名常量
  public static readonly EVENT_LOW_BATTERY = 'EVENT_LOW_BATTERY';
  public static readonly EVENT_LOW_STORAGE = 'EVENT_LOW_STORAGE';

  private constructor() {}

  public static getInstance(): EventHubManager {
    if (!EventHubManager.instance) {
      EventHubManager.instance = new EventHubManager();
    }
    return EventHubManager.instance;
  }

  // 初始化，在主页面的 aboutToAppear 调用
  public async init(context: common.Context): Promise<void> {
    EventHubManager.ctx = context;
  }

  // 开启全局监听：电量不足信号
  static async subscribeBatteryWarning(): Promise<void> {
    if (!EventHubManager.ctx) return;

    EventHubManager.ctx.eventHub.on(EventHubManager.EVENT_LOW_BATTERY, (soc: number) => {
      // 接收到信号后，直接调用之前封装好的弹窗
      if (soc < 20){
        DialogUtils.showNotice(
          '电量不足警告',
          `检测到系统电量已降至 ${soc}%，请及时充电。`,
          () => { console.info('用户点击确定，准备进入省电模式') }
        );
      }
    });
  }

  // 发送电量不足信号
  public emitBatteryWarning(soc: number): void {
    if (EventHubManager.ctx) {
      EventHubManager.ctx.eventHub.emit(EventHubManager.EVENT_LOW_BATTERY, soc);
    }
  }

  // 开启全局监听：存储空间不足信号
  static async subscribeStorageWarning(): Promise<void> {
    if (!EventHubManager.ctx) return;

    EventHubManager.ctx.eventHub.on(EventHubManager.EVENT_LOW_STORAGE, (freePercentage: number) => {
      // 接收到信号后，显示存储空间不足警告

      if (freePercentage < 20) {
        DialogUtils.showNotice(
          '存储空间不足警告',
          '剩余存储空间不足20%，请注意清理',
          () => {
            console.info('用户确认存储空间警告，准备跳转到清理页面');
            // 这里可以添加跳转到清理页面的逻辑
          }
        );
      }
    });
  }
  // 发送存储空间警告信号（简化版）
  public emitStorageWarning(freePercentage: number): void {
    if (EventHubManager.ctx) {
      EventHubManager.ctx.eventHub.emit(EventHubManager.EVENT_LOW_STORAGE, freePercentage);
    }
  }


  // 开启所有监听
  static async subscribeAll(): Promise<void> {
    EventHubManager.subscribeBatteryWarning();
    EventHubManager.subscribeStorageWarning();
  }
  // 取消监听，防止内存泄漏
  public unSubscribeAll(): void {
    if (EventHubManager.ctx) {
      EventHubManager.ctx.eventHub.off(EventHubManager.EVENT_LOW_BATTERY);
      EventHubManager.ctx.eventHub.off(EventHubManager.EVENT_LOW_STORAGE);
    }
  }
}