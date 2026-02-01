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
 * 提醒数据模型
 */
import { ReminderTiming, ReminderType } from '../common/constants/Constants'
import { relationalStore } from '@kit.ArkData'

/**
 * 提醒初始化参数
 */
export interface ReminderInit {
  id?: string
  scheduleId?: string
  timing?: ReminderTiming
  type?: ReminderType
  triggerTime?: number
  isTriggered?: boolean
  customSound?: string
  vibrationEnabled?: boolean
}

/**
 * 提醒数据模型
 */
export class Reminder {
  id: string = ''
  scheduleId: string = '' // 关联的日程ID
  timing: ReminderTiming = ReminderTiming.ON_TIME // 提醒时机
  type: ReminderType = ReminderType.NOTIFICATION // 提醒方式
  triggerTime: number = 0 // 计算后的触发时间戳
  isTriggered: boolean = false // 是否已触发
  customSound: string = '' // 自定义提示音
  vibrationEnabled: boolean = true // 是否启用震动
  createdAt: number = 0

  constructor(init?: ReminderInit) {
    if (init) {
      if (init.id !== undefined) {
        this.id = init.id
      }
      if (init.scheduleId !== undefined) {
        this.scheduleId = init.scheduleId
      }
      if (init.timing !== undefined) {
        this.timing = init.timing
      }
      if (init.type !== undefined) {
        this.type = init.type
      }
      if (init.triggerTime !== undefined) {
        this.triggerTime = init.triggerTime
      }
      if (init.isTriggered !== undefined) {
        this.isTriggered = init.isTriggered
      }
      if (init.customSound !== undefined) {
        this.customSound = init.customSound
      }
      if (init.vibrationEnabled !== undefined) {
        this.vibrationEnabled = init.vibrationEnabled
      }
    }
    if (!this.id) {
      this.id = generateReminderUUID()
    }
    if (!this.createdAt) {
      this.createdAt = Date.now()
    }
  }

  /**
   * 根据日程开始时间计算提醒触发时间
   */
  calculateTriggerTime(scheduleStartTime: number): void {
    this.triggerTime = scheduleStartTime - this.timing * 60 * 1000
  }

  /**
   * 获取提醒时机的显示文本
   */
  getTimingText(): string {
    switch (this.timing) {
      case ReminderTiming.ON_TIME:
        return '准时提醒'
      case ReminderTiming.MINUTES_5:
        return '提前5分钟'
      case ReminderTiming.MINUTES_10:
        return '提前10分钟'
      case ReminderTiming.MINUTES_15:
        return '提前15分钟'
      case ReminderTiming.MINUTES_30:
        return '提前30分钟'
      case ReminderTiming.HOURS_1:
        return '提前1小时'
      case ReminderTiming.HOURS_2:
        return '提前2小时'
      case ReminderTiming.DAYS_1:
        return '提前1天'
      case ReminderTiming.DAYS_2:
        return '提前2天'
      case ReminderTiming.WEEKS_1:
        return '提前1周'
      default:
        return '未知'
    }
  }

  /**
   * 转换为数据库存储对象
   */
  toDbObject(): relationalStore.ValuesBucket {
    const bucket: relationalStore.ValuesBucket = {
      id: this.id,
      scheduleId: this.scheduleId,
      timing: this.timing,
      type: this.type,
      triggerTime: this.triggerTime,
      isTriggered: this.isTriggered ? 1 : 0,
      customSound: this.customSound,
      vibrationEnabled: this.vibrationEnabled ? 1 : 0,
      createdAt: this.createdAt
    }
    return bucket
  }

  /**
   * 从数据库对象创建 Reminder 实例
   */
  static fromDbObject(obj: relationalStore.ValuesBucket): Reminder {
    const reminder = new Reminder()
    reminder.id = obj['id'] as string
    reminder.scheduleId = obj['scheduleId'] as string
    reminder.timing = obj['timing'] as ReminderTiming
    reminder.type = obj['type'] as ReminderType
    reminder.triggerTime = obj['triggerTime'] as number
    reminder.isTriggered = (obj['isTriggered'] as number) === 1
    reminder.customSound = obj['customSound'] as string
    reminder.vibrationEnabled = (obj['vibrationEnabled'] as number) === 1
    reminder.createdAt = obj['createdAt'] as number
    return reminder
  }
}

/**
 * 生成 UUID
 */
function generateReminderUUID(): string {
  const timestamp = Date.now().toString(36)
  const randomPart = Math.random().toString(36).substring(2, 15)
  return `reminder-${timestamp}-${randomPart}`
}
