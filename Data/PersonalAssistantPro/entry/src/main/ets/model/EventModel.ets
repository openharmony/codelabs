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
// entry/src/main/ets/model/EventModel.ets
import relationalStore from '@ohos.data.relationalStore';
import { RdbHelper, TABLE_EVENT } from '../common/database/RdbHelper';
import { Logger } from '../common/utils/Logger';

const logger = new Logger('EventModel');

export class Event {
  id: number = 0;
  title: string;
  description: string;
  startTime: number;
  endTime: number;
  isAllDay: boolean;
  reminderId: number;

  constructor(title: string, startTime: number, endTime: number, description: string = '', isAllDay: boolean = false, reminderId: number = -1) {
    this.title = title;
    this.startTime = startTime;
    this.endTime = endTime;
    this.description = description;
    this.isAllDay = isAllDay;
    this.reminderId = reminderId;
  }
}

export class EventModel {

  static async update(event: Event): Promise<number> {
    const values: relationalStore.ValuesBucket = {
      'title': event.title,
      'description': event.description,
      'start_time': event.startTime,
      'end_time': event.endTime,
      'is_all_day': event.isAllDay ? 1 : 0,
      'reminder_id': event.reminderId
    };

    let predicates = new relationalStore.RdbPredicates(TABLE_EVENT);
    predicates.equalTo('id', event.id);

    try {
      const rows = await RdbHelper.getInstance().update(values, predicates);
      return rows;
    } catch (err) {
      logger.error('Update event failed', JSON.stringify(err));
      return -1;
    }
  }

  static async deleteById(id: number): Promise<boolean> {
    let predicates = new relationalStore.RdbPredicates(TABLE_EVENT);
    predicates.equalTo('id', id);

    try {
      const rows = await RdbHelper.getInstance().delete(predicates);
      return rows > 0;
    } catch (err) {
      logger.error('Delete event failed', JSON.stringify(err));
      return false;
    }
  }

  static async insert(event: Event): Promise<number> {
    const values: relationalStore.ValuesBucket = {
      'title': event.title,
      'description': event.description,
      'start_time': event.startTime,
      'end_time': event.endTime,
      'is_all_day': event.isAllDay ? 1 : 0,
      'reminder_id': event.reminderId
    };

    try {
      const id = await RdbHelper.getInstance().insert(TABLE_EVENT, values);
      return id;
    } catch (err) {
      logger.error('Insert event failed', JSON.stringify(err));
      return -1;
    }
  }

  // ========================================================
  // ✅ 新增方法 1：查询所有日程 (供 DataExportService 使用)
  // ========================================================
  static async queryAll(): Promise<Event[]> {
    let predicates = new relationalStore.RdbPredicates(TABLE_EVENT);
    predicates.orderByDesc('start_time');
    try {
      const resultSet = await RdbHelper.getInstance().query(predicates);
      return EventModel.resultSetToEvents(resultSet);
    } catch (err) {
      logger.error('Query all events failed', JSON.stringify(err));
      return [];
    }
  }

  // ========================================================
  // ✅ 新增方法 2：查询时间段日程 (供日历视图筛选使用)
  // ========================================================
  static async queryRange(start: number, end: number): Promise<Event[]> {
    let predicates = new relationalStore.RdbPredicates(TABLE_EVENT);
    predicates.orderByAsc('start_time');

    try {
      const resultSet = await RdbHelper.getInstance().query(predicates);
      const allEvents = EventModel.resultSetToEvents(resultSet);
      // 内存筛选: (EventStart < RangeEnd) AND (EventEnd > RangeStart)
      return allEvents.filter(e => e.startTime < end && e.endTime > start);
    } catch (err) {
      logger.error('Query range events failed', JSON.stringify(err));
      return [];
    }
  }

  static async queryUpcoming(): Promise<Event[]> {
    const now = new Date().getTime();
    let predicates = new relationalStore.RdbPredicates(TABLE_EVENT);
    predicates.greaterThanOrEqualTo('end_time', now);
    predicates.orderByAsc('start_time');

    try {
      const resultSet = await RdbHelper.getInstance().query(predicates);
      return EventModel.resultSetToEvents(resultSet);
    } catch (err) {
      logger.error('Query upcoming events failed', JSON.stringify(err));
      return [];
    }
  }

  static async search(keyword: string): Promise<Event[]> {
    try {
      const resultSet = await RdbHelper.getInstance().queryByLike(
        TABLE_EVENT,
        ['title', 'description'],
        keyword
      );
      return EventModel.resultSetToEvents(resultSet);
    } catch (err) {
      logger.error('Search events failed', JSON.stringify(err));
      return [];
    }
  }

  private static resultSetToEvents(resultSet: relationalStore.ResultSet): Event[] {
    const events: Event[] = [];
    if (!resultSet) return events;

    try {
      while (resultSet.goToNextRow()) {
        const e = new Event(
          resultSet.getString(resultSet.getColumnIndex('title')),
          resultSet.getLong(resultSet.getColumnIndex('start_time')),
          resultSet.getLong(resultSet.getColumnIndex('end_time'))
        );
        e.id = resultSet.getLong(resultSet.getColumnIndex('id'));
        e.description = resultSet.getString(resultSet.getColumnIndex('description'));
        e.isAllDay = resultSet.getLong(resultSet.getColumnIndex('is_all_day')) === 1;

        const reminderIndex = resultSet.getColumnIndex('reminder_id');
        e.reminderId = reminderIndex >= 0 ? resultSet.getLong(reminderIndex) : -1;

        events.push(e);
      }
    } catch (e) {
      logger.error('Parse resultSet error', e);
    } finally {
      resultSet.close();
    }
    return events;
  }
}