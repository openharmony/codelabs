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
// PAP/PersonalAssistantPro/entry/src/main/ets/services/PreferenceService.ets
import dataPreferences from '@ohos.data.preferences';
import { Logger } from '../common/utils/Logger';
import { Context } from '@kit.AbilityKit';

export class PreferenceService {
  private static instance: PreferenceService;
  private preferences: dataPreferences.Preferences | null = null;
  private logger = new Logger('PreferenceService');
  private readonly PREF_NAME = 'personal_assistant_pref';
  private readonly KEY_AES_SECRET = 'secure_aes_key';

  private constructor() {
  }

  public static getInstance(): PreferenceService {
    if (!PreferenceService.instance) {
      PreferenceService.instance = new PreferenceService();
    }
    return PreferenceService.instance;
  }

  async init(context: Context): Promise<void> {
    try {
      this.preferences = await dataPreferences.getPreferences(context, this.PREF_NAME);
      this.logger.info('Preferences initialized successfully.');
    } catch (err) {
      this.logger.error('Failed to get preferences', err);
    }
  }

  // --- AES 密钥专用方法 ---

  async getAesKey(): Promise<string> {
    return await this.getValue<string>(this.KEY_AES_SECRET, '');
  }

  async saveAesKey(key: string): Promise<void> {
    await this.putValue(this.KEY_AES_SECRET, key);
  }

  // --- 🔥 通用读写方法 (解决 Model 层调用报错的核心) ---

  /**
   * 通用获取值方法
   * 使用泛型 <T> 避免 'any' 类型报错
   */
  async getValue<T>(key: string, defValue: T): Promise<T> {
    if (!this.preferences) {
      return defValue;
    }
    try {
      // 显式断言为 ValueType，满足编译器检查
      const val = await this.preferences.get(key, defValue as dataPreferences.ValueType);
      return val as T;
    } catch (err) {
      this.logger.error(`Failed to get value for ${key}`, err);
      return defValue;
    }
  }

  /**
   * 通用写入值方法
   */
  async putValue(key: string, value: dataPreferences.ValueType): Promise<void> {
    if (!this.preferences) {
      return;
    }
    try {
      await this.preferences.put(key, value);
      await this.preferences.flush();
    } catch (err) {
      this.logger.error(`Failed to put value for ${key}`, err);
    }
  }
}