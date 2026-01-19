/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// PAP/PersonalAssistantPro/entry/src/main/ets/services/NotificationService.ets

import reminderAgentManager from '@ohos.reminderAgentManager';
import notificationManager from '@ohos.notificationManager';
import common from '@ohos.app.ability.common';
import wantAgent from '@ohos.app.ability.wantAgent';
import { Logger } from '../common/utils/Logger';

const logger = new Logger('NotificationService');

export class NotificationService {

  static async checkAndRequestPermission(context: common.UIAbilityContext): Promise<boolean> {
    try {
      await notificationManager.requestEnableNotification(context);
      return true;
    } catch (err) {
      return false;
    }
  }

  // ✅ 恢复标准 Calendar Reminder 模式 (最稳定)
  static async publishCalendarReminder(context: common.UIAbilityContext, eventId: number, title: string, content: string, triggerTime: number): Promise<number> {
    // 1. 时间校验
    if (triggerTime < new Date().getTime()) {
      logger.warn('Time passed.');
      return -1;
    }

    const date = new Date(triggerTime);

    try {
      // 2. 准备跳转 (WantAgent)
      const wantInfo: wantAgent.WantAgentInfo = {
        wants: [
          {
            bundleName: context.abilityInfo.bundleName,
            abilityName: context.abilityInfo.name,
            parameters: {
              'targetPage': 'EventDetailPage',
              'eventId': eventId
            }
          }
        ],
        operationType: wantAgent.OperationType.START_ABILITY,
        requestCode: 0,
        wantAgentFlags: [wantAgent.WantAgentFlags.UPDATE_PRESENT_FLAG]
      };
      const agent = await wantAgent.getWantAgent(wantInfo);

      // 3. 提醒配置 (回归标准)
      const reminderRequest: reminderAgentManager.ReminderRequestCalendar = {
        reminderType: reminderAgentManager.ReminderType.REMINDER_TYPE_CALENDAR,
        dateTime: {
          year: date.getFullYear(),
          month: date.getMonth() + 1,
          day: date.getDate(),
          hour: date.getHours(),
          minute: date.getMinutes(),
          second: 0
        },
        content: content,
        title: title,

        // ❌ [移除] 不再强制指定 slotType，防止系统丢弃请求
        // slotType: notification.SlotType.SOCIAL_COMMUNICATION,

        // ✅ [恢复] 设为 60 秒 (或更长)
        // 让它像闹钟一样持续响铃，直到用户点击按钮或关闭
        ringDuration: 60,

        snoozeTimes: 1, // 允许稍后提醒一次
        timeInterval: 5 * 60, // 稍后提醒间隔5分钟

        notificationId: eventId,
        wantAgent: agent as ESObject,

        // ✅ [保留] 按钮依然是必要的交互入口
        actionButton: [
          { title: '关闭', type: reminderAgentManager.ActionButtonType.ACTION_BUTTON_TYPE_CLOSE },
          { title: '稍后', type: reminderAgentManager.ActionButtonType.ACTION_BUTTON_TYPE_SNOOZE }
        ]
      };

      // 4. 发布
      const reminderId = await reminderAgentManager.publishReminder(reminderRequest);
      logger.info(`Scheduled standard calendar reminder. ID: ${reminderId}`);
      return reminderId;

    } catch (err) {
      logger.error(`Schedule failed: ${err.message}`);
      return -1;
    }
  }

  static async cancelReminder(reminderId: number): Promise<void> {
    if (reminderId < 0) return;
    try {
      await reminderAgentManager.cancelReminder(reminderId);
    } catch (err) {
      // ignore
    }
  }
}