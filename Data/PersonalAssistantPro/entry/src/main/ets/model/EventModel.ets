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

@Observed
export class Event {
  id: number = 0;
  title: string;
  description: string;
  startTime: number;
  endTime: number;
  isAllDay: boolean;
  reminderId: number;

  constructor(title: string, startTime: number, endTime: number, description: string = '', isAllDay: boolean = false,
    reminderId: number = -1) {
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
  //  新增方法 1：查询所有日程 (供 DataExportService 使用)
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
  //  新增方法 2：查询时间段日程 (供日历视图筛选使用)
  // ========================================================
  static async queryRange(start: number, end: number): Promise<Event[]> {
    let predicates = new relationalStore.RdbPredicates(TABLE_EVENT);

    // 数据库直接筛选：
    // 1. 日程开始时间早于查询结束时间
    // 2. 且 日程结束时间晚于查询开始时间
    predicates.lessThanOrEqualTo('start_time', end)
      .and()
      .greaterThanOrEqualTo('end_time', start)
      .orderByAsc('start_time');

    try {
      const resultSet = await RdbHelper.getInstance().query(predicates);
      return EventModel.resultSetToEvents(resultSet);
    } catch (err) {
      logger.error('Query range events failed', JSON.stringify(err));
      return [];
    }
  }

  static async queryByMonth(year: number, month: number): Promise<Event[]> {
    try {
      // 1. 月初：year年 month月(索引需-1) 1日 00:00:00
      const startOfMonth = new Date(year, month - 1, 1).getTime();

      // 2. 月末：下个月的第0天，即本月的最后一天 23:59:59
      // 注意：Date构造函数里 month 不用减1，因为我们要找的是"下个月的前一天"
      const endOfMonth = new Date(year, month, 0, 23, 59, 59, 999).getTime();

      logger.info(`Querying month: ${year}-${month}, Range: ${startOfMonth} - ${endOfMonth}`);

      // 3. 复用 queryRange
      //  [修复] ArkTS 不允许静态方法用 this，改为类名调用
      return await EventModel.queryRange(startOfMonth, endOfMonth);
    } catch (err) {
      logger.error('Query by month failed', JSON.stringify(err));
      return [];
    }
  }

  static async queryById(id: number): Promise<Event | undefined> {
    logger.error(`[DEBUG_2_MODEL] queryById 被调用，查询 ID: ${id}`); // [DEBUG]

    let predicates = new relationalStore.RdbPredicates(TABLE_EVENT);
    predicates.equalTo('id', id);

    try {
      const resultSet = await RdbHelper.getInstance().query(predicates);
      const events = EventModel.resultSetToEvents(resultSet);
      if (events.length > 0) {
        logger.error(`[DEBUG_2_MODEL] 数据库查到了! 时间戳: ${events[0].startTime}`); // [DEBUG]
        return events[0];
      } else {
        logger.error(`[DEBUG_2_MODEL] 数据库查无此ID`); // [DEBUG]
      }
    } catch (err) {
      logger.error('Query event by id failed', JSON.stringify(err));
    }
    return undefined;
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
    if (!resultSet) {
      return events;
    }

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