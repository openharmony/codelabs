// SPDX-License-Identifier: MIT
// 提醒类型枚举
export enum ReminderType {
  SPORT_START = 'sport_start',      // 运动开始提醒
  SPORT_DURATION = 'sport_duration', // 运动时长提醒
  GOAL_ACHIEVEMENT = 'goal_achievement', // 目标达成提醒
  REST_REMINDER = 'rest_reminder',   // 休息提醒
  REGULAR_SPORT = 'regular_sport'    // 定期运动提醒
}

// 提醒配置接口
export interface ReminderConfig {
  type: ReminderType;       // 提醒类型
  enabled: boolean;         // 是否启用
  interval?: number;        // 提醒间隔（秒）
  durationThreshold?: number; // 时长阈值（分钟）
  message: string;          // 提醒消息
  soundEnabled?: boolean;   // 是否启用声音
  vibrationEnabled?: boolean; // 是否启用震动
}

// 运动目标接口
export interface SportGoal {
  distance?: number;        // 距离目标（米）
  duration?: number;        // 时长目标（分钟）
  calories?: number;        // 卡路里目标
}

// 提醒数据类
export class ReminderData {
  id: string;               // 提醒ID
  type: ReminderType;       // 提醒类型
  message: string;          // 提醒消息
  timestamp: number;        // 提醒时间戳
  isRead: boolean;          // 是否已读

  constructor(id: string, type: ReminderType, message: string, timestamp: number) {
    this.id = id;
    this.type = type;
    this.message = message;
    this.timestamp = timestamp;
    this.isRead = false;
  }
}

// 全局目标存储，确保所有实例共享同一个目标
let globalGoal: SportGoal = {};

// 提醒设置类
export class ReminderSettings {
  private settings: Map<ReminderType, ReminderConfig> = new Map();

  constructor() {
    // 初始化默认提醒设置
    this.initializeDefaultSettings();
  }

  // 初始化默认提醒设置
  private initializeDefaultSettings(): void {
    this.settings.set(ReminderType.SPORT_START, {
      type: ReminderType.SPORT_START,
      enabled: true,
      message: '运动即将开始，请做好准备',
      soundEnabled: true,
      vibrationEnabled: true
    });

    this.settings.set(ReminderType.SPORT_DURATION, {
      type: ReminderType.SPORT_DURATION,
      enabled: true,
      interval: 1800, // 30分钟
      message: '您已运动一段时间，继续保持',
      soundEnabled: true,
      vibrationEnabled: false
    });

    this.settings.set(ReminderType.GOAL_ACHIEVEMENT, {
      type: ReminderType.GOAL_ACHIEVEMENT,
      enabled: true,
      message: '恭喜！您已达成运动目标',
      soundEnabled: true,
      vibrationEnabled: true
    });

    this.settings.set(ReminderType.REST_REMINDER, {
      type: ReminderType.REST_REMINDER,
      enabled: true,
      durationThreshold: 60, // 60分钟
      message: '建议您适当休息，避免过度运动',
      soundEnabled: true,
      vibrationEnabled: false
    });

    this.settings.set(ReminderType.REGULAR_SPORT, {
      type: ReminderType.REGULAR_SPORT,
      enabled: false,
      interval: 86400, // 24小时
      message: '该进行今天的运动了，保持健康生活方式',
      soundEnabled: true,
      vibrationEnabled: false
    });
  }

  // 获取提醒配置
  getReminderConfig(type: ReminderType): ReminderConfig | undefined {
    return this.settings.get(type);
  }

  // 更新提醒配置
  updateReminderConfig(config: ReminderConfig): void {
    this.settings.set(config.type, config);
  }

  // 获取所有提醒配置
  getAllReminderConfigs(): ReminderConfig[] {
    return Array.from(this.settings.values());
  }

  // 设置运动目标
  setGoal(goal: SportGoal): void {
    console.log('ReminderSettings.setGoal - 设置前 - 距离:', globalGoal.distance, '米');
    console.log('ReminderSettings.setGoal - 设置前 - 时长:', globalGoal.duration, '分钟');
    console.log('ReminderSettings.setGoal - 设置前 - 卡路里:', globalGoal.calories);
    
    console.log('ReminderSettings.setGoal - 设置值 - 距离:', goal.distance, '米');
    console.log('ReminderSettings.setGoal - 设置值 - 时长:', goal.duration, '分钟');
    console.log('ReminderSettings.setGoal - 设置值 - 卡路里:', goal.calories);
    
    globalGoal = goal;
    
    console.log('ReminderSettings.setGoal - 设置后 - 距离:', globalGoal.distance, '米');
    console.log('ReminderSettings.setGoal - 设置后 - 时长:', globalGoal.duration, '分钟');
    console.log('ReminderSettings.setGoal - 设置后 - 卡路里:', globalGoal.calories);
  }

  // 获取运动目标
  getGoal(): SportGoal {
    console.log('ReminderSettings.getGoal - 返回值 - 距离:', globalGoal.distance, '米');
    console.log('ReminderSettings.getGoal - 返回值 - 时长:', globalGoal.duration, '分钟');
    console.log('ReminderSettings.getGoal - 返回值 - 卡路里:', globalGoal.calories);
    return globalGoal;
  }

  // 检查是否达成目标并返回达成的目标类型
  checkGoalAchievement(distance: number, duration: number, calories: number): string[] {
    const achievedGoals: string[] = [];
    const hasDistanceGoal = globalGoal.distance !== undefined;
    const hasDurationGoal = globalGoal.duration !== undefined;
    const hasCaloriesGoal = globalGoal.calories !== undefined;

    console.log('ReminderSettings.checkGoalAchievement - 距离目标:', hasDistanceGoal, '当前值:', distance, '目标值:', globalGoal.distance);
    console.log('ReminderSettings.checkGoalAchievement - 时长目标:', hasDurationGoal, '当前值:', duration / 60000, '分钟', '目标值:', globalGoal.duration, '分钟');
    console.log('ReminderSettings.checkGoalAchievement - 卡路里目标:', hasCaloriesGoal, '当前值:', calories, '目标值:', globalGoal.calories);

    // 检查各个目标是否达成
    if (hasDistanceGoal && distance >= globalGoal.distance!) {
      achievedGoals.push('distance');
      console.log('ReminderSettings.checkGoalAchievement - 距离目标达成');
    }
    if (hasDurationGoal && duration >= globalGoal.duration! * 60000) { // 转换为毫秒
      achievedGoals.push('duration');
      console.log('ReminderSettings.checkGoalAchievement - 时长目标达成');
    }
    if (hasCaloriesGoal && calories >= globalGoal.calories!) {
      achievedGoals.push('calories');
      console.log('ReminderSettings.checkGoalAchievement - 卡路里目标达成');
    }

    console.log('ReminderSettings.checkGoalAchievement - 达成的目标:', achievedGoals);
    return achievedGoals;
  }

  // 检查是否有任何目标达成
  hasAnyGoalAchieved(distance: number, duration: number, calories: number): boolean {
    return this.checkGoalAchievement(distance, duration, calories).length > 0;
  }
}

// 提醒状态类
export class ReminderState {
  lastDurationReminder: number = 0; // 上次时长提醒时间
  lastRegularReminder: number = 0;  // 上次定期提醒时间
  distanceGoalAchieved: boolean = false;  // 距离目标是否已达成
  durationGoalAchieved: boolean = false;  // 时长目标是否已达成
  caloriesGoalAchieved: boolean = false;  // 卡路里目标是否已达成
  activeReminders: Set<string> = new Set(); // 活跃的提醒ID

  reset(): void {
    this.lastDurationReminder = 0;
    this.lastRegularReminder = 0;
    this.distanceGoalAchieved = false;
    this.durationGoalAchieved = false;
    this.caloriesGoalAchieved = false;
    this.activeReminders.clear();
  }
}
