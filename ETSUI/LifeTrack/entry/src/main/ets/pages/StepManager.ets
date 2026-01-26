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

import { StepDao } from '../dao/StepDao';
import { StepRecord, StepStats, StepResult, StepTrend, StepGoal } from '../model/StepModel';
import common from '@ohos.app.ability.common';

// 连续达标天数结果接口
interface StreakResult {
  currentStreak: number;
  longestStreak: number;
}

export class StepManager {
  private stepDao: StepDao;
  private static instance: StepManager | null = null;

  private constructor(context: common.Context) {
    this.stepDao = new StepDao(context);
  }

  // 单例模式获取实例
  static getInstance(context?: common.Context): StepManager {
    if (!StepManager.instance && context) {
      StepManager.instance = new StepManager(context);
    }
    return StepManager.instance!;
  }

  // 保存步数记录
  async saveStepRecord(stepData: Partial<StepRecord>): Promise<StepResult> {
    try {
      // 验证必要字段
      if (!stepData.date) {
        return { success: false, error: '日期不能为空' };
      }

      if (stepData.steps === undefined || stepData.steps < 0) {
        return { success: false, error: '步数必须大于等于0' };
      }

      // 构建完整的步数记录对象
      const now = Date.now();
      const step: StepRecord = {
        id: stepData.id || '0',
        date: stepData.date,
        steps: stepData.steps,
        calories: stepData.calories ?? this.calculateCalories(stepData.steps),
        distance: stepData.distance ?? this.calculateDistance(stepData.steps),
        created: stepData.id && stepData.created ? stepData.created : now,
        updated: now
      };

      // 保存到数据库
      const savedId = await this.stepDao.saveStepRecord(step);

      // 重新获取保存的记录以返回完整数据
      const savedRecord = await this.stepDao.getStepRecordById(savedId);

      if (savedRecord) {
        return { success: true, data: savedRecord };
      } else {
        return { success: false, error: '保存失败' };
      }

    } catch (error) {
      console.error('Save step record failed:', error);
      return {
        success: false,
        error: `保存失败: ${error instanceof Error ? error.message : '未知错误'}`
      };
    }
  }

  // 根据日期获取步数记录
  async getStepRecordByDate(date: string): Promise<StepResult> {
    try {
      if (!date) {
        return { success: false, error: '无效的日期' };
      }

      const step = await this.stepDao.getStepRecordByDate(date);

      if (!step) {
        return { success: true, data: undefined }; // 日期没有记录是正常的
      }

      return { success: true, data: step };

    } catch (error) {
      console.error('Get step record by date failed:', error);
      return {
        success: false,
        error: `查询失败: ${error instanceof Error ? error.message : '未知错误'}`
      };
    }
  }

  // 获取指定日期范围内的步数记录
  async getStepRecordsByDateRange(startDate: string, endDate: string): Promise<StepResult> {
    try {
      if (!startDate || !endDate) {
        return { success: false, error: '开始日期和结束日期不能为空' };
      }

      const records = await this.stepDao.getStepRecordsByDateRange(startDate, endDate);

      return { success: true, data: records };

    } catch (error) {
      console.error('Get step records by date range failed:', error);
      return {
        success: false,
        error: `查询失败: ${error instanceof Error ? error.message : '未知错误'}`
      };
    }
  }

  // 获取最近的步数记录
  async getRecentStepRecords(limit: number = 7): Promise<StepResult> {
    try {
      const records = await this.stepDao.getRecentStepRecords(limit);

      return { success: true, data: records };

    } catch (error) {
      console.error('Get recent step records failed:', error);
      return {
        success: false,
        error: `查询失败: ${error instanceof Error ? error.message : '未知错误'}`
      };
    }
  }

  // 获取所有步数记录
  async getAllStepRecords(): Promise<StepResult> {
    try {
      const records = await this.stepDao.getAllStepRecords();

      return { success: true, data: records };

    } catch (error) {
      console.error('Get all step records failed:', error);
      return {
        success: false,
        error: `查询失败: ${error instanceof Error ? error.message : '未知错误'}`
      };
    }
  }

  // 计算卡路里消耗（估算值，基于平均值）
  private calculateCalories(steps: number): number {
    // 每1000步大约消耗40-60卡路里，这里取中间值50
    return Math.round((steps / 1000) * 50 * 100) / 100; // 保留两位小数
  }

  // 计算行走距离（估算值，基于平均步幅）
  private calculateDistance(steps: number): number {
    // 平均步幅约为0.7米
    return Math.round(steps * 0.7 * 100) / 100; // 保留两位小数
  }

  // 获取步数统计数据
  async getStepStats(days: number = 30): Promise<StepStats> {
    try {
      // 获取过去指定天数的记录
      const now = new Date();
      const startDate = new Date();
      startDate.setDate(now.getDate() - days);

      const startDateStr = this.formatDate(startDate);
      const endDateStr = this.formatDate(now);

      const result = await this.getStepRecordsByDateRange(startDateStr, endDateStr);

      if (!result.success || !result.data) {
        return {
          totalSteps: 0,
          avgSteps: 0,
          maxSteps: 0,
          daysReachedGoal: 0,
          currentStreak: 0,
          longestStreak: 0
        };
      }

      const records = result.data as StepRecord[];

      if (records.length === 0) {
        return {
          totalSteps: 0,
          avgSteps: 0,
          maxSteps: 0,
          daysReachedGoal: 0,
          currentStreak: 0,
          longestStreak: 0
        };
      }

      // 计算统计数据
      const totalSteps = records.reduce((sum, record) => sum + record.steps, 0);
      const avgSteps = Math.round(totalSteps / records.length);
      const maxSteps = Math.max(...records.map(record => record.steps), 0);

      // 计算达标天数（假设每日目标为8000步）
      const dailyGoal = 8000;
      const daysReachedGoal = records.filter(record => record.steps >= dailyGoal).length;

      // 计算连续达标天数
      const streakResult = this.calculateStreaks(records, dailyGoal);
      const currentStreak = streakResult.currentStreak;
      const longestStreak = streakResult.longestStreak;

      return {
        totalSteps,
        avgSteps,
        maxSteps,
        daysReachedGoal,
        currentStreak,
        longestStreak
      };
    } catch (error) {
      console.error('Get step stats failed:', error);
      return {
        totalSteps: 0,
        avgSteps: 0,
        maxSteps: 0,
        daysReachedGoal: 0,
        currentStreak: 0,
        longestStreak: 0
      };
    }
  }

  // 计算连续达标天数
  private calculateStreaks(records: StepRecord[], dailyGoal: number): StreakResult {
    if (records.length === 0) {
      return { currentStreak: 0, longestStreak: 0 };
    }

    // 按日期排序（从最新到最旧）
    const sortedRecords = [...records].sort((a, b) =>
      new Date(b.date).getTime() - new Date(a.date).getTime()
    );

    let currentStreak = 0;
    let maxStreak = 0;
    let tempStreak = 0;

    // 从最新的日期开始计算当前连续达标天数
    for (let i = 0; i < sortedRecords.length; i++) {
      const isGoalMet = sortedRecords[i].steps >= dailyGoal;

      if (isGoalMet) {
        tempStreak++;
        if (tempStreak > maxStreak) maxStreak = tempStreak;
      } else {
        // 如果今天没达标，当前连续天数为0
        if (i === 0) { // 最新的一天
          tempStreak = 0;
        } else {
          // 不重置tempStreak，因为我们要找的是整体最长连续天数
          break; // 只计算到最新的连续序列
        }
      }
    }

    // 检查是否是连续的（今天是否达标）
    if (sortedRecords[0].steps >= dailyGoal) {
      // 从今天开始向前检查连续达标天数
      currentStreak = 0;
      for (const record of sortedRecords) {
        if (record.steps >= dailyGoal) {
          currentStreak++;
        } else {
          break;
        }
      }
    } else {
      currentStreak = 0;
    }

    return { currentStreak, longestStreak: maxStreak };
  }

  // 获取步数趋势数据
  async getStepTrends(days: number = 7): Promise<StepTrend[]> {
    try {
      const now = new Date();
      const startDate = new Date();
      startDate.setDate(now.getDate() - (days - 1));

      const startDateStr = this.formatDate(startDate);
      const endDateStr = this.formatDate(now);

      const result = await this.getStepRecordsByDateRange(startDateStr, endDateStr);

      if (!result.success || !result.data) {
        return [];
      }

      const records = result.data as StepRecord[];
      const dailyGoal = 8000; // 假设每日目标为8000步

      // 按日期排序
      const sortedRecords = records.sort((a, b) =>
        new Date(a.date).getTime() - new Date(b.date).getTime()
      );

      // 生成日期范围内每一天的数据，包括没有记录的日期
      const trends: StepTrend[] = [];
      const currentDate = new Date(startDate);

      while (currentDate <= now) {
        const dateStr = this.formatDate(currentDate);
        const record = sortedRecords.find(r => r.date === dateStr);

        trends.push({
          date: dateStr,
          steps: record ? record.steps : 0,
          goalMet: record ? record.steps >= dailyGoal : false
        });

        currentDate.setDate(currentDate.getDate() + 1);
      }

      return trends;
    } catch (error) {
      console.error('Get step trends failed:', error);
      return [];
    }
  }

  // 检查当日是否已记录步数
  async hasTodayRecord(): Promise<boolean> {
    try {
      const today = this.formatDate(new Date());
      const result = await this.getStepRecordByDate(today);

      return result.success && result.data !== null;
    } catch (error) {
      console.error('Check today record failed:', error);
      return false;
    }
  }

  // 格式化日期为 YYYY-MM-DD 格式
  private formatDate(date: Date): string {
    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, '0');
    const day = String(date.getDate()).padStart(2, '0');
    return `${year}-${month}-${day}`;
  }

  // 获取步数目标状态
  async getStepGoalStatus(date: string): Promise<StepGoal> {
    try {
      const record = await this.getStepRecordByDate(date);

      if (!record.success || !record.data) {
        return {
          dailyGoal: 8000,
          isActive: true,
          goalMet: false
        };
      }

      const stepRecord = record.data as StepRecord;
      const dailyGoal = 8000; // 默认每日目标8000步

      return {
        dailyGoal: dailyGoal,
        isActive: true,
        goalMet: stepRecord.steps >= dailyGoal
      };
    } catch (error) {
      console.error('Get step goal status failed:', error);
      return {
        dailyGoal: 8000,
        isActive: true,
        goalMet: false
      };
    }
  }
}