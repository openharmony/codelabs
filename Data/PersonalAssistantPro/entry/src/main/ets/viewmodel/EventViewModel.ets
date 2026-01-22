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
// src/main/ets/viewmodel/EventViewModel.ets
import { Event, EventModel } from '../model/EventModel';
import { DateUtils } from '../common/utils/DateUtils';
import { Logger } from '../common/utils/Logger';

/**
 * 📝 [新增] 定义状态返回接口
 * 因为本文件是逻辑层(.ts)，无法直接返回 $r对象，
 * 所以返回 Key 和 参数，由 UI 层通过 $r('app.string.' + key, param) 解析
 */
export interface EventStatusInfo {
  key: string; // string.json 中的键名
  param?: number; // 动态参数 (如: 3天后 的 3)
}

/**
 * 日程业务逻辑模型 (ViewModel)
 * 处理日程冲突检测、状态判断、时间计算等复杂业务
 */
export class EventViewModel {
  private logger = new Logger('EventViewModel');

  /**
   * 检查日程起止时间是否合法
   */
  public validateEventTime(start: number, end: number): boolean {
    if (start >= end) {
      this.logger.warn('Validate failed: Start time must be before end time');
      return false;
    }
    return true;
  }

  /**
   * 检查是否与现有日程冲突
   */
  public async checkConflict(startTime: number, endTime: number, excludeId?: number): Promise<boolean> {
    this.logger.info('Checking for event conflicts...');

    try {
      const allEvents = await EventModel.queryUpcoming();

      for (const event of allEvents) {
        if (excludeId && event.id === excludeId) {
          continue;
        }

        const overlapStart = Math.max(startTime, event.startTime);
        const overlapEnd = Math.min(endTime, event.endTime);

        if (overlapStart < overlapEnd) {
          this.logger.warn(`Conflict detected with event: "${event.title}" (${DateUtils.formatFull(event.startTime)})`);
          return true; // 存在冲突
        }
      }
    } catch (e) {
      this.logger.error('Check conflict failed');
    }

    return false; // 无冲突
  }

  /**
   * 📝 [修改] 获取日程的状态信息
   * 原方法名: getEventStatusText -> 建议改为 getEventStatusInfo
   * @param event
   * @returns EventStatusInfo 包含资源Key和参数
   */
  public getEventStatusInfo(event: Event): EventStatusInfo {
    const now = Date.now();

    if (now > event.endTime) {
      // 对应: "已结束" -> calendar_status_ended
      return { key: 'calendar_status_ended' };
    } else if (now >= event.startTime && now <= event.endTime) {
      // 对应: "进行中" -> calendar_status_ongoing
      return { key: 'calendar_status_ongoing' };
    } else {
      // 计算还有多久开始
      const diff = event.startTime - now;
      const hours = Math.floor(diff / (3600 * 1000));

      if (hours > 48) {
        const days = Math.floor(hours / 24);
        // 对应: "%d天后" -> calendar_time_days_later
        return { key: 'calendar_time_days_later', param: days };
      } else if (hours > 24) {
        // 对应: "明天" -> calendar_time_tomorrow
        return { key: 'calendar_time_tomorrow' };
      } else if (hours > 0) {
        // 对应: "%d小时后" -> calendar_time_hours_later
        return { key: 'calendar_time_hours_later', param: hours };
      } else {
        const mins = Math.floor(diff / (60 * 1000));
        // 对应: "%d分钟后" -> calendar_time_mins_later
        return { key: 'calendar_time_mins_later', param: Math.max(1, mins) };
      }
    }
  }

  /**
   * 获取日程状态对应的颜色资源 (用于 UI 展示)
   * @param event
   */
  public getEventStatusColor(event: Event): string {
    const now = Date.now();
    if (now > event.endTime) {
      return '#999999'; // 灰色 - 已结束
    } else if (now >= event.startTime && now <= event.endTime) {
      return '#007DFF'; // 蓝色 - 进行中
    } else {
      return '#FF9C00'; // 橙色 - 未开始
    }
  }

  /**
   * 将日程按日期分组 (Group By Date)
   */
  public groupEventsByDate(events: Array<Event>): Map<string, Array<Event>> {
    const map = new Map<string, Array<Event>>();

    events.forEach(event => {
      const dateKey = DateUtils.formatDate(event.startTime);
      if (!map.has(dateKey)) {
        map.set(dateKey, []);
      }
      map.get(dateKey)?.push(event);
    });

    return map;
  }
}