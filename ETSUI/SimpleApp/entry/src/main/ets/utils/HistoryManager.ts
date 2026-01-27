/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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

/**
 * 最近使用功能历史记录管理类
 */
import { preferences } from '@kit.ArkData';
import { common } from '@kit.AbilityKit';

export interface AppUsageRecord {
  appName: string;
  timestamp: number;
  packageName?: string;
  icon?: string;
}

export class HistoryManager {
  private static readonly KEY = 'appUsageHistory';
  private static readonly MAX_RECORDS = 10; // 最多保留10条记录

  /**
   * 添加新的使用记录
   */
  static async addRecord(context: common.UIAbilityContext, record: AppUsageRecord): Promise<void> {
    try {
      const pref = await preferences.getPreferences(context, 'history');
      const historyStr = await pref.get(this.KEY, '[]') as string;
      let history: AppUsageRecord[] = JSON.parse(historyStr);
      
      // 移除重复记录（相同应用）
      history = history.filter(item => item.appName !== record.appName);
      
      // 添加新记录到开头
      history.unshift(record);
      
      // 限制记录数量
      if (history.length > this.MAX_RECORDS) {
        history = history.slice(0, this.MAX_RECORDS);
      }
      
      await pref.put(this.KEY, JSON.stringify(history));
      await pref.flush();
      
      console.log('历史记录添加成功:', record.appName);
    } catch (error) {
      console.error('添加历史记录失败:', error);
    }
  }

  /**
   * 获取所有历史记录
   */
  static async getRecords(context: common.UIAbilityContext): Promise<AppUsageRecord[]> {
    try {
      const pref = await preferences.getPreferences(context, 'history');
      const historyStr = await pref.get(this.KEY, '[]') as string;
      return JSON.parse(historyStr);
    } catch (error) {
      console.error('获取历史记录失败:', error);
      return [];
    }
  }

  /**
   * 清除所有历史记录
   */
  static async clearRecords(context: common.UIAbilityContext): Promise<void> {
    try {
      const pref = await preferences.getPreferences(context, 'history');
      await pref.put(this.KEY, '[]');
      await pref.flush();
      console.log('历史记录已清除');
    } catch (error) {
      console.error('清除历史记录失败:', error);
    }
  }

  /**
   * 格式化时间显示
   */
  static formatTime(timestamp: number): string {
    const now = Date.now();
    const diff = now - timestamp;
    
    if (diff < 60000) { // 1分钟内
      return '刚刚';
    } else if (diff < 3600000) { // 1小时内
      return Math.floor(diff / 60000) + '分钟前';
    } else if (diff < 86400000) { // 1天内
      return Math.floor(diff / 3600000) + '小时前';
    } else {
      return Math.floor(diff / 86400000) + '天前';
    }
  }
}