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
// 每日任务
// 单个每日任务可以同时包含多种类型的目标（距离 / 时长 / 卡路里 / 次数）
export class DailyTask {
  // 目标值（为 0 表示该类型未设置）
  distanceTarget: number = 0; // 距离目标（米）
  durationTarget: number = 0; // 时长目标（与统计中的单位保持一致）
  caloriesTarget: number = 0; // 卡路里目标
  recordsTarget: number = 0; // 运动次数目标

  // 已完成值（用于在详情中展示最近一次统计结果）
  distanceCompleted: number = 0;
  durationCompleted: number = 0;
  caloriesCompleted: number = 0;
  recordsCompleted: number = 0;
  // 是否完成（当天所有已设置目标是否都已达成）
  isCompleted: boolean = false;

  constructor() {
    // 默认全部为 0，调用方在保存前需要保证至少有一个目标类型 > 0
  }
}

// 计划状态
export enum PlanStatus {
  ACTIVE = 'active', // 进行中
  COMPLETED = 'completed', // 已完成
  PAUSED = 'paused', // 已暂停
  CANCELLED = 'cancelled' // 已取消
}

// 训练计划模型
export class Plan {
  id: string = ''; // 计划ID（时间戳）
  name: string = ''; // 计划名称
  description: string = ''; // 计划描述
  goal: string = ''; // 计划目标（如：减重5kg、提升耐力等）
  startDate: number = 0; // 开始日期（时间戳）
  endDate: number = 0; // 结束日期（时间戳）
  dailyTasks: DailyTask[] = []; // 每日任务列表
  status: PlanStatus = PlanStatus.ACTIVE; // 计划状态
  createdTime: number = 0; // 创建时间（时间戳）
  completedDays: number = 0; // 已完成天数
  totalDays: number = 0; // 总天数
  completionRate: number = 0; // 完成度（0-100）

  constructor(
    name: string,
    description: string,
    goal: string,
    startDate: number,
    endDate: number,
    dailyTasks: DailyTask[]
  ) {
    this.id = Date.now().toString();
    this.name = name;
    this.description = description;
    this.goal = goal;
    this.startDate = startDate;
    this.endDate = endDate;
    this.dailyTasks = dailyTasks;
    this.status = PlanStatus.ACTIVE;
    this.createdTime = Date.now();
    this.completedDays = 0;
    this.totalDays = this.calculateTotalDays();
    this.completionRate = 0;
  }

  // 计算总天数
  calculateTotalDays(): number {
    const start = new Date(this.startDate);
    const end = new Date(this.endDate);
    const diffTime = Math.abs(end.getTime() - start.getTime());
    const diffDays = Math.ceil(diffTime / (1000 * 60 * 60 * 24));
    return diffDays + 1; // 包含开始和结束日期
  }

  // 获取当前进行中的天数
  getCurrentDay(): number {
    const today = new Date();
    today.setHours(0, 0, 0, 0);
    const start = new Date(this.startDate);
    start.setHours(0, 0, 0, 0);

    if (today < start) {
      return 0; // 还未开始
    }

    const diffTime = today.getTime() - start.getTime();
    const diffDays = Math.floor(diffTime / (1000 * 60 * 60 * 24));
    return Math.min(diffDays + 1, this.totalDays);
  }

  // 检查计划是否已过期
  isExpired(): boolean {
    const today = new Date();
    today.setHours(0, 0, 0, 0);
    const end = new Date(this.endDate);
    end.setHours(23, 59, 59, 999);
    return today > end;
  }

  // 检查计划是否已开始
  isStarted(): boolean {
    const today = new Date();
    today.setHours(0, 0, 0, 0);
    const start = new Date(this.startDate);
    start.setHours(0, 0, 0, 0);
    return today >= start;
  }
}
