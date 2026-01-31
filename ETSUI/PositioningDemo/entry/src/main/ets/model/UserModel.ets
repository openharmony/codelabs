/*
 * Copyright (c) 2023 公司名称
 * All rights reserved.
 *
 * 文件名称：example.cpp
 作者：作者名称
 * 创建日期：2023-01-01
 * 版本：1.0.0
 *
 * 描述：示文件的描述
 */
// 用户信息数据模型（纯本地存储）
export class UserInfo {
  nickname: string = '运动爱好者';
  signature: string = '坚持运动，享受健康';
  gender: string = '未设置'; // 男、女、未设置
  age: number = 0;
  height: number = 0; // 身高（cm）
  weight: number = 0; // 体重（kg）

  constructor() {}
}

// 用户设置数据模型
export class UserSettings {
  // 运动设置
  autoStart: boolean = false;
  voiceReminder: boolean = true;
  vibration: boolean = false;
  keepScreenOn: boolean = true;
  saveTrajectory: boolean = true;

  // 提醒设置
  enableDistanceReminder: boolean = true;
  distanceInterval: number = 1000; // 米
  enableTimeReminder: boolean = true;
  timeInterval: number = 300000; // 毫秒

  // 显示单位
  distanceUnit: string = 'km'; // km 或 mile
  speedUnit: string = 'kmh'; // kmh 或 mph

  constructor() {}
}

// 用户运动统计数据
export class UserStatistics {
  totalRecords: number = 0;
  totalDistance: number = 0; // 米
  totalDuration: number = 0; // 毫秒
  totalCalories: number = 0;
  averageSpeed: number = 0; // 米/秒
  maxSpeed: number = 0;
  longestDistance: number = 0;
  longestDuration: number = 0;

  // 按类型统计
  runningCount: number = 0;
  walkingCount: number = 0;
  cyclingCount: number = 0;
  runningDistance: number = 0;
  walkingDistance: number = 0;
  cyclingDistance: number = 0;

  constructor() {}
}

// 周统计数据
export class WeeklyStatistics {
  weekLabel: string = '';
  startDate: Date = new Date();
  endDate: Date = new Date();
  totalRecords: number = 0;
  totalDistance: number = 0;
  totalDuration: number = 0;
  totalCalories: number = 0;
  dailyRecords: DailyRecord[] = [];

  constructor() {}
}

// 月统计数据
export class MonthlyStatistics {
  monthLabel: string = '';
  year: number = 0;
  month: number = 0;
  totalRecords: number = 0;
  totalDistance: number = 0;
  totalDuration: number = 0;
  totalCalories: number = 0;
  weeklyData: WeeklyData[] = [];

  constructor() {}
}

// 每日记录
export class DailyRecord {
  date: Date = new Date();
  dayLabel: string = '';
  records: number = 0;
  distance: number = 0;
  duration: number = 0;
  calories: number = 0;

  constructor() {}
}

// 每周数据
export class WeeklyData {
  weekNumber: number = 0;
  records: number = 0;
  distance: number = 0;
  duration: number = 0;
  calories: number = 0;

  constructor() {}
}

// 运动类型统计
export class SportTypeStatistics {
  type: string = '';
  typeName: string = '';
  count: number = 0;
  totalDistance: number = 0;
  totalDuration: number = 0;
  totalCalories: number = 0;
  averageDistance: number = 0;
  averageDuration: number = 0;
  averageSpeed: number = 0;
  percentage: number = 0;

  constructor() {}
}

// 计算BMI
export function calculateBMI(height: number, weight: number): number {
  if (height <= 0 || weight <= 0) return 0;
  const heightInMeters = height / 100;
  return weight / (heightInMeters * heightInMeters);
}

// 获取BMI评价
export function getBMICategory(bmi: number): string {
  if (bmi === 0) return '未设置';
  if (bmi < 18.5) return '偏瘦';
  if (bmi < 24) return '正常';
  if (bmi < 28) return '偏胖';
  return '肥胖';
}
