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
  /**
   * 检查并请求通知权限
   * 修复逻辑：必须等待用户操作结束后，再次查询真实的开关状态，而不是直接返回 true
   */
  static async checkAndRequestPermission(context: common.UIAbilityContext): Promise<boolean> {
    try {
      // 1. 初次检查
      const isEnabled = await notificationManager.isNotificationEnabled();
      if (isEnabled) {
        return true;
      }

      // 2. 请求权限 (弹窗)
      logger.info('Notification disabled, requesting permission...');
      await notificationManager.requestEnableNotification(context);

      // 3. 再次检查最终结果 (关键步骤)
      return await notificationManager.isNotificationEnabled();
    } catch (err) {
      logger.error(`Request notification permission failed: ${JSON.stringify(err)}`);
      return false;
    }
  }

  /**
   * 发布日历提醒
   * @param context 上下文
   * @param eventId 事件ID (用于通知去重和参数传递)
   * @param title 标题
   * @param content 内容
   * @param triggerTime 触发时间戳
   */
  static async publishCalendarReminder(context: common.UIAbilityContext, eventId: number, title: string,
    content: string, triggerTime: number): Promise<number> {
    const now = new Date().getTime();

    // 1. 时间校验 (修复：给予 1 分钟的宽限期，防止毫秒级延迟导致校验失败)
    if (triggerTime < now - 60000) {
      logger.warn(`Time passed. Trigger: ${triggerTime}, Now: ${now}`);
      return -1;
    }

    const date = new Date(triggerTime);

    try {
      // 2. 准备 WantAgentInfo (修复：补全 action 和 entities)
      const wantInfo: wantAgent.WantAgentInfo = {
        wants: [
          {
            bundleName: context.abilityInfo.bundleName,
            abilityName: context.abilityInfo.name,
            // 必须加：匹配 module.json5 的 action，系统据此识别入口
            action: 'ohos.want.action.home',
            // 必须加：匹配 module.json5 的 entities
            entities: ['entity.system.home'],
            parameters: {
              // 业务参数：点击通知后跳转到详情页
              'targetPage': 'EventDetailPage',
              'eventId': eventId,
              // 系统参数：防止 Intent 被系统去重拦截
              'fromNotification': true
            }
          }
        ],
        // 使用 START_ABILITY 启动应用
        operationType: wantAgent.OperationType.START_ABILITY,
        requestCode: 0,
        // 修复：使用 REPLACE_ELEMENT 确保 eventId 参数被更新，而不是沿用旧的
        wantAgentFlags: [wantAgent.WantAgentFlags.REPLACE_ELEMENT]
      };

      // 3. 获取 WantAgent
      const agent = await wantAgent.getWantAgent(wantInfo);

      // 📝 [修复] 预先解析国际化字符串，因为 actionButton.title 只接受 string
      const closeTitle = context.resourceManager.getStringSync($r('app.string.notification_action_close').id);
      const snoozeTitle = context.resourceManager.getStringSync($r('app.string.notification_action_snooze').id);

      // 4. 构建提醒请求对象
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

        // 响铃时长：60秒
        ringDuration: 60,

        // 稍后提醒设置：允许1次，间隔5分钟
        snoozeTimes: 1,
        timeInterval: 5 * 60,

        // 使用 eventId 作为 notificationId，方便后续更新或取消
        notificationId: eventId,

        // 绑定点击跳转动作
        wantAgent: agent as ESObject,

        // 交互按钮
        actionButton: [
          { title: closeTitle, type: reminderAgentManager.ActionButtonType.ACTION_BUTTON_TYPE_CLOSE },
          { title: snoozeTitle, type: reminderAgentManager.ActionButtonType.ACTION_BUTTON_TYPE_SNOOZE }
        ]
      };

      // 5. 发布提醒
      const reminderId = await reminderAgentManager.publishReminder(reminderRequest);
      logger.info(`[Success] Calendar reminder published. ID: ${reminderId}`);
      return reminderId;

    } catch (err) {
      logger.error(`[ERROR] Publish failed. Code:${err.code}, Message:${err.message}`);
      return -1;
    }
  }

  /**
   * 取消提醒
   * @param reminderId 提醒ID
   */
  static async cancelReminder(reminderId: number): Promise<void> {
    if (reminderId < 0) {
      return;
    }
    try {
      await reminderAgentManager.cancelReminder(reminderId);
      logger.info(`Reminder ${reminderId} canceled.`);
    } catch (err) {
      logger.error(`Cancel reminder failed code:${err.code} message:${err.message}`);
    }
  }
}