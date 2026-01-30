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
 * Reminder 模型单元测试
 */
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from '@ohos/hypium';
import { Reminder, ReminderInit } from '../../main/ets/model/Reminder';
import { ReminderTiming, ReminderType } from '../../main/ets/common/constants/Constants';
import { relationalStore } from '@kit.ArkData';

export default function reminderModelTest() {
  describe('ReminderModelTest', () => {
    
    describe('构造函数测试', () => {
      it('创建默认 Reminder 实例', 0, () => {
        const reminder = new Reminder();
        
        expect(reminder.id).assertContain('reminder-');
        expect(reminder.scheduleId).assertEqual('');
        expect(reminder.timing).assertEqual(ReminderTiming.ON_TIME);
        expect(reminder.type).assertEqual(ReminderType.NOTIFICATION);
        expect(reminder.triggerTime).assertEqual(0);
        expect(reminder.isTriggered).assertFalse();
        expect(reminder.customSound).assertEqual('');
        expect(reminder.vibrationEnabled).assertTrue();
        expect(reminder.createdAt).assertLarger(0);
      });

      it('使用 ReminderInit 创建实例', 0, () => {
        const triggerTime = Date.now() + 3600000;
        const init: ReminderInit = {
          id: 'reminder-test-001',
          scheduleId: 'schedule-001',
          timing: ReminderTiming.MINUTES_15,
          type: ReminderType.POPUP,
          triggerTime: triggerTime,
          isTriggered: false,
          customSound: 'alarm.mp3',
          vibrationEnabled: false
        };
        
        const reminder = new Reminder(init);
        
        expect(reminder.id).assertEqual('reminder-test-001');
        expect(reminder.scheduleId).assertEqual('schedule-001');
        expect(reminder.timing).assertEqual(ReminderTiming.MINUTES_15);
        expect(reminder.type).assertEqual(ReminderType.POPUP);
        expect(reminder.triggerTime).assertEqual(triggerTime);
        expect(reminder.isTriggered).assertFalse();
        expect(reminder.customSound).assertEqual('alarm.mp3');
        expect(reminder.vibrationEnabled).assertFalse();
      });

      it('部分初始化参数创建实例', 0, () => {
        const init: ReminderInit = {
          scheduleId: 'schedule-partial',
          timing: ReminderTiming.HOURS_1
        };
        
        const reminder = new Reminder(init);
        
        expect(reminder.id).assertContain('reminder-');
        expect(reminder.scheduleId).assertEqual('schedule-partial');
        expect(reminder.timing).assertEqual(ReminderTiming.HOURS_1);
        expect(reminder.type).assertEqual(ReminderType.NOTIFICATION);  // 默认值
      });
    });

    describe('calculateTriggerTime 方法测试', () => {
      it('准时提醒', 0, () => {
        const reminder = new Reminder({
          timing: ReminderTiming.ON_TIME
        });
        const scheduleStartTime = 1705305600000;  // 示例时间戳
        
        reminder.calculateTriggerTime(scheduleStartTime);
        
        expect(reminder.triggerTime).assertEqual(scheduleStartTime);
      });

      it('提前5分钟提醒', 0, () => {
        const reminder = new Reminder({
          timing: ReminderTiming.MINUTES_5
        });
        const scheduleStartTime = 1705305600000;
        
        reminder.calculateTriggerTime(scheduleStartTime);
        
        expect(reminder.triggerTime).assertEqual(scheduleStartTime - 5 * 60 * 1000);
      });

      it('提前15分钟提醒', 0, () => {
        const reminder = new Reminder({
          timing: ReminderTiming.MINUTES_15
        });
        const scheduleStartTime = 1705305600000;
        
        reminder.calculateTriggerTime(scheduleStartTime);
        
        expect(reminder.triggerTime).assertEqual(scheduleStartTime - 15 * 60 * 1000);
      });

      it('提前30分钟提醒', 0, () => {
        const reminder = new Reminder({
          timing: ReminderTiming.MINUTES_30
        });
        const scheduleStartTime = 1705305600000;
        
        reminder.calculateTriggerTime(scheduleStartTime);
        
        expect(reminder.triggerTime).assertEqual(scheduleStartTime - 30 * 60 * 1000);
      });

      it('提前1小时提醒', 0, () => {
        const reminder = new Reminder({
          timing: ReminderTiming.HOURS_1
        });
        const scheduleStartTime = 1705305600000;
        
        reminder.calculateTriggerTime(scheduleStartTime);
        
        expect(reminder.triggerTime).assertEqual(scheduleStartTime - 60 * 60 * 1000);
      });

      it('提前1天提醒', 0, () => {
        const reminder = new Reminder({
          timing: ReminderTiming.DAYS_1
        });
        const scheduleStartTime = 1705305600000;
        
        reminder.calculateTriggerTime(scheduleStartTime);
        
        // DAYS_1 = 1440 分钟 = 24小时
        expect(reminder.triggerTime).assertEqual(scheduleStartTime - 1440 * 60 * 1000);
      });

      it('提前1周提醒', 0, () => {
        const reminder = new Reminder({
          timing: ReminderTiming.WEEKS_1
        });
        const scheduleStartTime = 1705305600000;
        
        reminder.calculateTriggerTime(scheduleStartTime);
        
        // WEEKS_1 = 10080 分钟 = 7天
        expect(reminder.triggerTime).assertEqual(scheduleStartTime - 10080 * 60 * 1000);
      });
    });

    describe('getTimingText 方法测试', () => {
      it('准时提醒文本', 0, () => {
        const reminder = new Reminder({ timing: ReminderTiming.ON_TIME });
        expect(reminder.getTimingText()).assertEqual('准时提醒');
      });

      it('提前5分钟文本', 0, () => {
        const reminder = new Reminder({ timing: ReminderTiming.MINUTES_5 });
        expect(reminder.getTimingText()).assertEqual('提前5分钟');
      });

      it('提前10分钟文本', 0, () => {
        const reminder = new Reminder({ timing: ReminderTiming.MINUTES_10 });
        expect(reminder.getTimingText()).assertEqual('提前10分钟');
      });

      it('提前15分钟文本', 0, () => {
        const reminder = new Reminder({ timing: ReminderTiming.MINUTES_15 });
        expect(reminder.getTimingText()).assertEqual('提前15分钟');
      });

      it('提前30分钟文本', 0, () => {
        const reminder = new Reminder({ timing: ReminderTiming.MINUTES_30 });
        expect(reminder.getTimingText()).assertEqual('提前30分钟');
      });

      it('提前1小时文本', 0, () => {
        const reminder = new Reminder({ timing: ReminderTiming.HOURS_1 });
        expect(reminder.getTimingText()).assertEqual('提前1小时');
      });

      it('提前2小时文本', 0, () => {
        const reminder = new Reminder({ timing: ReminderTiming.HOURS_2 });
        expect(reminder.getTimingText()).assertEqual('提前2小时');
      });

      it('提前1天文本', 0, () => {
        const reminder = new Reminder({ timing: ReminderTiming.DAYS_1 });
        expect(reminder.getTimingText()).assertEqual('提前1天');
      });

      it('提前2天文本', 0, () => {
        const reminder = new Reminder({ timing: ReminderTiming.DAYS_2 });
        expect(reminder.getTimingText()).assertEqual('提前2天');
      });

      it('提前1周文本', 0, () => {
        const reminder = new Reminder({ timing: ReminderTiming.WEEKS_1 });
        expect(reminder.getTimingText()).assertEqual('提前1周');
      });
    });

    describe('toDbObject 方法测试', () => {
      it('转换为数据库对象', 0, () => {
        const triggerTime = Date.now();
        const reminder = new Reminder({
          id: 'db-reminder-001',
          scheduleId: 'schedule-db',
          timing: ReminderTiming.HOURS_2,
          type: ReminderType.EMAIL,
          triggerTime: triggerTime,
          isTriggered: true,
          customSound: 'custom.mp3',
          vibrationEnabled: false
        });
        
        const dbObj = reminder.toDbObject();
        
        expect(dbObj['id']).assertEqual('db-reminder-001');
        expect(dbObj['scheduleId']).assertEqual('schedule-db');
        expect(dbObj['timing']).assertEqual(ReminderTiming.HOURS_2);
        expect(dbObj['type']).assertEqual(ReminderType.EMAIL);
        expect(dbObj['triggerTime']).assertEqual(triggerTime);
        expect(dbObj['isTriggered']).assertEqual(1);  // boolean 转为 1
        expect(dbObj['customSound']).assertEqual('custom.mp3');
        expect(dbObj['vibrationEnabled']).assertEqual(0);  // boolean 转为 0
      });
    });

    describe('fromDbObject 静态方法测试', () => {
      it('从数据库对象创建实例', 0, () => {
        const now = Date.now();
        const dbObj: relationalStore.ValuesBucket = {
          id: 'from-db-reminder',
          scheduleId: 'schedule-from-db',
          timing: ReminderTiming.DAYS_2,
          type: ReminderType.SMS,
          triggerTime: now,
          isTriggered: 0,
          customSound: 'ring.mp3',
          vibrationEnabled: 1,
          createdAt: now - 3600000
        };
        
        const reminder = Reminder.fromDbObject(dbObj);
        
        expect(reminder.id).assertEqual('from-db-reminder');
        expect(reminder.scheduleId).assertEqual('schedule-from-db');
        expect(reminder.timing).assertEqual(ReminderTiming.DAYS_2);
        expect(reminder.type).assertEqual(ReminderType.SMS);
        expect(reminder.triggerTime).assertEqual(now);
        expect(reminder.isTriggered).assertFalse();
        expect(reminder.customSound).assertEqual('ring.mp3');
        expect(reminder.vibrationEnabled).assertTrue();
        expect(reminder.createdAt).assertEqual(now - 3600000);
      });
    });
  });
}
