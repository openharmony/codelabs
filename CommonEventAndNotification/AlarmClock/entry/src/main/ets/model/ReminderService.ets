/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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

import reminderAgent from '@ohos.reminderAgentManager';
import notification from '@ohos.notificationManager';
import { CommonConstants } from '../common/constants/CommonConstants';
import ReminderItem from '../viewmodel/ReminderItemBean';

/**
 * Base on ohos reminder agent service
 */
export default class ReminderService {
  /**
   * open notification permission
   */
  public openNotificationPermission() {
    notification.requestEnableNotification().then(() => {
      console.info('Enable notification success');
    }).catch((err: Error) => {
      console.error('Enable notification failed because ' + JSON.stringify(err));
    });
  }

  /**
   * Adding and modifying alarm reminders
   *
   * @param alarmItem ReminderItem
   * @param callback callback
   */
  public addReminder(alarmItem: ReminderItem, callback?: (reminderId: number) => void) {
    let reminder = this.initReminder(alarmItem);
    reminderAgent.publishReminder(reminder, (err, reminderId) => {
      if (callback != null) {
        callback(reminderId);
      }
    });
  }

  /**
   * Adding and modifying alarm reminders
   *
   * @param reminderId number
   */
  public deleteReminder(reminderId: number) {
    reminderAgent.cancelReminder(reminderId);
  }

  private initReminder(item: ReminderItem): reminderAgent.ReminderRequestAlarm{
    return {
      reminderType: item.remindType,
      hour: item.hour,
      minute: item.minute,
      daysOfWeek: item.repeatDays,
      title: item.name,
      ringDuration: item.duration * CommonConstants.DEFAULT_TOTAL_MINUTE,
      snoozeTimes: item.intervalTimes,
      timeInterval: item.intervalMinute,
      actionButton: [
        {
          title: '关闭',
          type: reminderAgent.ActionButtonType.ACTION_BUTTON_TYPE_CLOSE
        },
        {
          title: '稍后提醒',
          type: reminderAgent.ActionButtonType.ACTION_BUTTON_TYPE_SNOOZE
        },
      ],
      wantAgent: {
        pkgName: CommonConstants.BUNDLE_NAME,
        abilityName: CommonConstants.ABILITY_NAME
      },
      notificationId: item.notificationId,
      expiredContent: 'this reminder has expired',
      snoozeContent: 'remind later',
      slotType: notification.SlotType.SERVICE_INFORMATION
    }
  }
}