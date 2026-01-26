/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-20
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { EventDao } from '../dao/EventDao';
import { CalendarEvent } from '../model/EventModel';
import common from '@ohos.app.ability.common';

interface SaveResult {
  success: boolean;
  id?: string;
  error?: string;
}

interface GetResult {
  success: boolean;
  data?: CalendarEvent;
  error?: string;
}

interface DeleteResult {
  success: boolean;
  error?: string;
}

interface ToggleResult {
  success: boolean;
  error?: string;
}

interface TodayEventsResult {
  success: boolean;
  data: CalendarEvent[];
  error?: string;
}

export class DataManager {
  private eventDao: EventDao;
  private static instance: DataManager | null = null;

  private constructor(context: common.Context) {
    this.eventDao = new EventDao(context);
  }

  // 单例模式获取实例
  static getInstance(context?: common.Context): DataManager {
    if (!DataManager.instance && context) {
      DataManager.instance = new DataManager(context);
    }
    return DataManager.instance!;
  }

  // 保存日程（新增或更新）
  async saveEvent(eventData: Partial<CalendarEvent>): Promise<SaveResult> {
    try {
      // 验证必填字段
      if (!eventData.title || !eventData.title.trim()) {
        return { success: false, error: '标题不能为空' };
      }

      if (!eventData.startTime || !eventData.endTime) {
        return { success: false, error: '开始时间和结束时间不能为空' };
      }

      if (eventData.startTime > eventData.endTime) {
        return { success: false, error: '开始时间不能晚于结束时间' };
      }

      // 构建完整的日程对象
      const now = Date.now();
      const event: CalendarEvent = {
        id: eventData.id || '0',
        title: eventData.title.trim(),
        description: eventData.description || '',
        startTime: eventData.startTime,
        endTime: eventData.endTime,
        allDay: eventData.allDay || false,
        location: eventData.location || '',
        calendarId: eventData.calendarId || 'default',
        color: eventData.color || '#2196F3',
        reminder: eventData.reminder !== undefined ? eventData.reminder : 0,
        repeatRule: eventData.repeatRule || '',
        completed: eventData.completed || false,
        created: eventData.id && eventData.created ? eventData.created : now,
        updated: now
      };

      // 保存到数据库
      const savedId = await this.eventDao.saveEvent(event);
      return { success: true, id: savedId };

    } catch (error) {
      console.error('Save event failed:', error);
      return {
        success: false,
        error: `保存失败: ${error instanceof Error ? error.message : '未知错误'}`
      };
    }
  }

  // 根据ID获取日程
  async getEventById(id: string): Promise<GetResult> {
    try {
      if (!id) {
        return { success: false, error: '无效的日程ID' };
      }

      const event = await this.eventDao.getEventById(id);

      if (!event) {
        return { success: false, error: '未找到对应的日程' };
      }

      return { success: true, data: event };

    } catch (error) {
      console.error('Get event by id failed:', error);
      return {
        success: false,
        error: `查询失败: ${error instanceof Error ? error.message : '未知错误'}`
      };
    }
  }

  // 获取今天的日程
  async getTodayEvents(): Promise<TodayEventsResult> {
    try {
      const today = new Date();
      today.setHours(0, 0, 0, 0);
      const tomorrow = new Date(today);
      tomorrow.setDate(tomorrow.getDate() + 1);

      const events = await this.eventDao.getEventsByDateRange(
        today.getTime(),
        tomorrow.getTime()
      );

      return { success: true, data: events };

    } catch (error) {
      console.error('Get today events failed:', error);
      return {
        success: false,
        data: [],
        error: `查询失败: ${error instanceof Error ? error.message : '未知错误'}`
      };
    }
  }

  // 获取未完成的日程
  async getPendingEvents(): Promise<CalendarEvent[]> {
    try {
      const allEvents = await this.eventDao.getAllEvents();
      return allEvents.filter(event => !event.completed);
    } catch (error) {
      console.error('Get pending events failed:', error);
      return [];
    }
  }

  // 删除日程
  async deleteEvent(id: string): Promise<DeleteResult> {
    try {
      if (!id) {
        return { success: false, error: '无效的日程ID' };
      }

      const deleted = await this.eventDao.deleteEvent(id);

      if (!deleted) {
        return { success: false, error: '删除失败，日程可能不存在' };
      }

      return { success: true };

    } catch (error) {
      console.error('Delete event failed:', error);
      return {
        success: false,
        error: `删除失败: ${error instanceof Error ? error.message : '未知错误'}`
      };
    }
  }

  // 标记日程完成/未完成
  async toggleEventCompletion(id: string, completed: boolean): Promise<ToggleResult> {
    try {
      if (!id) {
        return { success: false, error: '无效的日程ID' };
      }

      const updated = await this.eventDao.updateEventCompletion(id, completed);

      if (!updated) {
        return { success: false, error: '更新失败，日程可能不存在' };
      }

      return { success: true };

    } catch (error) {
      console.error('Toggle event completion failed:', error);
      return {
        success: false,
        error: `更新失败: ${error instanceof Error ? error.message : '未知错误'}`
      };
    }
  }

  // 搜索日程（按标题或描述）
  async searchEvents(keyword: string): Promise<CalendarEvent[]> {
    // 这里需要扩展EventDao来实现搜索功能
    // 暂时返回所有日程进行前端过滤
    try {
      const allEvents = await this.eventDao.getAllEvents();
      const lowerKeyword = keyword.toLowerCase();

      return allEvents.filter(event =>
      event.title.toLowerCase().includes(lowerKeyword) ||
        (event.description && event.description.toLowerCase().includes(lowerKeyword))
      );
    } catch (error) {
      console.error('Search events failed:', error);
      return [];
    }
  }

  // 获取全部日程
  async getAllEvents(): Promise<CalendarEvent[]> {
    try {
      const allEvents = await this.eventDao.getAllEvents();

      return allEvents
    } catch (error) {
      console.error('Get all events failed:', error);
      return [];
    }
  }
}