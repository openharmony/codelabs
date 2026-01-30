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
 * Schedule 模型单元测试
 */
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from '@ohos/hypium';
import { Schedule, ScheduleInit } from '../../main/ets/model/Schedule';
import { relationalStore } from '@kit.ArkData';

export default function scheduleModelTest() {
  describe('ScheduleModelTest', () => {
    
    describe('构造函数测试', () => {
      it('创建默认 Schedule 实例', 0, () => {
        const schedule = new Schedule();
        
        expect(schedule.id).assertContain('schedule-');
        expect(schedule.title).assertEqual('');
        expect(schedule.description).assertEqual('');
        expect(schedule.startTime).assertEqual(0);
        expect(schedule.endTime).assertEqual(0);
        expect(schedule.isAllDay).assertFalse();
        expect(schedule.location).assertEqual('');
        expect(schedule.tagIds.length).assertEqual(0);
        expect(schedule.color).assertEqual('#4ECDC4');
        expect(schedule.repeatRule).assertEqual('');
        expect(schedule.reminderIds.length).assertEqual(0);
        expect(schedule.recordingIds.length).assertEqual(0);
        expect(schedule.externalAppLink).assertEqual('');
        expect(schedule.isDeleted).assertFalse();
        expect(schedule.createdAt).assertLarger(0);
        expect(schedule.updatedAt).assertLarger(0);
      });

      it('使用 ScheduleInit 创建实例', 0, () => {
        const now = Date.now();
        const init: ScheduleInit = {
          id: 'test-schedule-001',
          title: '测试日程',
          description: '这是一个测试日程',
          startTime: now,
          endTime: now + 3600000, // 1小时后
          isAllDay: false,
          location: '会议室A',
          tagIds: ['tag-work', 'tag-meeting'],
          color: '#FF6B6B',
          repeatRule: 'daily',
          reminderIds: ['reminder-001'],
          recordingIds: ['recording-001'],
          externalAppLink: 'https://example.com'
        };
        
        const schedule = new Schedule(init);
        
        expect(schedule.id).assertEqual('test-schedule-001');
        expect(schedule.title).assertEqual('测试日程');
        expect(schedule.description).assertEqual('这是一个测试日程');
        expect(schedule.startTime).assertEqual(now);
        expect(schedule.endTime).assertEqual(now + 3600000);
        expect(schedule.isAllDay).assertFalse();
        expect(schedule.location).assertEqual('会议室A');
        expect(schedule.tagIds.length).assertEqual(2);
        expect(schedule.tagIds[0]).assertEqual('tag-work');
        expect(schedule.color).assertEqual('#FF6B6B');
        expect(schedule.repeatRule).assertEqual('daily');
        expect(schedule.reminderIds.length).assertEqual(1);
        expect(schedule.recordingIds.length).assertEqual(1);
        expect(schedule.externalAppLink).assertEqual('https://example.com');
      });

      it('部分初始化参数创建实例', 0, () => {
        const init: ScheduleInit = {
          title: '部分初始化日程',
          isAllDay: true
        };
        
        const schedule = new Schedule(init);
        
        expect(schedule.id).assertContain('schedule-');
        expect(schedule.title).assertEqual('部分初始化日程');
        expect(schedule.isAllDay).assertTrue();
        expect(schedule.description).assertEqual('');
      });
    });

    describe('getDuration 方法测试', () => {
      it('计算1小时时长', 0, () => {
        const now = Date.now();
        const schedule = new Schedule({
          startTime: now,
          endTime: now + 3600000 // 1小时 = 3600000毫秒
        });
        
        expect(schedule.getDuration()).assertEqual(60);
      });

      it('计算30分钟时长', 0, () => {
        const now = Date.now();
        const schedule = new Schedule({
          startTime: now,
          endTime: now + 1800000 // 30分钟
        });
        
        expect(schedule.getDuration()).assertEqual(30);
      });

      it('计算0分钟时长', 0, () => {
        const now = Date.now();
        const schedule = new Schedule({
          startTime: now,
          endTime: now
        });
        
        expect(schedule.getDuration()).assertEqual(0);
      });

      it('计算2小时15分钟时长', 0, () => {
        const now = Date.now();
        const schedule = new Schedule({
          startTime: now,
          endTime: now + (2 * 60 + 15) * 60000 // 135分钟
        });
        
        expect(schedule.getDuration()).assertEqual(135);
      });
    });

    describe('isOnDate 方法测试', () => {
      it('日程在指定日期内', 0, () => {
        // 创建一个在 2024-01-15 10:00-12:00 的日程
        const startTime = new Date(2024, 0, 15, 10, 0, 0).getTime();
        const endTime = new Date(2024, 0, 15, 12, 0, 0).getTime();
        
        const schedule = new Schedule({
          startTime: startTime,
          endTime: endTime
        });
        
        const checkDate = new Date(2024, 0, 15);
        expect(schedule.isOnDate(checkDate)).assertTrue();
      });

      it('日程不在指定日期内', 0, () => {
        const startTime = new Date(2024, 0, 15, 10, 0, 0).getTime();
        const endTime = new Date(2024, 0, 15, 12, 0, 0).getTime();
        
        const schedule = new Schedule({
          startTime: startTime,
          endTime: endTime
        });
        
        const checkDate = new Date(2024, 0, 16);
        expect(schedule.isOnDate(checkDate)).assertFalse();
      });

      it('跨天日程第一天', 0, () => {
        // 创建一个跨天日程：1月15日 22:00 到 1月16日 02:00
        const startTime = new Date(2024, 0, 15, 22, 0, 0).getTime();
        const endTime = new Date(2024, 0, 16, 2, 0, 0).getTime();
        
        const schedule = new Schedule({
          startTime: startTime,
          endTime: endTime
        });
        
        const checkDate = new Date(2024, 0, 15);
        expect(schedule.isOnDate(checkDate)).assertTrue();
      });

      it('跨天日程第二天', 0, () => {
        const startTime = new Date(2024, 0, 15, 22, 0, 0).getTime();
        const endTime = new Date(2024, 0, 16, 2, 0, 0).getTime();
        
        const schedule = new Schedule({
          startTime: startTime,
          endTime: endTime
        });
        
        const checkDate = new Date(2024, 0, 16);
        expect(schedule.isOnDate(checkDate)).assertTrue();
      });
    });

    describe('toDbObject 方法测试', () => {
      it('转换为数据库对象', 0, () => {
        const now = Date.now();
        const schedule = new Schedule({
          id: 'db-test-001',
          title: '数据库测试',
          description: '测试描述',
          startTime: now,
          endTime: now + 3600000,
          isAllDay: true,
          location: '办公室',
          tagIds: ['tag-1', 'tag-2'],
          color: '#FF0000',
          repeatRule: 'weekly',
          reminderIds: ['rem-1'],
          recordingIds: ['rec-1'],
          externalAppLink: 'https://test.com'
        });
        
        const dbObj = schedule.toDbObject();
        
        expect(dbObj['id']).assertEqual('db-test-001');
        expect(dbObj['title']).assertEqual('数据库测试');
        expect(dbObj['description']).assertEqual('测试描述');
        expect(dbObj['startTime']).assertEqual(now);
        expect(dbObj['endTime']).assertEqual(now + 3600000);
        expect(dbObj['isAllDay']).assertEqual(1);  // boolean 转为 1/0
        expect(dbObj['location']).assertEqual('办公室');
        expect(dbObj['tagIds']).assertEqual('["tag-1","tag-2"]');  // JSON 字符串
        expect(dbObj['color']).assertEqual('#FF0000');
        expect(dbObj['repeatRule']).assertEqual('weekly');
        expect(dbObj['reminderIds']).assertEqual('["rem-1"]');
        expect(dbObj['recordingIds']).assertEqual('["rec-1"]');
        expect(dbObj['externalAppLink']).assertEqual('https://test.com');
        expect(dbObj['isDeleted']).assertEqual(0);
      });
    });

    describe('fromDbObject 静态方法测试', () => {
      it('从数据库对象创建实例', 0, () => {
        const now = Date.now();
        const dbObj: relationalStore.ValuesBucket = {
          id: 'from-db-001',
          title: '从数据库恢复',
          description: '数据库描述',
          startTime: now,
          endTime: now + 7200000,
          isAllDay: 0,
          location: '线上',
          tagIds: '["tag-a","tag-b","tag-c"]',
          color: '#00FF00',
          repeatRule: 'monthly',
          reminderIds: '["r1","r2"]',
          recordingIds: '["rec-a"]',
          externalAppLink: 'app://meeting',
          createdAt: now - 86400000,
          updatedAt: now,
          isDeleted: 0
        };
        
        const schedule = Schedule.fromDbObject(dbObj);
        
        expect(schedule.id).assertEqual('from-db-001');
        expect(schedule.title).assertEqual('从数据库恢复');
        expect(schedule.description).assertEqual('数据库描述');
        expect(schedule.startTime).assertEqual(now);
        expect(schedule.endTime).assertEqual(now + 7200000);
        expect(schedule.isAllDay).assertFalse();
        expect(schedule.location).assertEqual('线上');
        expect(schedule.tagIds.length).assertEqual(3);
        expect(schedule.tagIds[0]).assertEqual('tag-a');
        expect(schedule.color).assertEqual('#00FF00');
        expect(schedule.repeatRule).assertEqual('monthly');
        expect(schedule.reminderIds.length).assertEqual(2);
        expect(schedule.recordingIds.length).assertEqual(1);
        expect(schedule.externalAppLink).assertEqual('app://meeting');
        expect(schedule.createdAt).assertEqual(now - 86400000);
        expect(schedule.updatedAt).assertEqual(now);
        expect(schedule.isDeleted).assertFalse();
      });

      it('处理空或无效的 JSON 字段', 0, () => {
        const now = Date.now();
        const dbObj: relationalStore.ValuesBucket = {
          id: 'from-db-002',
          title: '空数组测试',
          description: '',
          startTime: now,
          endTime: now,
          isAllDay: 1,
          location: '',
          tagIds: '',  // 空字符串
          color: '',
          repeatRule: '',
          reminderIds: 'invalid-json',  // 无效 JSON
          recordingIds: '',
          externalAppLink: '',
          createdAt: now,
          updatedAt: now,
          isDeleted: 1
        };
        
        const schedule = Schedule.fromDbObject(dbObj);
        
        expect(schedule.tagIds.length).assertEqual(0);
        expect(schedule.reminderIds.length).assertEqual(0);  // 解析失败返回空数组
        expect(schedule.isAllDay).assertTrue();
        expect(schedule.isDeleted).assertTrue();
      });
    });
  });
}
