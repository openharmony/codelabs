// src/main/ets/viewmodel/EventViewModel.ets
import { Event, EventModel } from '../model/EventModel';
import { DateUtils } from '../common/utils/DateUtils';
import { Logger } from '../common/utils/Logger';

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
    // 检查是否是过去的时间 (可选逻辑)
    // const now = Date.now();
    // if (end < now) {
    //   this.logger.warn('Validate failed: End time is in the past');
    //   return false;
    // }
    return true;
  }

  /**
   * 检查是否与现有日程冲突
   * 逻辑：查询库中所有日程，判断时间段是否有重叠
   * 重叠公式：Max(start1, start2) < Min(end1, end2)
   */
  public async checkConflict(startTime: number, endTime: number, excludeId?: number): Promise<boolean> {
    this.logger.info('Checking for event conflicts...');

    try {
      // 获取即将到来的日程 (为了性能，实际应该根据日期范围查)
      const allEvents = await EventModel.queryUpcoming();

      for (const event of allEvents) {
        // 如果是编辑模式，排除自己
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
   * 获取日程的状态描述文本
   * @param event
   */
  public getEventStatusText(event: Event): string {
    const now = Date.now();

    if (now > event.endTime) {
      return '已结束';
    } else if (now >= event.startTime && now <= event.endTime) {
      return '进行中';
    } else {
      // 计算还有多久开始
      const diff = event.startTime - now;
      const hours = Math.floor(diff / (3600 * 1000));

      if (hours > 48) {
        const days = Math.floor(hours / 24);
        return `${days}天后`;
      } else if (hours > 24) {
        return '明天';
      } else if (hours > 0) {
        return `${hours}小时后`;
      } else {
        const mins = Math.floor(diff / (60 * 1000));
        return `${Math.max(1, mins)}分钟后`;
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
   * 适用于列表页的吸顶分组显示
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