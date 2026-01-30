/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/**
 * 通知服务
 * 负责日程提醒通知的发送和管理
 */
import { notificationManager } from '@kit.NotificationKit'
import { Logger } from '../common/utils/Logger'
import { Schedule } from '../model/Schedule'
import { Reminder } from '../model/Reminder'
import { dataService } from './DataService'
import { wantAgent, WantAgent } from '@kit.AbilityKit'

const TAG = 'NotificationService'

/**
 * 通知服务类
 */
export class NotificationService {
  private static instance: NotificationService | null = null
  private isEnabled: boolean = false
  private pollingIntervalId: number = -1

  private constructor() {}

  static getInstance(): NotificationService {
    if (!NotificationService.instance) {
      NotificationService.instance = new NotificationService()
    }
    return NotificationService.instance
  }

  /**
   * 初始化通知服务
   */
  async initialize(): Promise<void> {
    try {
      // 检查通知权限
      const isNotificationEnabled = await notificationManager.isNotificationEnabled()
      this.isEnabled = isNotificationEnabled
      
      if (!isNotificationEnabled) {
        // 请求通知权限
        await notificationManager.requestEnableNotification()
        this.isEnabled = await notificationManager.isNotificationEnabled()
      }
      
      Logger.info(TAG, `Notification service initialized, enabled: ${this.isEnabled}`)
    } catch (error) {
      Logger.error(TAG, `Failed to initialize notification service: ${JSON.stringify(error)}`)
      this.isEnabled = false
    }
  }

  /**
   * 启动轮询检查提醒（每整分钟执行一次）
   */
  startPolling(): void {
    if (this.pollingIntervalId !== -1) {
      return // 已经在轮询中
    }
    
    // 计算到下一个整分钟的毫秒数
    const now = new Date()
    const msToNextMinute = (60 - now.getSeconds()) * 1000 - now.getMilliseconds()
    
    // 先等待到整分钟，然后开始每分钟轮询
    setTimeout(() => {
      // 立即执行一次
      this.checkAndSendPendingReminders()
      
      // 然后每60秒执行一次
      this.pollingIntervalId = setInterval(() => {
        this.checkAndSendPendingReminders()
      }, 60 * 1000)
    }, msToNextMinute)
    
    Logger.info(TAG, `Reminder polling will start in ${msToNextMinute}ms (at next minute)`)
  }

  /**
   * 停止轮询
   */
  stopPolling(): void {
    if (this.pollingIntervalId !== -1) {
      clearInterval(this.pollingIntervalId)
      this.pollingIntervalId = -1
      Logger.info(TAG, 'Reminder polling stopped')
    }
  }

  /**
   * 发送日程提醒通知
   */
  async sendScheduleReminder(schedule: Schedule, reminder: Reminder): Promise<void> {
    if (!this.isEnabled) {
      Logger.warn(TAG, 'Notification is not enabled')
      return
    }

    try {
      // 创建 WantAgent 用于点击通知跳转
      const wantAgentInfo: wantAgent.WantAgentInfo = {
        wants: [
          {
            bundleName: 'com.example.memo_time',
            abilityName: 'EntryAbility',
            parameters: {
              scheduleId: schedule.id
            }
          }
        ],
        actionType: wantAgent.OperationType.START_ABILITY,
        requestCode: 0,
        actionFlags: [wantAgent.WantAgentFlags.UPDATE_PRESENT_FLAG]
      }

      const agent: WantAgent = await wantAgent.getWantAgent(wantAgentInfo)

      // 构建通知请求
      const notificationRequest: notificationManager.NotificationRequest = {
        id: this.generateNotificationId(reminder.id),
        content: {
          notificationContentType: notificationManager.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
          normal: {
            title: '日程提醒',
            text: schedule.title,
            additionalText: reminder.getTimingText()
          }
        },
        wantAgent: agent,
        notificationSlotType: notificationManager.SlotType.SOCIAL_COMMUNICATION
      }

      await notificationManager.publish(notificationRequest)
      
      // 标记提醒为已触发
      await dataService.updateReminderTriggered(reminder.id)
      
      Logger.info(TAG, `Notification sent for schedule: ${schedule.id}`)
    } catch (error) {
      Logger.error(TAG, `Failed to send notification: ${JSON.stringify(error)}`)
    }
  }

  /**
   * 发送即时测试通知
   */
  async sendTestNotification(): Promise<boolean> {
    if (!this.isEnabled) {
      Logger.warn(TAG, 'Notification is not enabled')
      return false
    }

    try {
      const notificationRequest: notificationManager.NotificationRequest = {
        id: Date.now() % 100000,
        content: {
          notificationContentType: notificationManager.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
          normal: {
            title: 'MemoTime',
            text: '通知测试成功！',
            additionalText: '您的通知功能已启用'
          }
        },
        notificationSlotType: notificationManager.SlotType.SOCIAL_COMMUNICATION
      }

      await notificationManager.publish(notificationRequest)
      Logger.info(TAG, 'Test notification sent successfully')
      return true
    } catch (error) {
      Logger.error(TAG, `Failed to send test notification: ${JSON.stringify(error)}`)
      return false
    }
  }

  /**
   * 取消所有通知
   */
  async cancelAllNotifications(): Promise<void> {
    try {
      await notificationManager.cancelAll()
      Logger.info(TAG, 'All notifications cancelled')
    } catch (error) {
      Logger.error(TAG, `Failed to cancel notifications: ${JSON.stringify(error)}`)
    }
  }

  /**
   * 取消指定日程的通知
   */
  async cancelScheduleNotifications(scheduleId: string): Promise<void> {
    try {
      // 获取该日程的所有提醒
      const reminders = await dataService.getRemindersByScheduleId(scheduleId)
      for (const reminder of reminders) {
        await notificationManager.cancel(this.generateNotificationId(reminder.id))
      }
      Logger.info(TAG, `Notifications cancelled for schedule: ${scheduleId}`)
    } catch (error) {
      Logger.error(TAG, `Failed to cancel schedule notifications: ${JSON.stringify(error)}`)
    }
  }

  /**
   * 检查并发送待触发的提醒
   */
  async checkAndSendPendingReminders(): Promise<void> {
    try {
      const now = Date.now()
      
      // 获取所有未触发的提醒
      const pendingReminders = await dataService.getPendingReminders()
      
      for (const reminder of pendingReminders) {
        // 检查是否应该触发（触发时间已到或在1分钟内）
        if (reminder.triggerTime <= now + 60 * 1000 && reminder.triggerTime >= now - 5 * 60 * 1000) {
          // 获取对应的日程
          const schedule = await dataService.getScheduleById(reminder.scheduleId)
          if (schedule) {
            await this.sendScheduleReminder(schedule, reminder)
            Logger.info(TAG, `  -> Triggered reminder for schedule: ${schedule.title}`)
          }
        }
      }
    } catch (error) {
      Logger.error(TAG, `Failed to check pending reminders: ${JSON.stringify(error)}`)
    }
  }

  /**
   * 生成通知ID
   */
  private generateNotificationId(reminderId: string): number {
    // 将字符串ID转换为数字ID
    let hash = 0
    for (let i = 0; i < reminderId.length; i++) {
      const char = reminderId.charCodeAt(i)
      hash = ((hash << 5) - hash) + char
      hash = hash & hash // Convert to 32bit integer
    }
    return Math.abs(hash) % 100000
  }

  /**
   * 获取通知是否启用
   */
  isNotificationEnabled(): boolean {
    return this.isEnabled
  }
}

export const notificationService = NotificationService.getInstance()
