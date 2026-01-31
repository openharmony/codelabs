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
      const timeStr = isTomorrow ? `明日 ${reminderTime.getHours()}:${reminderTime.getMinutes()}` : `${reminderTime.getHours()}:${reminderTime.getMinutes()}`;
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
    if (reminderId < 0) return;
    try {
      await reminderAgentManager.cancelReminder(reminderId);
      console.info(`[ReminderHelper] 取消成功, ID: ${reminderId}`);
    } catch (err) {
      console.error(`[ReminderHelper] 取消失败: ${JSON.stringify(err)}`);
    }
  }
}