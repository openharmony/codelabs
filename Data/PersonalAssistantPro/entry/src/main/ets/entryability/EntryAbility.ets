/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// PAP/PersonalAssistantPro/entry/src/main/ets/entryability/EntryAbility.ets
import { AbilityConstant, ConfigurationConstant, UIAbility, Want } from '@kit.AbilityKit';
import { window } from '@kit.ArkUI';
import { Logger } from '../common/utils/Logger';
import { runCurrentDebugCase } from '../debug/DebugRunner';
import { PreferenceService } from '../services/PreferenceService';
import { RdbHelper } from '../common/database/RdbHelper';
import { AesCryptoUtils } from '../common/utils/AesCryptoUtils';

export default class EntryAbility extends UIAbility {
  private logger = new Logger('EntryAbility');

  // ✅ 生命周期：应用冷启动 (被点击图标或通知唤起)
  async onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): Promise<void> {
    this.logger.info('Ability onCreate');
    AppStorage.setOrCreate('abilityContext', this.context);

    // ✅ 1. 检查是否有通知跳转参数
    this.handleNotificationJump(want);

    try {
      this.context.getApplicationContext().setColorMode(ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET);
    } catch (err) {
      this.logger.error(`Failed to set colorMode. Cause: ${JSON.stringify(err as object)}`);
    }

    // === 原有初始化逻辑保持不变 ===
    try {
      this.logger.info('=== [Debug Mode] Pre-initializing dependencies ===');

      // 1. 初始化首选项
      await PreferenceService.getInstance().init(this.context);

      // 2. 初始化数据库
      await RdbHelper.getInstance().initRdb(this.context);

      // 3. 初始化加密密钥
      const savedKey = await PreferenceService.getInstance().getAesKey();

      if (savedKey && savedKey.length > 0) {
        await AesCryptoUtils.initKey(savedKey);
        this.logger.info('✅ Restored AES key from preferences.');
      } else {
        const newKey = await AesCryptoUtils.initKey();
        await PreferenceService.getInstance().saveAesKey(newKey);
        this.logger.info('🆕 Generated and saved new AES key.');
      }

      this.logger.info('=== Dependencies ready. Starting Debug Case ===');

      // 4. 运行调试用例
      await runCurrentDebugCase();

    } catch (err) {
      this.logger.error(`Debug environment init failed: ${JSON.stringify(err as object)}`);
    }

    this.logger.info('Initialization delegated to SplashPage.');
  }

  // ✅ 生命周期：应用已在后台运行 (被通知唤起)
  // 必须添加这个方法，否则后台运行时点击通知没反应
  onNewWant(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    this.logger.info('Ability onNewWant');
    // 处理热启动时的跳转参数
    this.handleNotificationJump(want);
  }

  onDestroy(): void {
    this.logger.info('Ability onDestroy');
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    this.logger.info('Ability onWindowStageCreate');

    windowStage.loadContent('pages/SplashPage', (err) => {
      if (err.code) {
        this.logger.error(`Failed to load the content. Cause: ${JSON.stringify(err)}`);
        return;
      }
      this.logger.info('Succeeded in loading the content.');
    });
  }

  onWindowStageDestroy(): void {
    this.logger.info('Ability onWindowStageDestroy');
  }

  onForeground(): void {
    this.logger.info('Ability onForeground');
  }

  onBackground(): void {
    this.logger.info('Ability onBackground');
  }

  // ✅ 辅助方法：处理通知跳转逻辑
  private handleNotificationJump(want: Want) {
    if (want && want.parameters && want.parameters['targetPage']) {
      const targetPage = want.parameters['targetPage'] as string;
      const eventId = want.parameters['eventId'] as number;

      // ✅ 使用 logger 输出日志
      this.logger.info(`🔔 Notification Clicked! Preparing to jump to: ${targetPage}, ID: ${eventId}`);

      // 1. 先存 ID (数据)
      AppStorage.setOrCreate('notificationJumpId', eventId);

      // 2. 再存 Target (开关)
      AppStorage.setOrCreate('notificationJumpTarget', targetPage);
    }
  }
}