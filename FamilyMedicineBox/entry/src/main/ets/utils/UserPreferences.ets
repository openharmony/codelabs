/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

import dataPreferences from '@ohos.data.preferences';
import common from '@ohos.app.ability.common';

let preferences: dataPreferences.Preferences | null = null;

export class UserPreferences {
  async init(context: common.Context) {
    preferences = await dataPreferences.getPreferences(context, 'my_store');
  }

  async putUserId(userId: number) {
    if (preferences) {
      await preferences.put('current_user_id', userId);
      await preferences.flush();
    }
  }

  async getUserId(): Promise<number> {
    if (!preferences) {
      return 1;
    }
    return (await preferences.get('current_user_id', 1)) as number;
  }

  async saveAccount(account: string, password: string) {
    if (preferences) {
      await preferences.put('app_account', account);
      await preferences.put('app_password', password);
      await preferences.flush();
    }
  }

  async getAccount(): Promise<string> {
    if (!preferences) {
      return 'admin';
    }
    return (await preferences.get('app_account', 'admin')) as string;
  }

  async getPassword(): Promise<string> {
    if (!preferences) {
      return '123456';
    }
    return (await preferences.get('app_password', '123456')) as string;
  }

  async getUserName(): Promise<string> {
    return this.getAccount();
  }

  async saveCategories(jsonStr: string) {
    if (preferences) {
      await preferences.put('custom_categories', jsonStr);
      await preferences.flush();
    }
  }

  async getCategories(): Promise<string> {
    if (!preferences) {
      return '';
    }
    return (await preferences.get('custom_categories', '')) as string;
  }
}

export default new UserPreferences();
