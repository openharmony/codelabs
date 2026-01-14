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
import { AbilityConstant, ConfigurationConstant, UIAbility, Want } from '@kit.AbilityKit';
import { window } from '@kit.ArkUI';
import { Logger } from '../common/utils/Logger';
import { runCurrentDebugCase } from '../debug/DebugRunner';
import { PreferenceService } from '../services/PreferenceService';
import { RdbHelper } from '../common/database/RdbHelper';
import { AesCryptoUtils } from '../common/utils/AesCryptoUtils';

export default class EntryAbility extends UIAbility {
  private logger = new Logger('EntryAbility');

  async onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): Promise<void> {
    this.logger.info('Ability onCreate');
    AppStorage.setOrCreate('abilityContext', this.context);

    try {
      this.context.getApplicationContext().setColorMode(ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET);
    } catch (err) {
      this.logger.error(`Failed to set colorMode. Cause: ${JSON.stringify(err as object)}`);
    }

    try {
      this.logger.info('=== [Debug Mode] Pre-initializing dependencies ===');

      // 1. 初始化首选项
      await PreferenceService.getInstance().init(this.context);

      // 2. 初始化数据库
      await RdbHelper.getInstance().initRdb(this.context);

      // 3. 初始化加密密钥
      // ✅ 修复：AesCryptoUtils 已改为静态类，直接调用 initKey，去掉 getInstance()
      await AesCryptoUtils.initKey();

      this.logger.info('=== Dependencies ready. Starting Debug Case ===');

      // 4. 运行调试用例
      await runCurrentDebugCase();

    } catch (err) {
      this.logger.error(`Debug environment init failed: ${JSON.stringify(err as object)}`);
    }

    this.logger.info('Initialization delegated to SplashPage.');
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
}