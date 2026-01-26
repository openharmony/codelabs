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

import { StepManager } from '../pages/StepManager';
import { StepRecord } from '../model/StepModel';
import { sensor } from '@kit.SensorServiceKit';
import abilityAccessCtrl from '@ohos.abilityAccessCtrl';
import common from '@ohos.app.ability.common';
import { BusinessError } from '@kit.BasicServicesKit';

// 步数计数器服务类，用于后台持续监听步数
export class StepCounterService {
  private static instance: StepCounterService | null = null;
  private stepManager: StepManager;
  private initialStepCount: number = 0; // 设备重启后的初始步数
  private currentStepCount: number = 0; // 当前步数
  private isSubscribed: boolean = false; // 是否已经订阅传感器
  private context: common.Context;
  private listeners: Array<(steps: number) => void> = []; // 步数变化监听器

  private constructor(context: common.Context) {
    this.context = context;
    this.stepManager = StepManager.getInstance(context);
  }

  // 单例模式获取实例
  public static getInstance(context: common.Context): StepCounterService {
    if (!StepCounterService.instance) {
      StepCounterService.instance = new StepCounterService(context);
    }
    return StepCounterService.instance;
  }

  // 请求运动权限
  private async requestMotionPermission(): Promise<boolean> {
    const atManager = abilityAccessCtrl.createAtManager();
    try {
      const result = await atManager.requestPermissionsFromUser(
        this.context,
        ['ohos.permission.ACTIVITY_MOTION']
      );
      return result.permissions[0] === 'ohos.permission.ACTIVITY_MOTION' &&
        result.authResults[0] === 0;
    } catch (err) {
      console.error('申请权限失败:', err);
      return false;
    }
  }

  // 添加步数变化监听器
  public addStepChangeListener(listener: (steps: number) => void): void {
    this.listeners.push(listener);
  }

  // 移除步数变化监听器
  public removeStepChangeListener(listener: (steps: number) => void): void {
    const index = this.listeners.indexOf(listener);
    if (index > -1) {
      this.listeners.splice(index, 1);
    }
  }

  // 通知所有监听器步数变化
  private notifyStepChange(steps: number): void {
    this.listeners.forEach(listener => {
      try {
        listener(steps);
      } catch (error) {
        console.error('步数监听器执行错误:', error);
      }
    });
  }

  // 开始后台步数监听
  public async startBackgroundStepTracking(): Promise<boolean> {
    try {
      const hasPermission = await this.requestMotionPermission();
      if (!hasPermission) {
        console.error('无法开始步数跟踪，缺少权限');
        return false;
      }

      if (this.isSubscribed) {
        console.info('步数监听已在运行中');
        return true;
      }

      const callback = (data: sensor.PedometerResponse) => {
        const totalSteps = data.steps;
        
        // 首次初始化，设置初始步数
        if (this.initialStepCount === 0) {
          this.initialStepCount = totalSteps;
          this.currentStepCount = 0;
          console.info(`步数传感器初始化成功，初始步数: ${this.initialStepCount}`);
        } else {
          // 计算相对于初始值的步数
          const newStepCount = Math.max(0, totalSteps - this.initialStepCount);
          
          // 如果步数发生变化，则更新
          if (newStepCount !== this.currentStepCount) {
            this.currentStepCount = newStepCount;
            console.info(`步数更新: ${this.currentStepCount}`);
            
            // 通知所有监听器
            this.notifyStepChange(this.currentStepCount);
            
            // 自动保存当前步数
            this.saveCurrentSteps();
          }
        }
      };

      sensor.on(sensor.SensorId.PEDOMETER, callback);
      this.isSubscribed = true;
      console.info('开始后台步数监听');
      return true;
    } catch (error) {
      console.error('开始后台步数监听失败:', error);
      return false;
    }
  }

  // 停止后台步数监听
  public stopBackgroundStepTracking(): void {
    if (this.isSubscribed) {
      sensor.off(sensor.SensorId.PEDOMETER);
      this.isSubscribed = false;
      console.info('停止后台步数监听');
    }
  }

  // 保存当前步数到数据库
  private async saveCurrentSteps(): Promise<void> {
    try {
      const today = this.formatDate(new Date());
      
      // 获取今天已有的步数记录
      const existingRecord = await this.stepManager.getStepRecordByDate(today);
      
      if (existingRecord.success && existingRecord.data) {
        // 如果今天已有记录，更新它
        const existingData = existingRecord.data as StepRecord;
        const updatedRecord: Partial<StepRecord> = {
          id: existingData.id,
          date: today,
          steps: this.currentStepCount,
          calories: this.calculateCalories(this.currentStepCount),
          distance: this.calculateDistance(this.currentStepCount),
          updated: Date.now()
        };
        
        await this.stepManager.saveStepRecord(updatedRecord);
      } else {
        // 如果今天没有记录，创建新记录
        const newRecord: Partial<StepRecord> = {
          date: today,
          steps: this.currentStepCount,
          calories: this.calculateCalories(this.currentStepCount),
          distance: this.calculateDistance(this.currentStepCount)
        };
        
        await this.stepManager.saveStepRecord(newRecord);
      }
    } catch (error) {
      console.error('保存步数失败:', error);
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

  // 获取当前步数
  public getCurrentSteps(): number {
    return this.currentStepCount;
  }

  // 检查是否正在监听
  public isTracking(): boolean {
    return this.isSubscribed;
  }
}