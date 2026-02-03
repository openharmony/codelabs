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
import { common } from '@kit.AbilityKit';
import { preferences } from '@kit.ArkData';

// 名片数据类型定义
export interface BusinessCardData {
  name: string;
  phone: string;
  email: string;
  company: string;
  position: string;
  department: string;
  address: string;
  website: string;
  notes: string;
  showInList: boolean;
  contactId?: string;
}

// 名片数据管理器
export class BusinessCardManager {
  private context: common.Context;
  private static readonly PREF_NAME = 'businessCard';
  private static readonly CARD_DATA_KEY = 'cardData';

  constructor(context: common.Context) {
    this.context = context;
  }

  // 获取保存的名片数据
  async getSavedCardData(): Promise<BusinessCardData | null> {
    try {
      console.log('BusinessCardManager: 开始加载名片数据');
      const prefs = await preferences.getPreferences(this.context, BusinessCardManager.PREF_NAME);
      const cardJson = await prefs.get(BusinessCardManager.CARD_DATA_KEY, '');

      if (cardJson && cardJson !== '') {
        const data: BusinessCardData = JSON.parse(cardJson.toString()) as BusinessCardData;
        console.log('BusinessCardManager: 加载名片数据成功');
        return data;
      } else {
        console.log('BusinessCardManager: 无保存的名片数据');
        return null;
      }
    } catch (error) {
      console.error('BusinessCardManager: 加载名片数据失败:', error);
      return null;
    }
  }

  // 保存名片数据
  async saveCardData(cardData: BusinessCardData): Promise<boolean> {
    try {
      console.log('BusinessCardManager: 开始保存名片数据');
      const prefs = await preferences.getPreferences(this.context, BusinessCardManager.PREF_NAME);
      await prefs.put(BusinessCardManager.CARD_DATA_KEY, JSON.stringify(cardData));
      await prefs.flush();
      console.log('BusinessCardManager: 保存名片数据成功');
      return true;
    } catch (error) {
      console.error('BusinessCardManager: 保存名片数据失败:', error);
      return false;
    }
  }

  // 删除名片数据
  async deleteCardData(): Promise<boolean> {
    try {
      console.log('BusinessCardManager: 开始删除名片数据');
      const prefs = await preferences.getPreferences(this.context, BusinessCardManager.PREF_NAME);
      await prefs.delete(BusinessCardManager.CARD_DATA_KEY);
      await prefs.flush();
      console.log('BusinessCardManager: 删除名片数据成功');
      return true;
    } catch (error) {
      console.error('BusinessCardManager: 删除名片数据失败:', error);
      return false;
    }
  }

  // 获取默认名片数据
  getDefaultCardData(): BusinessCardData {
    return {
      name: '我的名片',
      phone: '',
      email: '',
      company: '',
      position: '',
      department: '',
      address: '',
      website: '',
      notes: '这是我的个人名片',
      showInList: true
    };
  }
}