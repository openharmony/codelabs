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

// services/ReminderService.ts
import reminderManager from '@ohos.reminderAgentManager';
import Notification from '@ohos.notification';
import { notificationManager } from '@kit.NotificationKit';
import wantAgent from '@ohos.wantAgent';
import { ReminderOption, RepeatRule } from '../model/EventTypes';
import { CalendarEvent } from '../model/EventModel';

// ============ 类型定义 ============

// 2. LocalDateTime
interface LocalDateTime {
  year: number;
  month: number;
  day: number;
  hour: number;
  minute: number;
  second?: number;
}

// 3. ActionButton
interface ActionButton {
  title: string;
  type: number;
}

// 4. WantAgent简化类型
interface SimpleWantAgent {
  trigger(): Promise<void>;
}

// 6. 简化的提醒请求类型（用于发布）
interface SimplifiedReminderRequest {
  reminderType: reminderManager.ReminderType;
  dateTime: LocalDateTime;
  title?: string;
  content?: string;
  notificationId?: number;
  ringDuration?: number;
  snoozeTimes?: number;
  timeInterval?: number;
  expiredContent?: string;
  snoozeContent?: string;
  slotType?: Notification.SlotType;
  tapDismissed?: boolean;
  autoDeletedTime?: number;
  snoozeSlotType?: Notification.SlotType;
  // repeatMonths?: number[];
  // repeatDays?: number[];
  // daysOfWeek?: number[];
  // endDateTime?: LocalDateTime;
}

// 7. ValidReminder
interface ValidReminder {
  reminderId: number;
  notificationId?: number;
  reminderType?: reminderManager.ReminderType;
  title?: string;
  content?: string;
  dateTime?: LocalDateTime;
}

// 8. Notification类型
interface NotificationNormalContent {
  title: string;
  text: string;
  additionalText?: string;
}

interface NotificationContent {
  contentType: Notification.ContentType;
  normal: NotificationNormalContent;
}

// 9. 备用通知请求类型
interface FallbackNotificationRequest {
  content: NotificationContent;
  id: number;
  slotType: Notification.SlotType;
  deliveryTime: number;
  tapDismissed: boolean;
  autoDeletedTime?: number;
}

// ============ ReminderService 类 ============

export class ReminderService {
  private static instance: ReminderService | null = null;

  public static getInstance(): ReminderService {
    if (!ReminderService.instance) {
      ReminderService.instance = new ReminderService();
    }
    return ReminderService.instance;
  }

  // ============ 公开方法 ============

  // 设置提醒
  async setReminder(event: CalendarEvent): Promise<boolean> {
    try {
      await this.cancelReminder(event.id);

      if (event.reminder < 0) {
        return true;
      }

      const remindTime = this.calculateRemindTime(event.startTime, event.reminder);
      if (!remindTime) {
        return false;
      }

      // 使用 reminderAgentManager
      const reminderSuccess = await this.trySetSimpleReminder(event, remindTime);
      if (reminderSuccess) {
        return true;
      }

      // 备用方案
      return await this.setFallbackNotification(event, remindTime);

    } catch (error) {
      console.error('设置提醒失败:', this.formatError(error));
      return false;
    }
  }

  // 取消提醒
  async cancelReminder(eventId: string): Promise<boolean> {
    try {
      const reminders = await reminderManager.getValidReminders();
      const targetId = this.generateNotificationId(eventId);

      for (const reminder of reminders) {
        const validReminder = reminder as ValidReminder;
        if (validReminder.notificationId === targetId && validReminder.reminderId) {
          await reminderManager.cancelReminder(validReminder.reminderId);
          console.log(`提醒已取消: eventId=${eventId}`);
          return true;
        }
      }

      // 取消通知
      try {
        const notificationId = this.generateNotificationId(eventId);
        // 7. 通过 notificationManager 对象调用 cancel
        await notificationManager.cancel(notificationId);
        console.info(`提醒已取消: eventId=${eventId}`);
        return true;
      } catch (error) {
        console.error('取消提醒失败:', this.formatError(error));
        return false;
      }

    } catch (error) {
      console.error('取消提醒失败:', this.formatError(error));
      return false;
    }
  }

  // ============ 私有方法 ============

  // 尝试使用日历提醒
  private async trySetSimpleReminder(
    event: CalendarEvent,
    remindTime: number,
  ): Promise<boolean> {
    try {
      const remindDate = new Date(remindTime);

      // 创建提醒请求
      const reminderRequest: SimplifiedReminderRequest = {
        reminderType: this.getReminderType(),
        dateTime: {
          year: remindDate.getFullYear(),
          month: remindDate.getMonth() + 1,
          day: remindDate.getDate(),
          hour: remindDate.getHours(),
          minute: remindDate.getMinutes(),
          second: remindDate.getSeconds()
        }as LocalDateTime,
        title: event.title,
        content: this.getReminderContent(event),
        notificationId: this.generateNotificationId(event.id),
        ringDuration: 30,
        snoozeTimes: 1,
        timeInterval: 300,
        expiredContent: '日程已过期',
        snoozeContent: '日程提醒',
        slotType: Notification.SlotType.CONTENT_INFORMATION,
        tapDismissed: true,
        autoDeletedTime: event.endTime,
        snoozeSlotType: Notification.SlotType.CONTENT_INFORMATION
      };

      // // 添加重复配置
      // this.addRepeatConfig(reminderRequest, repeatType, event.startTime, event.endTime);

      // 尝试发布提醒
      const reminderId = await reminderManager.publishReminder(reminderRequest);
      console.log(`提醒设置成功: eventId=${event.id}, reminderId=${reminderId}`);
      return true;

    } catch (error) {
      console.error('使用简单提醒失败:', this.formatError(error));
      return false;
    }
  }

  // 创建WantAgent（简化版本，先不处理）
  private async createWantAgent(): Promise<SimpleWantAgent | undefined> {
    try {
      // 暂时不创建WantAgent
      return undefined;
    } catch (error) {
      console.error('创建WantAgent失败:', this.formatError(error));
      return undefined;
    }
  }

  // 备用通知方案
  private async setFallbackNotification(event: CalendarEvent, remindTime: number): Promise<boolean> {
    try {
      // 创建通知内容
      const notificationContent: notificationManager.NotificationContent = {
        contentType: Notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
        normal: {
          title: event.title,
          text: this.getReminderContent(event),
          additionalText: event.location || ''
        }
      };

      // 创建通知请求
      const request: notificationManager.NotificationRequest = {
        content: notificationContent,
        id: this.generateNotificationId(event.id),
        slotType: Notification.SlotType.CONTENT_INFORMATION,
        deliveryTime: remindTime,
        tapDismissed: true,
        autoDeletedTime: event.endTime
      };

      // 发布通知
      await notificationManager.publish(request);
      console.log('备用通知设置成功');
      return true;

    } catch (error) {
      console.error('设置备用通知失败:', this.formatError(error));
      return false;
    }
  }

  // 格式化错误信息
  private formatError(error: Error | string | object): string {
    if (error instanceof Error) {
      return error.message;
    }
    if (typeof error === 'string') {
      return error;
    }
    try {
      return JSON.stringify(error);
    } catch {
      return '未知错误';
    }
  }

  // 计算提醒时间
  private calculateRemindTime(eventStartTime: number, reminderMinutes: number): number | null {
    if (reminderMinutes < 0) return null;
    if (reminderMinutes === 0) return eventStartTime;

    const remindTime = eventStartTime - (reminderMinutes * 60 * 1000);
    return remindTime < Date.now() ? null : remindTime;
  }

  // 生成通知ID
  private generateNotificationId(eventId: string): number {
    let hash = 0;
    for (let i = 0; i < eventId.length; i++) {
      hash = ((hash << 5) - hash) + eventId.charCodeAt(i);
      hash |= 0;
    }
    return Math.abs(hash) % 1000000;
  }

  // 获取提醒内容
  private getReminderContent(event: CalendarEvent): string {
    const startTime = new Date(event.startTime);

    if (event.allDay) {
      const dateStr = startTime.toLocaleDateString('zh-CN');
      return `全天日程 · ${dateStr}${event.location ? ` · ${event.location}` : ''}`;
    }

    const timeStr = startTime.toLocaleTimeString('zh-CN', {
      hour: '2-digit',
      minute: '2-digit'
    });
    const dateStr = startTime.toLocaleDateString('zh-CN');

    return `${dateStr} ${timeStr}${event.location ? ` · ${event.location}` : ''}`;
  }

  // 获取包名
  private getPackageName(): string {
    try {
      return getContext().applicationInfo?.name || 'com.example.calendar';
    } catch {
      return 'com.example.calendar';
    }
  }

  // 获取提醒类型
  private getReminderType(): reminderManager.ReminderType {
    try {
      if (reminderManager.ReminderType.REMINDER_TYPE_TIMER !== undefined) {
        return reminderManager.ReminderType.REMINDER_TYPE_TIMER;
      }
      if (reminderManager.ReminderType.REMINDER_TYPE_ALARM !== undefined) {
        return reminderManager.ReminderType.REMINDER_TYPE_ALARM;
      }
      return 0 as reminderManager.ReminderType;
    } catch {
      return 0 as reminderManager.ReminderType;
    }
  }

  // 获取提醒选项
  getReminderOptions(): ReminderOption[] {
    return [
      { label: '不提醒', minutes: -1 },
      { label: '开始时', minutes: 0 },
      { label: '提前5分钟', minutes: 5 },
      { label: '提前10分钟', minutes: 10 },
      { label: '提前30分钟', minutes: 30 },
      { label: '提前1小时', minutes: 60 },
      { label: '提前1天', minutes: 1440 }
    ];
  }

  // 根据分钟数获取索引
  getReminderIndexByMinutes(minutes: number): number {
    const options = this.getReminderOptions();
    const index = options.findIndex(option => option.minutes === minutes);
    return index >= 0 ? index : 0;
  }

  // 根据索引获取分钟数
  getReminderMinutesByIndex(index: number): number {
    const options = this.getReminderOptions();
    if (index >= 0 && index < options.length) {
      return options[index].minutes;
    }
    return -1;
  }
}