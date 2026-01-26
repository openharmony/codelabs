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
import { DebugCase } from './DebugCase';
import { SettingModel } from '../model/SettingModel';
import { BiometricService } from '../services/BiometricService';

export class BiometricSettingCase extends DebugCase {
  readonly name = 'F10 Biometric & Settings Persistence Test';
  private settingModel = new SettingModel();

  async run(): Promise<void> {
    this.logInfo('=== 🧬 开始生物认证与设置模块测试 ===');

    // ---------------------------------------------------------
    // Step 1: 硬件能力检查
    // ---------------------------------------------------------
    this.logInfo('Step 1: Checking Biometric Hardware Availability...');
    const isAvailable = BiometricService.getInstance().checkAvailability();

    if (isAvailable) {
      this.logInfo(' Biometric hardware is AVAILABLE on this device.');
    } else {
      this.logInfo('⚠️ Biometric hardware is NOT available (Normal for some Simulators).');
    }

    // ---------------------------------------------------------
    // Step 2: 读写测试 - 开启开关
    // ---------------------------------------------------------
    this.logInfo('Step 2: Testing Setting Persistence (Turn ON)...');

    // 写入 true
    await this.settingModel.setBiometricEnabled(true);

    // 立即读取验证
    const isEnabledAfterSet = await this.settingModel.isBiometricEnabled();

    if (isEnabledAfterSet === true) {
      this.logInfo(' Write TRUE success. Value read back matches.');
    } else {
      this.logError(`❌ Write TRUE failed. Read back: ${isEnabledAfterSet}`);
    }

    // ---------------------------------------------------------
    // Step 3: 读写测试 - 关闭开关
    // ---------------------------------------------------------
    this.logInfo('Step 3: Testing Setting Persistence (Turn OFF)...');

    // 写入 false
    await this.settingModel.setBiometricEnabled(false);

    // 立即读取验证
    const isEnabledAfterOff = await this.settingModel.isBiometricEnabled();

    if (isEnabledAfterOff === false) {
      this.logInfo(' Write FALSE success. Value read back matches.');
    } else {
      this.logError(`❌ Write FALSE failed. Read back: ${isEnabledAfterOff}`);
    }

    this.logInfo('=== 🧬 测试结束 ===');
  }
}