/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
import preferences from '@ohos.data.preferences';
import common from '@ohos.app.ability.common';
import Logger from '../common/utils/Logger';

/**
 * AuthService
 * 使用 Preferences 实现 4 位数字 PIN 码的持久化存储与校验
 *
 * PIN 码仅保存在应用沙箱内，用于“隐私安全保险箱”身份验证。
 */
class AuthService {
  private static readonly PREF_NAME: string = 'privacy_safe_box_pref';
  private static readonly KEY_PIN: string = 'pin_code';

  private static async getStore(context: common.UIAbilityContext): Promise<preferences.Preferences> {
    try {
      return await preferences.getPreferences(context, AuthService.PREF_NAME);
    } catch (err) {
      Logger.error('[AuthService] getStore failed', JSON.stringify(err));
      throw new Error(`获取存储失败: ${JSON.stringify(err)}`);
    }
  }

  /** 是否已经设置过 PIN 码 */
  static async hasPin(context: common.UIAbilityContext): Promise<boolean> {
    const store = await AuthService.getStore(context);
    const value = await store.get(AuthService.KEY_PIN, '') as string;
    return !!value && value.length > 0;
  }

  /** 设置/更新 PIN 码 */
  static async setPin(context: common.UIAbilityContext, pin: string): Promise<void> {
    const store = await AuthService.getStore(context);
    await store.put(AuthService.KEY_PIN, pin);
    await store.flush();
    Logger.info('[AuthService] PIN updated');
  }

  /** 校验 PIN 码是否正确 */
  static async verifyPin(context: common.UIAbilityContext, pin: string): Promise<boolean> {
    const store = await AuthService.getStore(context);
    const value = await store.get(AuthService.KEY_PIN, '') as string;
    return value === pin;
  }

  /** 清除已保存的 PIN 码（当前未在 UI 中暴露，仅供必要时使用） */
  static async clearPin(context: common.UIAbilityContext): Promise<void> {
    const store = await AuthService.getStore(context);
    await store.delete(AuthService.KEY_PIN);
    await store.flush();
    Logger.info('[AuthService] PIN cleared');
  }
}

export default AuthService;

