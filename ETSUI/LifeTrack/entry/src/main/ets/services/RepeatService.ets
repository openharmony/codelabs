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

// services/RepeatService.ts
import { EventInstance, RepeatOption, RepeatRule } from '../model/EventTypes';
import { CalendarEvent } from '../model/EventModel';
import common from '@ohos.app.ability.common';
import { EventDao } from '../dao/EventDao';
import { ReminderService } from './ReminderService';

export class RepeatService {
  private static instance: RepeatService;
  private context: common.Context; // 显式声明字段
  private eventDao: EventDao;
  // 重复选项配置
  public readonly repeatOptions: RepeatOption[] = [
    { label: '一次性活动', value: 'none', type: 'none' },
    { label: '每天', value: 'daily', type: 'daily' },
    { label: '每周', value: 'weekly', type: 'weekly' },
    { label: '每月', value: 'monthly', type: 'monthly' },
    { label: '每年', value: 'yearly', type: 'yearly' }
  ];

  public static getInstance(context: common.Context): RepeatService {
    if (!RepeatService.instance) {
      RepeatService.instance = new RepeatService(context);
    }
    return RepeatService.instance;
  }

  // 构造函数：显式赋值
  constructor(context: common.Context) {
    this.context = context; // 显式赋值
    this.eventDao = new EventDao(context);
  }

  // 处理重复事件
  async handleRepeatingEvent(event: CalendarEvent): Promise<void> {
    if (!event.repeatRule || event.repeatRule.trim() === '') {
      return;
    }

    try {
      const repeatRule: RepeatRule = JSON.parse(event.repeatRule);

      // 先删除旧的重复实例
      await this.cleanupOldInstances(event.id);

      // 生成并保存新的实例
      const instances = this.generateFutureInstances(event, repeatRule);

      if (instances.length > 0) {
        await this.saveEventInstances(event.id, instances);
        console.info(`为事件 ${event.id} 生成了 ${instances.length} 个重复实例`);

        // 为每个实例设置提醒（如果原事件有提醒）
        if (event.reminder >= 0) {
          await this.setRemindersForInstances(event, instances);
        }
      }

      console.log(`为事件 ${event.id} 生成了 ${instances.length} 个重复实例`);
    } catch (error) {
      console.error('处理重复事件失败:', error);
    }
  }

  // 清理旧的重复实例
  private async cleanupOldInstances(originalEventId: string): Promise<void> {
    try {
      // 删除该事件的所有旧实例
      const deleted = await this.eventDao.deleteEventInstancesByOriginalId(originalEventId);
      console.info(`清理了事件 ${originalEventId} 的旧实例`);
    } catch (error) {
      console.error('清理旧实例失败:', error);
    }
  }

  // 生成未来实例
  generateFutureInstances(event: CalendarEvent, repeatRule: RepeatRule): EventInstance[] {
    const instances: EventInstance[] = [];

    // 根据结束条件确定最大实例数
    let maxInstances = 100; // 默认最多100个
    let endDate: number | null = null;

    if (repeatRule.endType === 'count') {
      maxInstances = Math.min(repeatRule.endCount || 100, 100);
    } else if (repeatRule.endType === 'date' && repeatRule.endDate) {
      endDate = repeatRule.endDate;
    }

    let currentTime = event.startTime;
    let count = 0;
    const now = Date.now();

    while (count < maxInstances) {
      // 获取下一次发生时间
      currentTime = this.getNextOccurrence(currentTime, repeatRule);

      // 检查结束条件
      if (endDate && currentTime > endDate) {
        break;
      }

      // 只生成未来一年的实例
      const oneYearLater = now + 365 * 24 * 60 * 60 * 1000;
      if (currentTime > oneYearLater) {
        break;
      }

      // 生成实例ID
      const instanceId = `${event.id}_${currentTime}_${count}`;

      // 创建实例
      instances.push({
        id: instanceId,
        originalId: event.id,
        title: event.title,
        description: event.description || '',
        instanceTime: currentTime,
        completed: false
      });

      count++;

      // 如果当前时间已超过结束日期，停止生成
      if (endDate && currentTime >= endDate) {
        break;
      }

      // 如果是一次性活动，生成一个实例后退出
      if (repeatRule.type === 'none') {
        break;
      }
    }

    return instances;
  }

  // 获取下一次发生的时间
  private getNextOccurrence(currentTime: number, repeatRule: RepeatRule): number {
    const currentDate = new Date(currentTime);

    switch (repeatRule.type) {
      case 'daily':
        currentDate.setDate(currentDate.getDate() + repeatRule.interval);
        break;
      case 'weekly':
        currentDate.setDate(currentDate.getDate() + (7 * repeatRule.interval));
        break;
      case 'monthly':
        currentDate.setMonth(currentDate.getMonth() + repeatRule.interval);
        break;
      case 'yearly':
        currentDate.setFullYear(currentDate.getFullYear() + repeatRule.interval);
        break;
      case 'none':
      default:
      // 不增加时间，返回原时间
        break;
    }

    return currentDate.getTime();
  }

  // 保存事件实例（这里需要根据你的数据库实现来完善）
  private async saveEventInstances(originalEventId: string, instances: EventInstance[]): Promise<void> {
    if (instances.length === 0) {
      return;
    }

    try {
      // 使用 EventDao 的批量保存方法
      const success = await this.eventDao.saveEventInstances(instances);

      if (success) {
        console.info(`成功保存 ${instances.length} 个事件实例`);
      } else {
        console.error('保存事件实例失败');
      }
    } catch (error) {
      console.error('保存事件实例时发生错误:', error);
    }
    console.log(`需要保存 ${instances.length} 个事件实例`);
  }

  // 为实例设置提醒
  private async setRemindersForInstances(originalEvent: CalendarEvent, instances: EventInstance[]): Promise<void> {
    try {
      const reminderService = ReminderService.getInstance();

      for (const instance of instances) {
        // 计算实例的结束时间（保持相同的时长）
        const duration = originalEvent.endTime - originalEvent.startTime;

        // 为每个实例创建类似的事件对象用于提醒
        const instanceEvent: CalendarEvent = {
          id: instance.id,
          title: originalEvent.title,
          description: originalEvent.description || '',
          startTime: instance.instanceTime,
          endTime: instance.instanceTime + duration,
          allDay: originalEvent.allDay,
          location: originalEvent.location || '',
          calendarId: originalEvent.calendarId || 'default',
          color: originalEvent.color || '#2196F3',
          reminder: originalEvent.reminder,
          repeatRule: '', // 实例没有重复规则
          completed: originalEvent.completed || false,
          created: originalEvent.created || Date.now(),
          updated: originalEvent.updated || Date.now()
        };

        // 设置提醒
        await reminderService.setReminder(instanceEvent);
      }

      console.info(`为 ${instances.length} 个实例设置了提醒`);
    } catch (error) {
      console.error('为实例设置提醒失败:', error);
    }
  }

  // 获取某个日期范围内的重复实例
  async getRepeatedEventsInRange(startDate: number, endDate: number): Promise<CalendarEvent[]> {
    try {
      // 获取该时间范围内的实例
      const instances = await this.eventDao.getEventInstancesByDateRange(startDate, endDate);

      // 获取对应的原始事件信息
      const events: CalendarEvent[] = [];
      const processedOriginalIds = new Set<string>();

      for (const instance of instances) {
        // 如果还未获取过这个原始事件
        if (!processedOriginalIds.has(instance.originalId)) {
          const originalEvent = await this.eventDao.getEventById(instance.originalId);
          if (originalEvent) {
            events.push(originalEvent);
            processedOriginalIds.add(instance.originalId);
          }
        }
      }

      return events;
    } catch (error) {
      console.error('获取重复事件失败:', error);
      return [];
    }
  }

  // 获取带实例的事件列表
  async getEventsWithInstances(startDate: number, endDate: number): Promise<CalendarEvent[]> {
    try {
      // 获取普通事件
      const normalEvents = await this.eventDao.getEventsByDateRange(startDate, endDate);

      // 获取重复事件的实例
      const repeatedEvents = await this.getRepeatedEventsInRange(startDate, endDate);

      // 合并结果，去重
      const eventMap = new Map<string, CalendarEvent>();

      normalEvents.forEach(event => eventMap.set(event.id, event));
      repeatedEvents.forEach(event => {
        if (!eventMap.has(event.id)) {
          eventMap.set(event.id, event);
        }
      });

      return Array.from(eventMap.values());
    } catch (error) {
      console.error('获取带实例的事件失败:', error);
      return [];
    }
  }

  // 当原始事件被删除时，清理所有相关实例
  async cleanupOnEventDelete(eventId: string): Promise<void> {
    try {
      await this.eventDao.deleteEventInstancesByOriginalId(eventId);
      console.info(`已清理事件 ${eventId} 的所有重复实例`);
    } catch (error) {
      console.error('清理重复实例失败:', error);
    }
  }

  // 获取重复选项
  getRepeatOptions(): RepeatOption[] {
    return this.repeatOptions;
  }

  // 根据规则获取索引
  getRepeatIndexByRule(ruleString: string): number {
    if (!ruleString || ruleString.trim() === '') {
      return 0; // 对应"一次性活动"
    }

    try {
      const rule: RepeatRule = JSON.parse(ruleString);
      const index = this.repeatOptions.findIndex(option => option.type === rule.type);
      return index >= 0 ? index : 0;
    } catch (error) {
      console.error('解析重复规则失败:', error);
      return 0;
    }
  }

  // 根据索引获取规则字符串
  getRepeatRuleByIndex(index: number): string {
    if (index < 0 || index >= this.repeatOptions.length) {
      return '';
    }

    const option = this.repeatOptions[index];
    if (option.type === 'none') {
      return '';
    }

    const rule: RepeatRule = {
      type: option.type,
      interval: 1,
      endType: 'never'
    };

    return JSON.stringify(rule);
  }
}