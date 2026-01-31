/*
 * Copyright (c) 2023 Your Company Name
 * Licensed under the Apache License, Version 2.0 (the");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *      http://www.apache.org/licenses/LICENSE-20
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY, either express or implied.
 * the License for the specific language governing permissions and
 * limitations under the License.
 */
import { Plan, PlanStatus, DailyTask } from '../model/PlanModel';
import { HistoryService } from './HistoryService';
import { SportData } from '../model/SportModel';
import { preferences } from '@kit.ArkData';
import { common } from '@kit.AbilityKit';

// 计划数据接口（用于序列化）
interface PlanData {
  id: string;
  name: string;
  description: string;
  goal: string;
  startDate: number;
  endDate: number;
  dailyTasks: TaskData[];
  status: PlanStatus;
  createdTime: number;
  completedDays: number;
  totalDays: number;
  completionRate: number;
}

// 任务数据接口（用于序列化）
interface TaskData {
  distanceTarget: number;
  durationTarget: number;
  caloriesTarget: number;
  recordsTarget: number;
  distanceCompleted: number;
  durationCompleted: number;
  caloriesCompleted: number;
  recordsCompleted: number;
  isCompleted: boolean;
}

// 计划统计信息接口
interface PlanStatistics {
  totalPlans: number;
  activePlans: number;
  completedPlans: number;
  averageCompletionRate: number;
}

// 每日统计数据接口
interface DayStatistics {
  distance: number;
  duration: number;
  calories: number;
  records: number;
}

export class PlanService {
  private preferences: preferences.Preferences | null = null;
  private readonly KEY_PLANS = 'training_plans';
  private historyService: HistoryService;

  constructor(historyService: HistoryService) {
    this.historyService = historyService;
  }

  async init(context: common.Context): Promise<void> {
    try {
      this.preferences = await preferences.getPreferences(context, 'plans');
      console.log('训练计划服务初始化成功');
    } catch (err) {
      console.error('初始化训练计划服务失败:', err);
    }
  }

  // 创建计划
  async createPlan(plan: Plan): Promise<void> {
    if (!this.preferences) {
      console.error('训练计划服务未初始化');
      return;
    }

    try {
      const plans = await this.getAllPlans();
      plans.push(plan);
      await this.savePlans(plans);
      console.log(`计划已创建: ${plan.name}`);
    } catch (err) {
      console.error('创建计划失败:', err);
      throw new Error(`创建计划失败: ${err}`);
    }
  }

  // 获取所有计划
  async getAllPlans(): Promise<Plan[]> {
    if (!this.preferences) {
      return [];
    }

    try {
      const plansJson = await this.preferences.get(this.KEY_PLANS, '[]');
      const plansData: PlanData[] = JSON.parse(plansJson as string) as PlanData[];
      
      const plans: Plan[] = [];
      for (let i = 0; i < plansData.length; i++) {
        const planData = plansData[i];
        const plan = this.deserializePlan(planData);
        // 更新计划进度
        await this.updatePlanProgress(plan);
        plans.push(plan);
      }
      // 将最新的进度结果持久化，保证列表页与详情页显示一致
      await this.savePlans(plans);
      return plans;
    } catch (err) {
      console.error('获取计划列表失败:', err);
      return [];
    }
  }

  // 获取计划详情
  async getPlanById(id: string): Promise<Plan | null> {
    const plans = await this.getAllPlans();
    const plan = plans.find(p => p.id === id);
    if (plan) {
      await this.updatePlanProgress(plan);
      return plan;
    }
    return null;
  }

  // 更新计划
  async updatePlan(plan: Plan): Promise<void> {
    if (!this.preferences) {
      console.error('训练计划服务未初始化');
      return;
    }

    try {
      const plans = await this.getAllPlans();
      const index = plans.findIndex(p => p.id === plan.id);
      if (index !== -1) {
        plans[index] = plan;
        await this.savePlans(plans);
        console.log(`计划已更新: ${plan.name}`);
      }
    } catch (err) {
      console.error('更新计划失败:', err);
      throw new Error(`更新计划失败: ${err}`);
    }
  }

  // 删除计划
  async deletePlan(id: string): Promise<void> {
    if (!this.preferences) {
      console.error('训练计划服务未初始化');
      return;
    }

    try {
      const plans = await this.getAllPlans();
      const filteredPlans = plans.filter(p => p.id !== id);
      await this.savePlans(filteredPlans);
      console.log(`计划已删除: ${id}`);
    } catch (err) {
      console.error('删除计划失败:', err);
      throw new Error(`删除计划失败: ${err}`);
    }
  }

  // 更新计划进度
  async updatePlanProgress(plan: Plan): Promise<void> {
    if (plan.status === PlanStatus.COMPLETED || plan.status === PlanStatus.CANCELLED) {
      return; // 已完成或已取消的计划不再更新进度
    }

    // 检查计划是否已过期
    if (plan.isExpired()) {
      plan.status = PlanStatus.COMPLETED;
      plan.completionRate = 100;
      return;
    }

    // 获取计划日期范围内的运动记录
    const records = await this.historyService.getSportRecords();
    const planRecords = records.filter(record => {
      const recordDate = new Date(record.startTime);
      const startDate = new Date(plan.startDate);
      const endDate = new Date(plan.endDate);
      startDate.setHours(0, 0, 0, 0);
      endDate.setHours(23, 59, 59, 999);
      return recordDate >= startDate && recordDate <= endDate;
    });

    // 按日期分组统计
    const dateMap = new Map<string, SportData[]>();
    planRecords.forEach(record => {
      const date = new Date(record.startTime);
      const dateKey = `${date.getFullYear()}-${date.getMonth() + 1}-${date.getDate()}`;
      if (!dateMap.has(dateKey)) {
        dateMap.set(dateKey, []);
      }
      dateMap.get(dateKey)!.push(record);
    });

    // 计算已完成天数
    let completedDays = 0;
    const today = new Date();
    today.setHours(0, 0, 0, 0);
    const startDate = new Date(plan.startDate);
    startDate.setHours(0, 0, 0, 0);
    const endDate = new Date(plan.endDate);
    endDate.setHours(23, 59, 59, 999);

    // 遍历计划期间的每一天
    const currentDate = new Date(startDate);
    while (currentDate <= endDate && currentDate <= today) {
      const dateKey = `${currentDate.getFullYear()}-${currentDate.getMonth() + 1}-${currentDate.getDate()}`;
      const dayRecords = dateMap.get(dateKey) || [];

      // 检查当天的任务是否完成
      if (this.checkDayTasksCompleted(plan.dailyTasks, dayRecords)) {
        completedDays++;
      }

      currentDate.setDate(currentDate.getDate() + 1);
    }

    plan.completedDays = completedDays;
    plan.completionRate = plan.totalDays > 0 ? Math.round((completedDays / plan.totalDays) * 100) : 0;

    // 如果完成度达到100%，标记为已完成
    if (plan.completionRate >= 100 && plan.isExpired()) {
      plan.status = PlanStatus.COMPLETED;
    }
  }

  // 检查当天的任务是否完成
  private checkDayTasksCompleted(tasks: DailyTask[], records: SportData[]): boolean {
    if (tasks.length === 0) {
      return false; // 没有任务，不算完成
    }

    // 计算当天的统计数据
    const dayStats: DayStatistics = {
      distance: records.reduce((sum, r) => sum + r.distance, 0),
      duration: records.reduce((sum, r) => sum + r.duration, 0),
      calories: records.reduce((sum, r) => sum + r.calories, 0),
      records: records.length
    };

    // 检查每个任务是否完成
    for (const task of tasks) {
      let hasAnyTarget = false;
      let allTargetsCompleted = true;

      // 距离目标
      task.distanceCompleted = dayStats.distance;
      if (task.distanceTarget > 0) {
        hasAnyTarget = true;
        if (dayStats.distance < task.distanceTarget) {
          allTargetsCompleted = false;
        }
      }

      // 时长目标（durationTarget 以“分钟”为单位，需要换算成毫秒比较）
      task.durationCompleted = dayStats.duration; // 记录当天总时长（毫秒）
      if (task.durationTarget > 0) {
        hasAnyTarget = true;
        const targetDurationMs = task.durationTarget * 60 * 1000;
        if (dayStats.duration < targetDurationMs) {
          allTargetsCompleted = false;
        }
      }

      // 卡路里目标
      task.caloriesCompleted = dayStats.calories;
      if (task.caloriesTarget > 0) {
        hasAnyTarget = true;
        if (dayStats.calories < task.caloriesTarget) {
          allTargetsCompleted = false;
        }
      }

      // 次数目标
      task.recordsCompleted = dayStats.records;
      if (task.recordsTarget > 0) {
        hasAnyTarget = true;
        if (dayStats.records < task.recordsTarget) {
          allTargetsCompleted = false;
        }
      }

      // 必须至少设置一个目标类型，并且所有已设置的目标都达成，任务才算完成
      task.isCompleted = hasAnyTarget && allTargetsCompleted;

      // 只要有一个任务未完成，当天不算完成
      if (!task.isCompleted) {
        return false;
      }
    }

    return true; // 所有任务（及其已设置的目标）都完成
  }

  // 获取进行中的计划数量
  async getActivePlansCount(): Promise<number> {
    const plans = await this.getAllPlans();
    return plans.filter(p => p.status === PlanStatus.ACTIVE).length;
  }

  // 获取已完成计划数量
  async getCompletedPlansCount(): Promise<number> {
    const plans = await this.getAllPlans();
    return plans.filter(p => p.status === PlanStatus.COMPLETED).length;
  }

  // 获取计划统计信息
  async getPlanStatistics(): Promise<PlanStatistics> {
    const plans = await this.getAllPlans();
    const activePlans = plans.filter(p => p.status === PlanStatus.ACTIVE);
    const completedPlans = plans.filter(p => p.status === PlanStatus.COMPLETED);
    
    const totalCompletionRate = plans.reduce((sum, p) => sum + p.completionRate, 0);
    const averageCompletionRate = plans.length > 0 ? Math.round(totalCompletionRate / plans.length) : 0;

    return {
      totalPlans: plans.length,
      activePlans: activePlans.length,
      completedPlans: completedPlans.length,
      averageCompletionRate
    };
  }

  // 保存计划列表
  private async savePlans(plans: Plan[]): Promise<void> {
    if (!this.preferences) {
      return;
    }

    try {
      const plansData = plans.map(plan => this.serializePlan(plan));
      await this.preferences.put(this.KEY_PLANS, JSON.stringify(plansData));
      await this.preferences.flush();
    } catch (err) {
      console.error('保存计划列表失败:', err);
      throw new Error(`保存计划列表失败: ${err}`);
    }
  }

  // 序列化计划
  private serializePlan(plan: Plan): PlanData {
    return {
      id: plan.id,
      name: plan.name,
      description: plan.description,
      goal: plan.goal,
      startDate: plan.startDate,
      endDate: plan.endDate,
      dailyTasks: plan.dailyTasks.map((task): TaskData => ({
        distanceTarget: task.distanceTarget,
        durationTarget: task.durationTarget,
        caloriesTarget: task.caloriesTarget,
        recordsTarget: task.recordsTarget,
        distanceCompleted: task.distanceCompleted,
        durationCompleted: task.durationCompleted,
        caloriesCompleted: task.caloriesCompleted,
        recordsCompleted: task.recordsCompleted,
        isCompleted: task.isCompleted
      })),
      status: plan.status,
      createdTime: plan.createdTime,
      completedDays: plan.completedDays,
      totalDays: plan.totalDays,
      completionRate: plan.completionRate
    };
  }

  // 反序列化计划
  private deserializePlan(data: PlanData): Plan {
    const plan = new Plan(
      data.name || '',
      data.description || '',
      data.goal || '',
      data.startDate || 0,
      data.endDate || 0,
      (data.dailyTasks || []).map((taskData: TaskData) => {
        const task = new DailyTask();
        task.distanceTarget = taskData.distanceTarget || 0;
        task.durationTarget = taskData.durationTarget || 0;
        task.caloriesTarget = taskData.caloriesTarget || 0;
        task.recordsTarget = taskData.recordsTarget || 0;
        task.distanceCompleted = taskData.distanceCompleted || 0;
        task.durationCompleted = taskData.durationCompleted || 0;
        task.caloriesCompleted = taskData.caloriesCompleted || 0;
        task.recordsCompleted = taskData.recordsCompleted || 0;
        task.isCompleted = taskData.isCompleted || false;
        return task;
      })
    );
    
    plan.id = data.id || plan.id;
    plan.status = data.status || PlanStatus.ACTIVE;
    plan.createdTime = data.createdTime || plan.createdTime;
    plan.completedDays = data.completedDays || 0;
    plan.totalDays = data.totalDays || plan.totalDays;
    plan.completionRate = data.completionRate || 0;
    
    return plan;
  }
}
