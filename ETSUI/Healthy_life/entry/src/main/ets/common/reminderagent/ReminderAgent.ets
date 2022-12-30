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

import reminderAgent from '@ohos.reminderAgent'
import notification from '@ohos.notification'
import { Logger } from '../utils/log/Logger'

// publishReminder
function publishReminder(params) {
    if(!params) return;
    let timer = fetchData(params);
    reminderAgent.publishReminder(timer).then((reminderId) => {
        Logger.info("reminderAgent", `promise, reminderId: ${reminderId}`);
    }).catch((err) => {
        Logger.error("reminderAgent", `publishReminder err: ${err}`);
    })
}

// cancelReminder
function cancelReminder(reminderId) {
    if(!reminderId) return;
    reminderAgent.cancelReminder(reminderId).then((data) => {
        Logger.info("reminderAgent", `cancelReminder promise: ${data}`);
    }).catch((err) => {
        Logger.error("reminderAgent", `cancelReminder err: ${err}`);
    })
}

// fetchData
function fetchData(params): reminderAgent.ReminderRequestAlarm {
    return {
        reminderType: reminderAgent.ReminderType.REMINDER_TYPE_ALARM,
        hour: params.hour || 0,
        minute: params.minute || 0,
        daysOfWeek: params.daysOfWeek || [],
        wantAgent: {
            pkgName: "com.example.exercisehealth",
            abilityName: "MainAbility"
        },
        title: params.title || '',
        content: params.content || '',
        notificationId: params.notificationId || -1,
        slotType: notification.SlotType.SOCIAL_COMMUNICATION
    }
}

// hasNotificationId
async function hasNotificationId(params: number) {
    if(!params) return;
    await reminderAgent.getValidReminders().then((reminders) => {
        if (!reminders.length) {
            return false;
        }
        let notificationIdList = [];
        for (let i = 0; i < reminders.length; i++) {
            notificationIdList.push(reminders[i].notificationId)
        }
        const flag = notificationIdList.indexOf(params);

        return flag === -1 ? false : true;
    })
}

const reminder = {
    publishReminder,
    cancelReminder,
    hasNotificationId
}

export default reminder