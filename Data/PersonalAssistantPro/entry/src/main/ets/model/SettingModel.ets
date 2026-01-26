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
import { PreferenceService } from '../services/PreferenceService';
import { Logger } from '../common/utils/Logger';

export class SettingModel {
  private logger = new Logger('SettingModel');
  private readonly KEY_BIOMETRIC_ENABLE = 'is_biometric_enabled';

  /**
   * 读取状态
   */
  async isBiometricEnabled(): Promise<boolean> {
    // 强制获取服务实例
    const service = PreferenceService.getInstance();

    // 读取值，默认为 false
    const value = await service.getValue<boolean>(this.KEY_BIOMETRIC_ENABLE, false);

    this.logger.info(`🔍 [Model] 读取生物认证状态: ${value}`);
    return value;
  }

  /**
   * 保存状态
   */
  async setBiometricEnabled(isEnabled: boolean): Promise<void> {
    this.logger.info(`💾 [Model] 正在保存状态: ${isEnabled}`);

    // 写入并强制刷新
    await PreferenceService.getInstance().putValue(this.KEY_BIOMETRIC_ENABLE, isEnabled);

    this.logger.info(` [Model] 保存完成`);
  }
}