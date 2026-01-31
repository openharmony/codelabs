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

// StepUtils.ets - 步数工具类 (保留此文件用于兼容性，推荐使用StepCounterService)
import { StepManager } from '../pages/StepManager';
import { StepRecord } from '../model/StepModel';
import common from '@ohos.app.ability.common';
import { StepCounterService } from '../services/StepCounterService';

export class StepUtils {
  private stepManager: StepManager;
  private stepCounterService: StepCounterService;

  constructor(context: common.Context) {
    this.stepManager = StepManager.getInstance(context);
    this.stepCounterService = StepCounterService.getInstance(context);
  }

  // 获取当前步数
  getCurrentSteps(): number {
    return this.stepCounterService.getCurrentSteps();
  }

  // 获取今天的真实步数数据（从传感器获取）
  async getTodayRealSteps(): Promise<number> {
    // 直接返回当前传感器获取的步数
    return this.stepCounterService.getCurrentSteps();
  }

  // 同步真实步数数据（现在主要是启动后台监听）
  async syncWithRealData(): Promise<void> {
    try {
      // 启动后台步数监听
      await this.stepCounterService.startBackgroundStepTracking();
    } catch (error) {
      console.error('同步真实步数数据失败:', error);
    }
  }

  // 生成过去几天的历史步数数据（仅用于初始化时填充历史数据）
  async generateHistoricalSteps(days: number = 7): Promise<void> {
    try {
      const today = new Date();

      for (let i = days - 1; i >= 0; i--) {
        const date = new Date(today);
        date.setDate(today.getDate() - i);

        const dateStr = this.formatDate(date);

        // 检查是否已经有该日期的记录
        const existingRecord = await this.stepManager.getStepRecordByDate(dateStr);
        if (existingRecord.success && existingRecord.data) {
          continue; // 如果已有记录，跳过
        }

        // 对于历史数据，我们暂时使用估算值（因为传感器只提供从设备启动以来的步数）
        // 在实际应用中，这里可以使用更复杂的算法或从云端同步历史数据
        const isWeekend = date.getDay() === 0 || date.getDay() === 6;
        const baseSteps = isWeekend ? 8000 : 6000; // 周末基础步数更高

        // 使用随机数生成模拟历史数据（实际应用中应该使用真实的传感器历史数据）
        const steps = Math.floor(baseSteps * (0.7 + Math.random() * 0.6)); // 70%-130%之间的随机值

        // 计算卡路里和距离
        const calories = this.calculateCalories(steps);
        const distance = this.calculateDistance(steps);

        // 保存记录
        const stepData: Partial<StepRecord> = {
          date: dateStr,
          steps: steps,
          calories: calories,
          distance: distance
        };

        await this.stepManager.saveStepRecord(stepData);
      }
    } catch (error) {
      console.error('生成历史步数数据失败:', error);
    }
  }

  // 计算卡路里消耗（估算值）
  private calculateCalories(steps: number): number {
    // 每1000步大约消耗40-60卡路里，这里取中间值50
    return Math.round((steps / 1000) * 50 * 100) / 100; // 保留两位小数
  }

  // 计算行走距离（估算值）
  private calculateDistance(steps: number): number {
    // 平均步幅约为0.7米
    return Math.round(steps * 0.7 * 100) / 100; // 保留两位小数
  }

  // 格式化日期为 YYYY-MM-DD 格式
  private formatDate(date: Date): string {
    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, '0');
    const day = String(date.getDate()).padStart(2, '0');
    return `${year}-${month}-${day}`;
  }
}