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

import reminderAgentManager from '@ohos.reminderAgentManager';
import notificationManager from '@ohos.notificationManager';
// 必须引入这个旧包，才能拿到 SlotType 枚举
import notification from '@ohos.notification';
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

  // ✅ 定时 + 强制标准 UI + 跳转
  static async publishCalendarReminder(context: common.UIAbilityContext, eventId: number, title: string, content: string, triggerTime: number): Promise<number> {
    // 1. 时间校验
    if (triggerTime < new Date().getTime()) {
      logger.warn('Time passed.');
      return -1;
    }

    const date = new Date(triggerTime);

    try {
      // 2. 准备跳转 (WantAgent) - 保持不变
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

      // 3. 提醒配置 (关键修改)
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

        // ✅ [关键修改 1] 显式指定 SlotType
        // 告诉系统：这是一条“社交通讯”级别的消息。
        // 系统会因此使用“标准横幅”模板（通常是不透明、高度正常的）来渲染它。
        slotType: notification.SlotType.SOCIAL_COMMUNICATION,

        // ✅ [关键修改 2] 控制时长
        // 设为 1 秒，仅仅为了触发一下系统通知，避免长时间占用变成全屏闹钟
        ringDuration: 1,

        snoozeTimes: 0,
        timeInterval: 0,

        notificationId: eventId,
        wantAgent: agent as ESObject,

        // ✅ [关键修改 3] 保留按钮
        // 在 SOCIAL_COMMUNICATION 模式下，按钮会显示在横幅下方，而不是全屏弹窗里
        actionButton: [
          { title: '查看', type: reminderAgentManager.ActionButtonType.ACTION_BUTTON_TYPE_CLOSE }
        ]
      };

      // 4. 发布
      const reminderId = await reminderAgentManager.publishReminder(reminderRequest);
      logger.info(`Scheduled standard UI reminder. ID: ${reminderId}`);
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