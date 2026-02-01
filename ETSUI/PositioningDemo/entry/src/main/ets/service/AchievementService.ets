/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-20
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import { Achievement, AchievementType, AchievementConfig } from '../model/AchievementModel';
import { HistoryService } from './HistoryService';
import { StatisticsService } from './StatisticsService';
import { SportData } from '../model/SportModel';
import { preferences } from '@kit.ArkData';
import { common } from '@kit.AbilityKit';

// 解锁成就信息接口
interface UnlockedAchievementInfo {
  unlockedTime: number;
}

export class AchievementService {
  private preferences: preferences.Preferences | null = null;
  private readonly KEY_UNLOCKED_ACHIEVEMENTS = 'unlocked_achievements';
  private historyService: HistoryService;
  private statisticsService: StatisticsService;

  constructor(historyService: HistoryService, statisticsService: StatisticsService) {
    this.historyService = historyService;
    this.statisticsService = statisticsService;
  }

  async init(context: common.Context): Promise<void> {
    try {
      this.preferences = await preferences.getPreferences(context, 'achievements');
      console.log('成就服务初始化成功');
    } catch (err) {
      console.error('初始化成就服务失败:', err);
    }
  }

  // 获取所有成就（包含解锁状态和进度）
  async getAllAchievements(): Promise<Achievement[]> {
    const allAchievements = AchievementConfig.getAllAchievements();
    const unlockedMap = await this.getUnlockedAchievementsMap();
    
    // 计算每个成就的进度和状态，并检查是否需要解锁
    for (const achievement of allAchievements) {
      const unlockedInfo = unlockedMap.get(achievement.id);
      if (unlockedInfo) {
        achievement.isUnlocked = true;
        achievement.unlockedTime = unlockedInfo.unlockedTime;
        achievement.progress = 100;
        achievement.currentValue = achievement.targetValue;
      } else {
        // 计算当前进度
        await this.calculateProgress(achievement);
        
        // 检查是否达到解锁条件，如果达到则自动解锁
        if (achievement.progress >= 100 && !achievement.isUnlocked) {
          achievement.isUnlocked = true;
          achievement.unlockedTime = Date.now();
          achievement.currentValue = achievement.targetValue;
          
          // 保存解锁状态
          await this.unlockAchievement(achievement.id, achievement.unlockedTime);
          console.log(`成就自动解锁: ${achievement.name}`);
        }
      }
    }

    return allAchievements;
  }

  // 检查并更新成就（在运动记录保存后调用）
  async checkAndUpdateAchievements(newRecord?: SportData): Promise<Achievement[]> {
    const allAchievements = AchievementConfig.getAllAchievements();
    const unlockedMap = await this.getUnlockedAchievementsMap();
    const newlyUnlocked: Achievement[] = [];

    for (const achievement of allAchievements) {
      // 如果已经解锁，跳过
      if (unlockedMap.has(achievement.id)) {
        achievement.isUnlocked = true;
        const unlockedInfo = unlockedMap.get(achievement.id)!;
        achievement.unlockedTime = unlockedInfo.unlockedTime;
        achievement.progress = 100;
        achievement.currentValue = achievement.targetValue;
        continue;
      }

      // 计算当前进度
      await this.calculateProgress(achievement);

      // 检查是否达到解锁条件
      if (achievement.progress >= 100 && !achievement.isUnlocked) {
        achievement.isUnlocked = true;
        achievement.unlockedTime = Date.now();
        achievement.currentValue = achievement.targetValue;
        
        // 保存解锁状态
        await this.unlockAchievement(achievement.id, achievement.unlockedTime);
        newlyUnlocked.push(achievement);
        
        console.log(`成就解锁: ${achievement.name}`);
      }
    }

    return newlyUnlocked;
  }

  // 计算成就进度
  private async calculateProgress(achievement: Achievement): Promise<void> {
    const records = await this.historyService.getSportRecords();
    const statistics = await this.statisticsService.calculateUserStatistics();

    let currentValue = 0;
    let targetValue = achievement.targetValue;

    switch (achievement.type) {
      case AchievementType.FIRST_SPORT:
        currentValue = records.length > 0 ? 1 : 0;
        break;

      case AchievementType.TOTAL_DISTANCE_1KM:
      case AchievementType.TOTAL_DISTANCE_5KM:
      case AchievementType.TOTAL_DISTANCE_10KM:
      case AchievementType.TOTAL_DISTANCE_50KM:
      case AchievementType.TOTAL_DISTANCE_100KM:
        currentValue = statistics.totalDistance;
        break;

      case AchievementType.CONSECUTIVE_DAYS_3:
      case AchievementType.CONSECUTIVE_DAYS_7:
      case AchievementType.CONSECUTIVE_DAYS_30:
        currentValue = await this.calculateConsecutiveDays(records);
        break;

      case AchievementType.TOTAL_RECORDS_10:
      case AchievementType.TOTAL_RECORDS_50:
      case AchievementType.TOTAL_RECORDS_100:
        currentValue = statistics.totalRecords;
        break;

      case AchievementType.SINGLE_DISTANCE_5KM:
      case AchievementType.SINGLE_DISTANCE_10KM:
        currentValue = statistics.longestDistance;
        break;

      case AchievementType.SINGLE_DURATION_30MIN:
      case AchievementType.SINGLE_DURATION_60MIN:
        currentValue = statistics.longestDuration;
        break;

      case AchievementType.TOTAL_CALORIES_1000:
      case AchievementType.TOTAL_CALORIES_5000:
        currentValue = statistics.totalCalories;
        break;

      default:
        currentValue = 0;
    }

    achievement.currentValue = currentValue;
    achievement.progress = Math.min(100, (currentValue / targetValue) * 100);
  }

  // 计算连续运动天数
  private async calculateConsecutiveDays(records: SportData[]): Promise<number> {
    if (records.length === 0) return 0;

    // 按日期分组，获取所有有运动的日期
    const dateSet = new Set<string>();
    records.forEach(record => {
      const date = new Date(record.startTime);
      const dateKey = `${date.getFullYear()}-${date.getMonth() + 1}-${date.getDate()}`;
      dateSet.add(dateKey);
    });

    // 转换为日期数组并排序（从新到旧）
    const dates = Array.from(dateSet)
      .map(key => {
        const parts = key.split('-');
        const year = Number(parts[0]);
        const month = Number(parts[1]);
        const day = Number(parts[2]);
        const d = new Date(year, month - 1, day);
        d.setHours(0, 0, 0, 0);
        return d;
      })
      .sort((a, b) => b.getTime() - a.getTime()); // 从新到旧排序

    if (dates.length === 0) return 0;

    // 从最新日期开始向前计算连续天数
    let consecutiveDays = 0;
    const today = new Date();
    today.setHours(0, 0, 0, 0);
    
    // 从今天或最近的日期开始
    let expectedDate = new Date(today);
    
    // 如果今天没有运动，从最近的运动日期开始
    if (dates[0].getTime() !== today.getTime()) {
      expectedDate = new Date(dates[0]);
    }

    // 向前查找连续天数
    for (const date of dates) {
      if (date.getTime() === expectedDate.getTime()) {
        consecutiveDays++;
        expectedDate.setDate(expectedDate.getDate() - 1);
      } else if (consecutiveDays > 0) {
        // 如果已经找到连续天数，但当前日期不连续，则停止
        break;
      }
    }

    return consecutiveDays;
  }

  // 获取已解锁成就的映射
  private async getUnlockedAchievementsMap(): Promise<Map<string, UnlockedAchievementInfo>> {
    const map = new Map<string, UnlockedAchievementInfo>();

    if (!this.preferences) {
      return map;
    }

    try {
      const unlockedJson = await this.preferences.get(this.KEY_UNLOCKED_ACHIEVEMENTS, '{}');
      const unlockedDataObj: Record<string, UnlockedAchievementInfo> = JSON.parse(unlockedJson as string) as Record<string, UnlockedAchievementInfo>;
      
      const keys = Object.keys(unlockedDataObj);
      for (let i = 0; i < keys.length; i++) {
        const id = keys[i];
        const info = unlockedDataObj[id];
        if (info) {
          map.set(id, info);
        }
      }
    } catch (err) {
      console.error('获取已解锁成就失败:', err);
    }

    return map;
  }

  // 解锁成就
  private async unlockAchievement(id: string, unlockedTime: number): Promise<void> {
    if (!this.preferences) {
      console.error('成就服务未初始化');
      return;
    }

    try {
      const unlockedJson = await this.preferences.get(this.KEY_UNLOCKED_ACHIEVEMENTS, '{}');
      const unlockedDataObj: Record<string, UnlockedAchievementInfo> = JSON.parse(unlockedJson as string) as Record<string, UnlockedAchievementInfo>;
      
      // 创建新对象并添加新成就
      const updatedData: Record<string, UnlockedAchievementInfo> = {};
      const keys = Object.keys(unlockedDataObj);
      for (let i = 0; i < keys.length; i++) {
        const key = keys[i];
        updatedData[key] = unlockedDataObj[key];
      }
      updatedData[id] = { unlockedTime };
      
      await this.preferences.put(this.KEY_UNLOCKED_ACHIEVEMENTS, JSON.stringify(updatedData));
      await this.preferences.flush();
      console.log(`成就已解锁并保存: ${id}`);
    } catch (err) {
      console.error('解锁成就失败:', err);
    }
  }

  // 获取已解锁成就数量
  async getUnlockedCount(): Promise<number> {
    const achievements = await this.getAllAchievements();
    return achievements.filter(a => a.isUnlocked).length;
  }

  // 获取总成就数量
  getTotalCount(): number {
    return AchievementConfig.getAllAchievements().length;
  }
}
