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

// DataManager.ets
import { EventDao } from '../dao/EventDao';
import { CalendarEvent } from '../model/EventModel';
import { ReminderService } from '../services/ReminderService';
import { RepeatService } from '../services/RepeatService';
import common from '@ohos.app.ability.common';
import { EventInstance } from '../model/EventTypes';

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
  private reminderService: ReminderService;
  private repeatService: RepeatService;
  private static instance: DataManager | null = null;

  private constructor(context: common.Context) {
    this.eventDao = new EventDao(context);
    this.reminderService = ReminderService.getInstance();
    this.repeatService = RepeatService.getInstance(context);
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

      // 2. 处理重复规则
      let repeatRuleString = '';

      if (eventData.repeatRule !== undefined && eventData.repeatRule !== null) {
        if (typeof eventData.repeatRule === 'string') {
          // 如果是字符串，直接使用（前端应该保证格式正确）
          repeatRuleString = eventData.repeatRule;
        } else if (typeof eventData.repeatRule === 'number') {
          // 如果传入的是索引，转换为规则字符串
          repeatRuleString = this.repeatService.getRepeatRuleByIndex(eventData.repeatRule);
        }
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
        reminder: eventData.reminder !== undefined ? eventData.reminder : -1,
        repeatRule: repeatRuleString,
        completed: eventData.completed || false,
        created: eventData.id && eventData.created ? eventData.created : now,
        updated: now
      };

      // 保存到数据库
      const savedId = await this.eventDao.saveEvent(event);

      //5. 设置提醒（通过ReminderService）
      if (event.reminder >= 0) {
        await this.reminderService.setReminder(event);
      }

      // 6. 处理重复事件（通过RepeatService）
      if (repeatRuleString.trim() !== '') {
        try {
          // 直接调用，让 RepeatService 内部处理可能的解析错误
          await this.repeatService.handleRepeatingEvent(event);
        } catch (error) {
          console.error('处理重复事件失败:', error);
        }
      }

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

      // 先取消主事件提醒
      await this.reminderService.cancelReminder(id);

      // 取消所有实例提醒
      await this.deleteEventInstanceReminders(id);

      // 清理重复实例
      await this.repeatService.cleanupOnEventDelete(id);

      // 删除主事件
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

  // 更新日程
  async updateEvent(eventId: string, eventData: Partial<CalendarEvent>): Promise<SaveResult> {
    try {
      // 1. 获取原事件
      const existingEvent = await this.eventDao.getEventById(eventId);
      if (!existingEvent) {
        return { success: false, error: '事件不存在' };
      }

      // 2. 合并数据
      const updatedEvent: CalendarEvent = {
        id: eventId,
        title: eventData.title ?? existingEvent.title,
        description: eventData.description ?? existingEvent.description,
        startTime: eventData.startTime ?? existingEvent.startTime,
        endTime: eventData.endTime ?? existingEvent.endTime,
        allDay: eventData.allDay ?? existingEvent.allDay,
        location: eventData.location ?? existingEvent.location,
        calendarId: eventData.calendarId ?? existingEvent.calendarId,
        color: eventData.color ?? existingEvent.color,
        reminder: eventData.reminder !== undefined ? eventData.reminder : existingEvent.reminder,
        repeatRule: eventData.repeatRule ?? existingEvent.repeatRule,
        completed: eventData.completed ?? existingEvent.completed,
        created: existingEvent.created,
        updated: Date.now()
      };

      // 3. 保存到数据库
      const savedId = await this.eventDao.saveEvent(updatedEvent);

      // 4. 更新提醒
      if (updatedEvent.reminder >= 0) {
        // 取消旧提醒
        await this.reminderService.cancelReminder(eventId);

        // 设置新提醒
        await this.reminderService.setReminder(updatedEvent);
      } else {
        // 取消提醒
        await this.reminderService.cancelReminder(eventId);
      }

      // 5. 检查是否需要更新重复实例
      const repeatRuleChanged = eventData.repeatRule !== undefined;
      const timeChanged = eventData.startTime !== undefined || eventData.endTime !== undefined;
      const reminderChanged = eventData.reminder !== undefined;

      if (repeatRuleChanged || timeChanged || reminderChanged) {
        // 重新生成实例和提醒
        await this.updateEventInstanceReminders(updatedEvent);
      }

      return { success: true, id: savedId };

    } catch (error) {
      console.error('Update event failed:', error);
      return {
        success: false,
        error: `更新失败: ${error instanceof Error ? error.message : '未知错误'}`
      };
    }
  }

  //删除事件的所有实例提醒
  async deleteEventInstanceReminders(eventId: string): Promise<boolean> {
    try {
      if (!eventId) {
        return false;
      }

      // 获取该事件的所有实例
      const instances = await this.eventDao.getEventInstancesByOriginalId(eventId);

      if (instances.length === 0) {
        return true; // 没有实例，直接返回成功
      }

      // 取消每个实例的提醒
      const reminderService = ReminderService.getInstance();
      for (const instance of instances) {
        await reminderService.cancelReminder(instance.id);
      }

      console.info(`删除了事件 ${eventId} 的 ${instances.length} 个实例提醒`);
      return true;

    } catch (error) {
      console.error('删除实例提醒失败:', error);
      return false;
    }
  }

  //更新事件实例提醒
  async updateEventInstanceReminders(event: CalendarEvent): Promise<boolean> {
    try {
      if (!event.id || event.reminder < 0) {
        return false;
      }

      // 1. 先删除所有旧实例提醒
      await this.deleteEventInstanceReminders(event.id);

      // 2. 重新生成实例并设置提醒
      if (event.repeatRule && event.repeatRule.trim() !== '') {
        // 让 RepeatService 重新处理重复事件
        await this.repeatService.handleRepeatingEvent(event);
      }

      return true;

    } catch (error) {
      console.error('更新实例提醒失败:', error);
      return false;
    }
  }

  // 获取提醒选项（代理给ReminderService）
  getReminderOptions() {
    return this.reminderService.getReminderOptions();
  }

  // 获取重复选项（代理给RepeatService）
  getRepeatOptions() {
    return this.repeatService.getRepeatOptions();
  }

  // 根据分钟数获取提醒索引（代理给ReminderService）
  getReminderIndexByMinutes(minutes: number): number {
    return this.reminderService.getReminderIndexByMinutes(minutes);
  }

  // 根据规则获取重复索引（代理给RepeatService）
  getRepeatIndexByRule(ruleString: string): number {
    return this.repeatService.getRepeatIndexByRule(ruleString);
  }

  // 根据索引获取提醒分钟数（代理给ReminderService）
  getReminderMinutesByIndex(index: number): number {
    return this.reminderService.getReminderMinutesByIndex(index);
  }

  // 根据索引获取重复规则（代理给RepeatService）
  getRepeatRuleByIndex(index: number): string {
    return this.repeatService.getRepeatRuleByIndex(index);
  }

  // 标记日程完成/未完成
  async toggleEventCompletion(id: string, completed: boolean): Promise<ToggleResult> {
    try {
      if (!id) {
        return { success: false, error: '无效的日程ID' };
      }

      // 判断是主事件还是实例
      const isInstance = id.includes('_'); // 实例ID格式：originalId_timestamp_index

      if (isInstance) {
        // 更新实例完成状态
        const success = await this.eventDao.updateEventInstanceCompletion(id, completed);

        if (!success) {
          return { success: false, error: '更新实例状态失败' };
        }

        return { success: true };
      } else {
        // 更新主事件完成状态
        const updated = await this.eventDao.updateEventCompletion(id, completed);

        if (!updated) {
          return { success: false, error: '更新失败，日程可能不存在' };
        }

        return { success: true };
      }
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

  // 根据原始事件ID获取事件实例
  async getEventInstancesByOriginalId(originalId: string): Promise<EventInstance[]> {
    try {
      return await this.eventDao.getEventInstancesByOriginalId(originalId);
    } catch (error) {
      console.error('获取事件实例失败:', error);
      return [];
    }
  }

  // 更新事件实例完成状态
  async updateEventInstanceCompletion(instanceId: string, completed: boolean): Promise<boolean> {
    try {
      return await this.eventDao.updateEventInstanceCompletion(instanceId, completed);
    } catch (error) {
      console.error('更新事件实例完成状态失败:', error);
      return false;
    }
  }

  // 删除事件实例
  async deleteEventInstance(instanceId: string): Promise<boolean> {
    try {
      return await this.eventDao.deleteEventInstance(instanceId);
    } catch (error) {
      console.error('删除事件实例失败:', error);
      return false;
    }
  }
}

