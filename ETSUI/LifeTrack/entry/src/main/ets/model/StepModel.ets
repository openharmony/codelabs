// StepModel.ets - 步数记录数据模型
import { OperationResult } from './ContactModel';

// 步数记录接口
export interface StepRecord {
  id: string;           // 主键ID
  date: string;         // 记录日期 (YYYY-MM-DD)
  steps: number;        // 步数
  calories: number;     // 消耗卡路里
  distance: number;     // 行走距离(米)
  created: number;      // 创建时间戳
  updated: number;      // 更新时间戳
}

// 步数统计数据接口
export interface StepStats {
  totalSteps: number;   // 总步数
  avgSteps: number;     // 平均步数
  maxSteps: number;     // 最大单日步数
  daysReachedGoal: number; // 达标天数
  currentStreak: number;   // 当前连续达标天数
  longestStreak: number;   // 最长连续达标天数
}

// 步数目标接口
export interface StepGoal {
  dailyGoal: number;    // 每日目标步数
  isActive: boolean;    // 是否激活目标
  goalMet: boolean;     // 当日是否达成目标
}

// 步数记录结果接口
export interface StepResult {
  success: boolean;
  data?: StepRecord | StepRecord[] | StepStats;
  error?: string;
}

// 步数趋势数据接口
export interface StepTrend {
  date: string;         // 日期
  steps: number;        // 步数
  goalMet: boolean;     // 是否达标
}