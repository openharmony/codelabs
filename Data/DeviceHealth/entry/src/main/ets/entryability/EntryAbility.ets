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
 * 应用生命周期管理
 * EntryAbility 是应用的入口点，负责初始化全局资源
 */

import { AbilityConstant, ConfigurationConstant, UIAbility, Want } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';
import { PreferenceManager } from '../manager/PreferenceManager';
import { EventHubManager } from '../services/EventHubManager';
import { MonitorCoreService } from '../services/MonitorCoreService';
import { BusinessError } from '@kit.BasicServicesKit';
import { runAllTests } from '../test/TestRunner'

const DOMAIN: number = 0x0000;

export default class EntryAbility extends UIAbility {

  /** 是否已经启动监控服务（前台只启动一次） */
  private started: boolean = false;

  /**
   * onCreate 生命周期
   * 应用首次启动时调用，只执行一次
   * 适合进行全局初始化操作
   *
   * @param _want 启动参数（未使用，用下划线前缀标记）
   * @param _launchParam 启动模式参数（未使用，用下划线前缀标记）
   */
  onCreate(want: Want, _launchParam: AbilityConstant.LaunchParam): void {
    try {
      // 设置颜色模式
      this.context.getApplicationContext().setColorMode(
        ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET
      );
    } catch (err) {
      // 显式声明 err 的类型为 BusinessError
      const error = err as BusinessError;
      hilog.error(DOMAIN, 'testTag', 'Failed to set colorMode. Cause: %{public}s', JSON.stringify(error));
    }

    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onCreate');

    console.info('[EntryAbility] onCreate')

    // ===== 临时测试入口 =====
    runAllTests()
    /**
     * 【关键步骤】初始化 PreferenceManager
     * 作用：
     * 1. 创建/获取 Preferences 数据库实例
     * 2. 加载持久化的配置数据
     * 3. 同步数据到 AppStorage（全局状态）
     *
     * 注意：
     * - 这是异步操作，但不需要 await
     * - 即使初始化未完成，UI 也会先使用默认值渲染
     * - 初始化完成后会自动通过 @StorageLink 刷新 UI
     */
    PreferenceManager.getInstance()
      .init(this.context)
      .then(() => {
        hilog.info(DOMAIN, 'DeviceHealthApp', 'PreferenceManager 初始化成功');
      })
      .catch((err: BusinessError) => {
        hilog.error(
          DOMAIN,
          'DeviceHealthApp',
          'PreferenceManager 初始化失败: %{public}s',
          JSON.stringify(err)
        );
      });
    EventHubManager.getInstance()
      .init(this.context)
      .then(() => EventHubManager.subscribeAll());
  }

  /**
   * onDestroy 生命周期
   * 应用退出时调用
   */
  onDestroy(): void {
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onDestroy');
  }

  /**
   * onWindowStageCreate 生命周期
   * 窗口创建时调用，用于加载首页
   *
   * @param windowStage 窗口阶段对象
   */
  onWindowStageCreate(windowStage: window.WindowStage): void {
    // Main window is created, set main page for this ability
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageCreate');

    /**
     * 加载设置页面
     */
    windowStage.loadContent('pages/SettingsPage', (err: BusinessError) => {
      if (err.code) {
        hilog.error(
          DOMAIN,
          'testTag',
          'Failed to load the content. Cause: %{public}s',
          JSON.stringify(err)
        );
        return;
      }
      hilog.info(DOMAIN, 'testTag', 'Succeeded in loading the content.');
    });
  }

  /**
   * onWindowStageDestroy 生命周期
   * 窗口销毁时调用
   */
  onWindowStageDestroy(): void {
    // Main window is destroyed, release UI related resources
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageDestroy');
  }

  /**
   * onForeground 生命周期
   * 应用切换到前台时调用
   */
  onForeground(): void {
    // Ability has brought to foreground
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onForeground');

    if (this.started) {
      return;
    }
    this.started = true;

    // 启动监控服务（Promise 链，避免修改方法签名）
    MonitorCoreService.init(this.context)
      .then(() => MonitorCoreService.startAll())
      .then(() => {
        hilog.info(DOMAIN, 'testTag', '%{public}s', 'MonitorCoreService started');
      })
      .catch((e: Error) => {
        hilog.error(
          DOMAIN,
          'testTag',
          'MonitorCoreService start failed: %{public}s',
          e.message
        );
      });
  }

  /**
   * onBackground 生命周期
   * 应用切换到后台时调用
   */
  onBackground(): void {
    // Ability has back to background
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onBackground');

    // // 退出前台即停止监控，避免后台轮询/耗电
    // MonitorCoreService.stopAll()
    //   .then(() => {
    //     hilog.info(DOMAIN, 'testTag', '%{public}s', 'MonitorCoreService stopped');
    //   })
    //   .catch((e: Error) => {
    //     hilog.error(
    //       DOMAIN,
    //       'testTag',
    //       'MonitorCoreService stop failed: %{public}s',
    //       e.message
    //     );
    //   });
    //
    // this.started = false;
  }
}
