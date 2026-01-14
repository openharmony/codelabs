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
import common from '@ohos.app.ability.common';
import wantAgent from '@ohos.app.ability.wantAgent';
import { Logger } from '../common/utils/Logger';

const logger = new Logger('NotificationService');

export class NotificationService {

  static async checkAndRequestPermission(context: common.UIAbilityContext): Promise<boolean> {
    try {
      const isEnabled = await notificationManager.isNotificationEnabled();
      if (isEnabled) {
        return true;
      }
      logger.info('Notification is disabled, requesting permission...');
      await notificationManager.requestEnableNotification(context);
      return await notificationManager.isNotificationEnabled();
    } catch (err) {
      logger.error(`Request notification permission failed: ${JSON.stringify(err)}`);
      return false;
    }
  }

  static async publishCalendarReminder(context: common.UIAbilityContext, title: string, content: string, targetTime: number): Promise<number> {
    logger.info(`[Step 1] Preparing reminder. Bundle: ${context.abilityInfo.bundleName}, Ability: ${context.abilityInfo.name}`);
    const date = new Date(targetTime);

    try {
      // 1. 准备 WantAgentInfo
      // 采用“模拟桌面图标点击”的策略，这是最稳妥的唤起方式
      const wantInfo: wantAgent.WantAgentInfo = {
        wants: [
          {
            bundleName: context.abilityInfo.bundleName,
            abilityName: context.abilityInfo.name,
            action: 'ohos.want.action.home', // 👈 必须加：匹配 module.json5 的 action
            entities: ['entity.system.home'], // 👈 必须加：匹配 module.json5 的 entities
            parameters: {
              'fromNotification': true // 👈 必须加：防止 Intent 被系统去重拦截
            }
          }
        ],
        // 使用 START_ABILITY 启动应用
        operationType: wantAgent.OperationType.START_ABILITY,
        requestCode: 0,
        // REPLACE_ELEMENT 确保使用最新的参数
        wantAgentFlags: [wantAgent.WantAgentFlags.REPLACE_ELEMENT]
      };

      // 2. 获取 WantAgent
      const agent = await wantAgent.getWantAgent(wantInfo);
      logger.info('[Step 2] WantAgent created with HOME action.');

      // 3. 构建提醒请求对象
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
        snoozeTimes: 0,
        timeInterval: 0,
        ringDuration: 5,
        notificationId: Math.floor(targetTime / 1000),

        // 使用 as ESObject 解决类型问题
        wantAgent: agent as ESObject
      };

      // 4. 发布提醒
      const reminderId = await reminderAgentManager.publishReminder(reminderRequest);
      logger.info(`[Step 3] Reminder published successfully. ID: ${reminderId}`);
      return reminderId;

    } catch (err) {
      logger.error(`[ERROR] Publish failed. Code:${err.code}, Message:${err.message}`);
      return -1;
    }
  }

  static async cancelReminder(reminderId: number): Promise<void> {
    if (reminderId < 0) return;
    try {
      await reminderAgentManager.cancelReminder(reminderId);
      logger.info(`Reminder ${reminderId} canceled.`);
    } catch (err) {
      logger.error(`Cancel reminder failed code:${err.code} message:${err.message}`);
    }
  }
}