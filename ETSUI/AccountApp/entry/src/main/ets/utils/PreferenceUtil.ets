/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
import dataPreferences from '@ohos.data.preferences';
import common from '@ohos.app.ability.common';

// 【修复】定义一个类来替代对象字面量，解决 ArkTS 报错
export class UserLoginInfo {
  userId: number = 0;
  username: string = '';
}

export class PreferenceUtil {
  private static readonly PREF_NAME = 'MyPasswordAppPrefs';
  private static readonly KEY_IS_LOCK_ENABLED = 'is_lock_enabled';
  private static readonly KEY_PIN_CODE = 'pin_code';
  private static readonly KEY_CURRENT_USER_ID = 'current_user_id';
  private static readonly KEY_CURRENT_USERNAME = 'current_username';

  private preferences: dataPreferences.Preferences | null = null;

  async init(context: common.UIAbilityContext): Promise<void> {
    try {
      this.preferences = await dataPreferences.getPreferences(context, PreferenceUtil.PREF_NAME);
    } catch (err) {
      console.error('Failed to get preferences');
    }
  }

  // --- 密码锁相关 ---
  async savePinCode(pin: string): Promise<void> {
    if (!this.preferences) return;
    try {
      await this.preferences.put(PreferenceUtil.KEY_PIN_CODE, pin);
      await this.preferences.put(PreferenceUtil.KEY_IS_LOCK_ENABLED, true);
      await this.preferences.flush();
    } catch (e) {
      console.error('Save PIN failed');
    }
  }

  async verifyPinCode(inputPin: string): Promise<boolean> {
    if (!this.preferences) return false;
    try {
      const savedPin = await this.preferences.get(PreferenceUtil.KEY_PIN_CODE, '');
      return savedPin === inputPin;
    } catch (e) {
      return false;
    }
  }

  async isLockEnabled(): Promise<boolean> {
    if (!this.preferences) return false;
    try {
      return await this.preferences.get(PreferenceUtil.KEY_IS_LOCK_ENABLED, false) as boolean;
    } catch (e) {
      return false;
    }
  }

  async clearLock(): Promise<void> {
    if (!this.preferences) return;
    try {
      await this.preferences.put(PreferenceUtil.KEY_IS_LOCK_ENABLED, false);
      await this.preferences.put(PreferenceUtil.KEY_PIN_CODE, '');
      await this.preferences.flush();
    } catch (e) {
      console.error('Clear lock failed');
    }
  }

  // --- 自动登录相关 ---

  async saveLoginInfo(userId: number, username: string): Promise<void> {
    if (!this.preferences) return;
    try {
      await this.preferences.put(PreferenceUtil.KEY_CURRENT_USER_ID, userId);
      await this.preferences.put(PreferenceUtil.KEY_CURRENT_USERNAME, username);
      await this.preferences.flush();
    } catch (e) {
      console.error('Save login info failed');
    }
  }

  // 【修复】返回类型改为 UserLoginInfo 类，解决 "Object literal" 报错
  async getLoginInfo(): Promise<UserLoginInfo | null> {
    if (!this.preferences) return null;
    try {
      const userId = await this.preferences.get(PreferenceUtil.KEY_CURRENT_USER_ID, 0) as number;
      const username = await this.preferences.get(PreferenceUtil.KEY_CURRENT_USERNAME, '') as string;

      if (userId > 0) {
        let info = new UserLoginInfo();
        info.userId = userId;
        info.username = username;
        return info;
      }
    } catch (e) {
      console.error('Get login info failed');
    }
    return null;
  }

  async clearLoginInfo(): Promise<void> {
    if (!this.preferences) return;
    try {
      await this.preferences.put(PreferenceUtil.KEY_CURRENT_USER_ID, 0);
      await this.preferences.put(PreferenceUtil.KEY_CURRENT_USERNAME, '');
      await this.preferences.flush();
    } catch (e) {
      console.error('Clear login info failed');
    }
  }
}

export default new PreferenceUtil();