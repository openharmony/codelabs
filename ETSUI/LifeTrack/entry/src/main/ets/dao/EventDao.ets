// EventDao.ets
import relationalStore from '@ohos.data.relationalStore';
import common from '@ohos.app.ability.common';
import { CalendarEvent } from '../model/EventModel';
import { DATABASE_CONFIG, SQL_CREATE_EVENTS_TABLE, SQL_CREATE_INDEXES } from './DatabaseConfig';
import util from '@ohos.util';

export class EventDao {
  private context: common.Context;
  private rdbStore: relationalStore.RdbStore | null = null;

  constructor(context: common.Context) {
    this.context = context;
  }

  // 初始化数据库
  private async initDatabase(): Promise<relationalStore.RdbStore> {
    if (this.rdbStore) {
      return this.rdbStore;
    }

    try {
      this.rdbStore = await relationalStore.getRdbStore(this.context, DATABASE_CONFIG);

      // 创建表
      await this.rdbStore.executeSql(SQL_CREATE_EVENTS_TABLE);

      // 创建索引
      for (const sql of SQL_CREATE_INDEXES) {
        await this.rdbStore.executeSql(sql);
      }

      console.log('Database initialized successfully');
      return this.rdbStore;
    } catch (err) {
      console.error(`Failed to init database: ${JSON.stringify(err)}`);
      throw new Error(`Failed to init database: ${JSON.stringify(err)}`);
    }
  }

  // 将Event对象转换为数据库值对象
  private eventToValueBucket(event: CalendarEvent): relationalStore.ValuesBucket {
    return {
      id: event.id,
      title: event.title,
      description: event.description || '',
      start_time: event.startTime,
      end_time: event.endTime,
      all_day: event.allDay ? 1 : 0,
      location: event.location || '',
      calendar_id: event.calendarId || 'default',
      color: event.color || '#2196F3',
      reminder: event.reminder || 0,
      repeat_rule: event.repeatRule || '',
      completed: event.completed ? 1 : 0,
      created_time: event.created || Date.now(),
      updated_time: event.updated || Date.now()
    };
  }

  // 将数据库行转换为Event对象
  private rowToEvent(resultSet: relationalStore.ResultSet): CalendarEvent {
    return {
      id: resultSet.getString(resultSet.getColumnIndex('id')),
      title: resultSet.getString(resultSet.getColumnIndex('title')),
      description: resultSet.getString(resultSet.getColumnIndex('description')),
      startTime: resultSet.getLong(resultSet.getColumnIndex('start_time')),
      endTime: resultSet.getLong(resultSet.getColumnIndex('end_time')),
      allDay: resultSet.getLong(resultSet.getColumnIndex('all_day')) === 1,
      location: resultSet.getString(resultSet.getColumnIndex('location')),
      calendarId: resultSet.getString(resultSet.getColumnIndex('calendar_id')),
      color: resultSet.getString(resultSet.getColumnIndex('color')),
      reminder: resultSet.getLong(resultSet.getColumnIndex('reminder')),
      repeatRule: resultSet.getString(resultSet.getColumnIndex('repeat_rule')),
      completed: resultSet.getLong(resultSet.getColumnIndex('completed')) === 1,
      created: resultSet.getLong(resultSet.getColumnIndex('created_time')),
      updated: resultSet.getLong(resultSet.getColumnIndex('updated_time'))
    };
  }

  // UUID格式验证函数
  private isValidUUID(uuid: string): boolean {
    if (!uuid || typeof uuid !== 'string') {
      return false;
    }
    // RFC 4122格式：8-4-4-4-12的十六进制数字，支持大小写
    const regex = /^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$/i;
    return regex.test(uuid);
  }

  // 保存或更新日程（主键存在则更新，不存在则插入）
  async saveEvent(event: CalendarEvent): Promise<string> {
    const rdbStore = await this.initDatabase();

    try {
      await rdbStore.beginTransaction();

      // 1. 确定最终的业务ID：优先使用传入的ID，否则生成新的UUID
      let finalId: string;
      if (event.id && this.isValidUUID(event.id)) {
        finalId = event.id;
      } else {
        finalId = util.generateRandomUUID(true);
        event.id = finalId; // 同步到event对象
      }

      // 2. 准备要写入数据库的数据
      const valueBucket = this.eventToValueBucket(event);
      // 确保valueBucket中包含确定的ID
      valueBucket['id'] = finalId;

      let result: string = finalId; // 最终返回的就是这个业务ID

      // 3. 尝试更新现有记录（如果ID有效且存在）
      const predicates = new relationalStore.RdbPredicates('calendar_events');
      predicates.equalTo('id', finalId);

      const updated = await rdbStore.update(valueBucket, predicates);

      // 4. 如果更新影响0行，说明记录不存在，执行插入
      if (updated === 0) {
        console.log(`No existing event with id: ${finalId}, performing insert`);
        const insertedRowId = await rdbStore.insert('calendar_events', valueBucket);
        console.log(`Inserted with rowid: ${insertedRowId}`);
      } else {
        console.log(`Updated existing event with id: ${finalId}, affected rows: ${updated}`);
      }

      await rdbStore.commit();
      console.log(`Event saved successfully, id: ${result}`);
      return result;

    } catch (err) {
      await rdbStore.rollback(0);
      console.error(`Failed to save event: ${JSON.stringify(err)}`);
      throw new Error(`Failed to save event: ${JSON.stringify(err)}`);
    }
  }

  // 根据ID查询单个日程
  async getEventById(id: string): Promise<CalendarEvent | null> {
    const rdbStore = await this.initDatabase();

    try {
      const predicates = new relationalStore.RdbPredicates('calendar_events');
      predicates.equalTo('id', id);

      const resultSet = await rdbStore.query(
        predicates,
        ['id', 'title', 'description', 'start_time', 'end_time', 'all_day',
          'location', 'calendar_id', 'color', 'reminder', 'repeat_rule',
          'completed', 'created_time', 'updated_time']
      );

      if (await resultSet.goToFirstRow()) {
        const event = this.rowToEvent(resultSet);
        resultSet.close();
        return event;
      }

      resultSet.close();
      return null;

    } catch (err) {
      console.error(`Failed to get event by id ${id}: ${JSON.stringify(err)}`);
      return null;
    }
  }

  // 批量查询日程（例如按日期范围）
  async getEventsByDateRange(startDate: number, endDate: number): Promise<CalendarEvent[]> {
    const rdbStore = await this.initDatabase();
    const events: CalendarEvent[] = [];

    try {
      const predicates = new relationalStore.RdbPredicates('calendar_events');
      predicates.greaterThanOrEqualTo('start_time', startDate);
      predicates.lessThanOrEqualTo('end_time', endDate);
      predicates.orderByAsc('start_time');

      const resultSet = await rdbStore.query(
        predicates,
        ['id', 'title', 'description', 'start_time', 'end_time', 'all_day',
          'location', 'calendar_id', 'color', 'reminder', 'repeat_rule',
          'completed', 'created_time', 'updated_time']
      );

      while (await resultSet.goToNextRow()) {
        events.push(this.rowToEvent(resultSet));
      }

      resultSet.close();
      return events;

    } catch (err) {
      console.error(`Failed to get events by date range: ${JSON.stringify(err)}`);
      return [];
    }
  }

  // 删除日程
  async deleteEvent(id: string): Promise<boolean> {
    const rdbStore = await this.initDatabase();

    try {
      const predicates = new relationalStore.RdbPredicates('calendar_events');
      predicates.equalTo('id', id);

      const deletedRows = await rdbStore.delete(predicates);
      return deletedRows > 0;

    } catch (err) {
      console.error(`Failed to delete event ${id}: ${JSON.stringify(err)}`);
      return false;
    }
  }

  // 获取所有日程
  async getAllEvents(): Promise<CalendarEvent[]> {
    const rdbStore = await this.initDatabase();
    const events: CalendarEvent[] = [];

    try {
      const predicates = new relationalStore.RdbPredicates('calendar_events');
      predicates.orderByDesc('start_time');

      const resultSet = await rdbStore.query(
        predicates,
        ['id', 'title', 'description', 'start_time', 'end_time', 'all_day',
          'location', 'calendar_id', 'color', 'reminder', 'repeat_rule',
          'completed', 'created_time', 'updated_time']
      );

      while (await resultSet.goToNextRow()) {
        events.push(this.rowToEvent(resultSet));
      }

      resultSet.close();
      return events;

    } catch (err) {
      console.error(`Failed to get all events: ${JSON.stringify(err)}`);
      return [];
    }
  }

  // 更新日程完成状态
  async updateEventCompletion(id: string, completed: boolean): Promise<boolean> {
    const rdbStore = await this.initDatabase();

    try {
      await rdbStore.beginTransaction();

      const valueBucket: relationalStore.ValuesBucket = {
        completed: completed ? 1 : 0,
        updated_time: Date.now()
      };

      const predicates = new relationalStore.RdbPredicates('calendar_events');
      predicates.equalTo('id', id);

      const updated = await rdbStore.update(valueBucket, predicates);
      await rdbStore.commit();

      return updated > 0;

    } catch (err) {
      await rdbStore.rollback(0);
      console.error(`Failed to update event completion: ${JSON.stringify(err)}`);
      return false;
    }
  }
}