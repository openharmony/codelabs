/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

export class ReminderHelper {
  /**
   * 发布一个重复的闹钟提醒
   * @param title 提醒标题 (习惯名称)
   * @param hour 小时
   * @param minute 分钟
   * @returns Promise<number> 返回提醒ID
   */
  static async publishReminder(title: string, hour: number, minute: number): Promise<number> {
    const now = new Date();
    const reminderTime = new Date();
    reminderTime.setHours(hour, minute, 0, 0);

    let isTomorrow = false;
    if (reminderTime <= now) {
      reminderTime.setDate(reminderTime.getDate() + 1);
      isTomorrow = true;
    }

    const reminderRequest: reminderAgentManager.ReminderRequestAlarm = {
      reminderType: reminderAgentManager.ReminderType.REMINDER_TYPE_ALARM,
      hour: reminderTime.getHours(),
      minute: reminderTime.getMinutes(),
      daysOfWeek: [1, 2, 3, 4, 5, 6, 7],
      title: '该打卡啦！',
      content: `别忘了坚持习惯：${title}`,
      ringDuration: 5,
      snoozeTimes: 0,
      timeInterval: 0,
      actionButton: [
        { title: '去打卡', type: reminderAgentManager.ActionButtonType.ACTION_BUTTON_TYPE_CLOSE }
      ]
    };

    try {
      const reminderId = await reminderAgentManager.publishReminder(reminderRequest);
      const timeStr = isTomorrow ? `明日 ${reminderTime.getHours()}:${reminderTime.getMinutes()}` :
        `${reminderTime.getHours()}:${reminderTime.getMinutes()}`;
      console.info(`[ReminderHelper] 发布成功, ID: ${reminderId}, 时间: ${timeStr}`);
      return reminderId;
    } catch (err) {
      console.error(`[ReminderHelper] 发布失败: ${JSON.stringify(err)}`);
      return -1;
    }
  }

  /**
   * 取消指定的提醒
   */
  static async cancelReminder(reminderId: number) {
    if (reminderId < 0) {
      return;
    }
    try {
      await reminderAgentManager.cancelReminder(reminderId);
      console.info(`[ReminderHelper] 取消成功, ID: ${reminderId}`);
    } catch (err) {
      console.error(`[ReminderHelper] 取消失败: ${JSON.stringify(err)}`);
    }
  }
}