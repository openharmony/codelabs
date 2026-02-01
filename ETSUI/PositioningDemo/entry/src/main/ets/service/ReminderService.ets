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

import { ReminderType, ReminderConfig, ReminderData, ReminderSettings, ReminderState, SportGoal } from '../model/ReminderModel';
import { SportData } from '../model/SportModel';
import promptAction from '@ohos.promptAction';
import { common } from '@kit.AbilityKit';

// 提醒服务类
export class ReminderService {
  private settings: ReminderSettings = new ReminderSettings();
  private state: ReminderState = new ReminderState();
  private reminderHistory: ReminderData[] = [];
  private context: common.UIAbilityContext | null = null;
  private reminderTimer: number = -1;

  // 初始化服务
  init(context: common.UIAbilityContext): void {
    this.context = context;
    console.log('提醒服务已初始化');
  }

  // 获取提醒设置
  getSettings(): ReminderSettings {
    return this.settings;
  }

  // 更新提醒设置
  updateSettings(newSettings: ReminderSettings): void {
    this.settings = newSettings;
    console.log('提醒设置已更新');
  }

  // 更新单个提醒配置
  updateReminderConfig(config: ReminderConfig): void {
    this.settings.updateReminderConfig(config);
    console.log('提醒配置已更新:', config.type);
  }

  // 设置运动目标
  setSportGoal(goal: SportGoal): void {
    console.log('开始设置运动目标 - 距离:', goal.distance, '米');
    console.log('开始设置运动目标 - 时长:', goal.duration, '分钟');
    console.log('开始设置运动目标 - 卡路里:', goal.calories);
    
    const beforeGoal = this.settings.getGoal();
    console.log('设置前的目标 - 距离:', beforeGoal.distance, '米');
    console.log('设置前的目标 - 时长:', beforeGoal.duration, '分钟');
    console.log('设置前的目标 - 卡路里:', beforeGoal.calories);
    
    this.settings.setGoal(goal);
    
    const afterGoal = this.settings.getGoal();
    console.log('设置后的目标 - 距离:', afterGoal.distance, '米');
    console.log('设置后的目标 - 时长:', afterGoal.duration, '分钟');
    console.log('设置后的目标 - 卡路里:', afterGoal.calories);
    
    console.log('运动目标已设置 - 距离:', goal.distance, '米');
    console.log('运动目标已设置 - 时长:', goal.duration, '分钟');
    console.log('运动目标已设置 - 卡路里:', goal.calories);
  }

  // 启动提醒服务
  startReminderService(): void {
    this.state.reset();
    this.startReminderTimer();
    console.log('提醒服务已启动');
  }

  // 停止提醒服务
  stopReminderService(): void {
    this.stopReminderTimer();
    this.state.reset();
    console.log('提醒服务已停止');
  }

  // 启动提醒定时器
  private startReminderTimer(): void {
    // 清除之前的定时器
    this.stopReminderTimer();

    // 创建新的定时器，每10秒检查一次提醒
    this.reminderTimer = setInterval(() => {
      this.checkRegularReminders();
    }, 10000);

    console.log('提醒定时器已启动');
  }

  // 停止提醒定时器
  private stopReminderTimer(): void {
    if (this.reminderTimer !== -1) {
      clearInterval(this.reminderTimer);
      this.reminderTimer = -1;
      console.log('提醒定时器已停止');
    }
  }

  // 检查定期运动提醒
  private checkRegularReminders(): void {
    const config = this.settings.getReminderConfig(ReminderType.REGULAR_SPORT);
    if (config && config.enabled && config.interval) {
      const now = Date.now();
      if (now - this.state.lastRegularReminder >= config.interval * 1000) {
        this.triggerReminder(ReminderType.REGULAR_SPORT, config.message);
        this.state.lastRegularReminder = now;
      }
    }
  }

  // 触发运动开始提醒
  triggerSportStartReminder(): void {
    const config = this.settings.getReminderConfig(ReminderType.SPORT_START);
    if (config && config.enabled) {
      this.triggerReminder(ReminderType.SPORT_START, config.message);
    }
  }

  // 检查并触发运动时长提醒
  checkDurationReminder(sportData: SportData): void {
    const config = this.settings.getReminderConfig(ReminderType.SPORT_DURATION);
    if (config && config.enabled && config.interval) {
      const now = Date.now();
      if (now - this.state.lastDurationReminder >= config.interval * 1000) {
        const durationMinutes = Math.floor(sportData.duration / 60000);
        const message = `${config.message}\n已运动 ${durationMinutes} 分钟`;
        this.triggerReminder(ReminderType.SPORT_DURATION, message);
        this.state.lastDurationReminder = now;
      }
    }
  }

  // 检查并触发目标达成提醒
  checkGoalAchievementReminder(sportData: SportData): boolean {
    const config = this.settings.getReminderConfig(ReminderType.GOAL_ACHIEVEMENT);
    console.log('检查目标达成提醒 - 配置:', config ? '存在' : '不存在', '启用状态:', config ? config.enabled : 'false');
    console.log('检查目标达成提醒 - 当前状态 - 距离:', this.state.distanceGoalAchieved);
    console.log('检查目标达成提醒 - 当前状态 - 时长:', this.state.durationGoalAchieved);
    console.log('检查目标达成提醒 - 当前状态 - 卡路里:', this.state.caloriesGoalAchieved);
    
    if (config && config.enabled) {
      // 改进日志输出，确保能够显示具体的数值
      console.log('当前运动数据 - 距离:', sportData.distance, '米');
      console.log('当前运动数据 - 时长:', sportData.duration, '毫秒');
      console.log('当前运动数据 - 卡路里:', sportData.calories);
      
      const goal = this.settings.getGoal();
      console.log('当前目标设置 - 距离:', goal.distance, '米');
      console.log('当前目标设置 - 时长:', goal.duration, '分钟');
      console.log('当前目标设置 - 卡路里:', goal.calories);
      
      // 简化目标检查逻辑，确保能够触发提醒
      const hasDistanceGoal = goal.distance !== undefined;
      const hasDurationGoal = goal.duration !== undefined;
      const hasCaloriesGoal = goal.calories !== undefined;
      
      console.log('是否有目标设置 - 距离:', hasDistanceGoal);
      console.log('是否有目标设置 - 时长:', hasDurationGoal);
      console.log('是否有目标设置 - 卡路里:', hasCaloriesGoal);
      
      let anyGoalAchieved = false;
      
      // 检查各个目标是否达成
      if (hasDistanceGoal && !this.state.distanceGoalAchieved) {
        console.log('距离目标检查:', sportData.distance, '>=', goal.distance!, '=', sportData.distance >= goal.distance!);
        if (sportData.distance >= goal.distance!) {
          const message = `恭喜！您已达成距离目标 ${goal.distance} 米`;
          console.log('触发距离目标达成提醒:', message);
          this.triggerReminder(ReminderType.GOAL_ACHIEVEMENT, message);
          this.state.distanceGoalAchieved = true;
          console.log('更新距离目标达成状态为:', this.state.distanceGoalAchieved);
          anyGoalAchieved = true;
        }
      }
      
      if (hasDurationGoal && !this.state.durationGoalAchieved) {
        const durationMinutes = sportData.duration / 60000;
        console.log('时长目标检查:', durationMinutes, '分钟 >=', goal.duration!, '分钟 =', durationMinutes >= goal.duration!);
        if (durationMinutes >= goal.duration!) {
          const message = `恭喜！您已达成时长目标 ${goal.duration} 分钟`;
          console.log('触发时长目标达成提醒:', message);
          this.triggerReminder(ReminderType.GOAL_ACHIEVEMENT, message);
          this.state.durationGoalAchieved = true;
          console.log('更新时长目标达成状态为:', this.state.durationGoalAchieved);
          anyGoalAchieved = true;
        }
      }
      
      if (hasCaloriesGoal && !this.state.caloriesGoalAchieved) {
        console.log('卡路里目标检查:', sportData.calories, '>=', goal.calories!, '=', sportData.calories >= goal.calories!);
        if (sportData.calories >= goal.calories!) {
          const message = `恭喜！您已达成卡路里目标 ${goal.calories} 卡路里`;
          console.log('触发卡路里目标达成提醒:', message);
          this.triggerReminder(ReminderType.GOAL_ACHIEVEMENT, message);
          this.state.caloriesGoalAchieved = true;
          console.log('更新卡路里目标达成状态为:', this.state.caloriesGoalAchieved);
          anyGoalAchieved = true;
        }
      }
      
      if (anyGoalAchieved) {
        console.log('至少有一个目标已达成');
        return true;
      } else {
        console.log('未达成任何目标');
      }
    } else {
      console.log('目标达成提醒未启用或配置不存在');
    }

    return false;
  }

  // 检查并触发休息提醒
  checkRestReminder(sportData: SportData): void {
    const config = this.settings.getReminderConfig(ReminderType.REST_REMINDER);
    if (config && config.enabled && config.durationThreshold) {
      const durationMinutes = Math.floor(sportData.duration / 60000);
      if (durationMinutes >= config.durationThreshold) {
        this.triggerReminder(ReminderType.REST_REMINDER, config.message);
      }
    }
  }

  // 触发提醒
  private triggerReminder(type: ReminderType, message: string): void {
    const reminderId = this.generateReminderId();
    const reminderData = new ReminderData(reminderId, type, message, Date.now());

    // 添加到提醒历史
    this.reminderHistory.push(reminderData);
    this.state.activeReminders.add(reminderId);

    // 显示提醒
    this.showReminder(message, type);

    console.log('触发提醒:', type, message);
  }

  // 显示提醒
  private showReminder(message: string, type: ReminderType): void {
    // 使用系统提示框显示提醒
    promptAction.showToast({
      message: message,
      duration: 3000
    });

    // 对于重要提醒，使用对话框
    if (type === ReminderType.GOAL_ACHIEVEMENT || type === ReminderType.REST_REMINDER) {
      promptAction.showDialog({
        title: '运动提醒',
        message: message,
        buttons: [
          {
            text: '确定',
            color: '#007AFF'
          }
        ]
      });
    }
  }

  // 生成提醒ID
  private generateReminderId(): string {
    return `reminder_${Date.now()}_${Math.floor(Math.random() * 1000)}`;
  }

  // 标记提醒为已读
  markReminderAsRead(reminderId: string): void {
    const reminder = this.reminderHistory.find(r => r.id === reminderId);
    if (reminder) {
      reminder.isRead = true;
      this.state.activeReminders.delete(reminderId);
      console.log('提醒已标记为已读:', reminderId);
    }
  }

  // 获取未读提醒数量
  getUnreadReminderCount(): number {
    return this.reminderHistory.filter(r => !r.isRead).length;
  }

  // 获取提醒历史
  getReminderHistory(): ReminderData[] {
    return this.reminderHistory;
  }

  // 清除提醒历史
  clearReminderHistory(): void {
    this.reminderHistory = [];
    this.state.activeReminders.clear();
    console.log('提醒历史已清除');
  }

  // 处理运动开始
  onSportStart(): void {
    this.triggerSportStartReminder();
    console.log('处理运动开始提醒');
  }

  // 处理运动数据更新
  onSportDataUpdate(sportData: SportData): void {
    this.checkDurationReminder(sportData);
    this.checkGoalAchievementReminder(sportData);
    this.checkRestReminder(sportData);
    console.log('处理运动数据更新提醒');
  }

  // 处理运动结束
  onSportEnd(): void {
    this.stopReminderService();
    console.log('处理运动结束提醒');
  }

  // 重置提醒状态
  reset(): void {
    this.state.reset();
    console.log('提醒状态已重置');
  }
}
