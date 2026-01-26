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

// StepDao.ets - 步数记录数据访问对象
import relationalStore from '@ohos.data.relationalStore';
import common from '@ohos.app.ability.common';
import { StepRecord } from '../model/StepModel';
import { DATABASE_CONFIG, SQL_CREATE_INDEXES, SQL_CREATE_STEP_INDEXES, SQL_CREATE_STEP_TABLE } from './DatabaseConfig';
import util from '@ohos.util';

export class StepDao {
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

      // 创建步数记录表
      await this.rdbStore.executeSql(SQL_CREATE_STEP_TABLE);

      // 创建索引
      for (const sql of SQL_CREATE_INDEXES) {
        await this.rdbStore.executeSql(sql);
      }
      
      // 为步数记录表创建索引
      for (const sql of SQL_CREATE_STEP_INDEXES) {
        await this.rdbStore.executeSql(sql);
      }

      console.log('Step database initialized successfully');
      return this.rdbStore;
    } catch (err) {
      console.error(`Failed to init step database: ${JSON.stringify(err)}`);
      throw new Error(`Failed to init step database: ${JSON.stringify(err)}`);
    }
  }

  // 将StepRecord对象转换为数据库值对象
  private stepToValueBucket(step: StepRecord): relationalStore.ValuesBucket {
    return {
      id: step.id,
      date: step.date,
      steps: step.steps,
      calories: step.calories,
      distance: step.distance,
      created_time: step.created,
      updated_time: step.updated
    };
  }

  // 将数据库行转换为StepRecord对象
  private rowToStep(resultSet: relationalStore.ResultSet): StepRecord {
    return {
      id: resultSet.getString(resultSet.getColumnIndex('id')),
      date: resultSet.getString(resultSet.getColumnIndex('date')),
      steps: resultSet.getLong(resultSet.getColumnIndex('steps')),
      calories: resultSet.getDouble(resultSet.getColumnIndex('calories')),
      distance: resultSet.getDouble(resultSet.getColumnIndex('distance')),
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

  // 保存或更新步数记录（主键存在则更新，不存在则插入）
  async saveStepRecord(step: StepRecord): Promise<string> {
    const rdbStore = await this.initDatabase();

    try {
      await rdbStore.beginTransaction();

      // 1. 确定最终的业务ID：优先使用传入的ID，否则生成新的UUID
      let finalId: string;
      if (step.id && this.isValidUUID(step.id)) {
        finalId = step.id;
      } else {
        finalId = util.generateRandomUUID(true);
        step.id = finalId; // 同步到step对象
      }

      // 2. 准备要写入数据库的数据
      const valueBucket = this.stepToValueBucket(step);
      // 确保valueBucket中包含确定的ID
      valueBucket['id'] = finalId;

      let result: string = finalId; // 最终返回的就是这个业务ID

      // 3. 尝试更新现有记录（如果ID有效且存在）
      const predicates = new relationalStore.RdbPredicates('step_records');
      predicates.equalTo('id', finalId);

      const updated = await rdbStore.update(valueBucket, predicates);

      // 4. 如果更新影响0行，说明记录不存在，执行插入
      if (updated === 0) {
        console.log(`No existing step record with id: ${finalId}, performing insert`);
        // 对于唯一约束的日期字段，我们也需要尝试按日期更新
        const datePredicates = new relationalStore.RdbPredicates('step_records');
        datePredicates.equalTo('date', step.date);

        // 检查是否已经存在当天的记录
        const dateResultSet = await rdbStore.query(
          datePredicates,
          ['id']
        );

        if (await dateResultSet.goToFirstRow()) {
          // 如果存在当天的记录，则更新它而不是插入
          const existingId = dateResultSet.getString(dateResultSet.getColumnIndex('id'));
          dateResultSet.close();
          
          // 更新现有日期记录
          const updatePredicates = new relationalStore.RdbPredicates('step_records');
          updatePredicates.equalTo('date', step.date);
          
          valueBucket['id'] = existingId; // 使用现有ID
          const updatedRows = await rdbStore.update(valueBucket, updatePredicates);
          result = existingId;
          console.log(`Updated existing step record for date: ${step.date}, affected rows: ${updatedRows}`);
        } else {
          // 插入新记录
          dateResultSet.close();
          const insertedRowId = await rdbStore.insert('step_records', valueBucket);
          console.log(`Inserted step record with rowid: ${insertedRowId}`);
        }
      } else {
        console.log(`Updated existing step record with id: ${finalId}, affected rows: ${updated}`);
      }

      await rdbStore.commit();
      console.log(`Step record saved successfully, id: ${result}`);
      return result;

    } catch (err) {
      await rdbStore.rollback(0);
      console.error(`Failed to save step record: ${JSON.stringify(err)}`);
      throw new Error(`Failed to save step record: ${JSON.stringify(err)}`);
    }
  }

  // 根据ID查询单个步数记录
  async getStepRecordById(id: string): Promise<StepRecord | null> {
    const rdbStore = await this.initDatabase();

    try {
      const predicates = new relationalStore.RdbPredicates('step_records');
      predicates.equalTo('id', id);

      const resultSet = await rdbStore.query(
        predicates,
        ['id', 'date', 'steps', 'calories', 'distance', 'created_time', 'updated_time']
      );

      if (await resultSet.goToFirstRow()) {
        const step = this.rowToStep(resultSet);
        resultSet.close();
        return step;
      }

      resultSet.close();
      return null;

    } catch (err) {
      console.error(`Failed to get step record by id ${id}: ${JSON.stringify(err)}`);
      return null;
    }
  }

  // 根据日期查询步数记录
  async getStepRecordByDate(date: string): Promise<StepRecord | null> {
    const rdbStore = await this.initDatabase();

    try {
      const predicates = new relationalStore.RdbPredicates('step_records');
      predicates.equalTo('date', date);

      const resultSet = await rdbStore.query(
        predicates,
        ['id', 'date', 'steps', 'calories', 'distance', 'created_time', 'updated_time']
      );

      if (await resultSet.goToFirstRow()) {
        const step = this.rowToStep(resultSet);
        resultSet.close();
        return step;
      }

      resultSet.close();
      return null;

    } catch (err) {
      console.error(`Failed to get step record by date ${date}: ${JSON.stringify(err)}`);
      return null;
    }
  }

  // 获取指定日期范围内的步数记录
  async getStepRecordsByDateRange(startDate: string, endDate: string): Promise<StepRecord[]> {
    const rdbStore = await this.initDatabase();
    const records: StepRecord[] = [];

    try {
      const predicates = new relationalStore.RdbPredicates('step_records');
      predicates.greaterThanOrEqualTo('date', startDate);
      predicates.lessThanOrEqualTo('date', endDate);
      predicates.orderByAsc('date'); // 按日期升序排列

      const resultSet = await rdbStore.query(
        predicates,
        ['id', 'date', 'steps', 'calories', 'distance', 'created_time', 'updated_time']
      );

      while (await resultSet.goToNextRow()) {
        records.push(this.rowToStep(resultSet));
      }

      resultSet.close();
      return records;

    } catch (err) {
      console.error(`Failed to get step records by date range: ${JSON.stringify(err)}`);
      return [];
    }
  }

  // 获取最近的N条步数记录
  async getRecentStepRecords(limit: number = 7): Promise<StepRecord[]> {
    const rdbStore = await this.initDatabase();
    const records: StepRecord[] = [];

    try {
      const predicates = new relationalStore.RdbPredicates('step_records');
      predicates.orderByDesc('date'); // 按日期降序排列，最新的在前

      const resultSet = await rdbStore.query(
        predicates,
        ['id', 'date', 'steps', 'calories', 'distance', 'created_time', 'updated_time']
      );

      let count = 0;
      while (await resultSet.goToNextRow() && count < limit) {
        records.push(this.rowToStep(resultSet));
        count++;
      }

      resultSet.close();
      return records;

    } catch (err) {
      console.error(`Failed to get recent step records: ${JSON.stringify(err)}`);
      return [];
    }
  }

  // 删除指定日期的步数记录
  async deleteStepRecordByDate(date: string): Promise<boolean> {
    const rdbStore = await this.initDatabase();

    try {
      const predicates = new relationalStore.RdbPredicates('step_records');
      predicates.equalTo('date', date);

      const deletedRows = await rdbStore.delete(predicates);
      return deletedRows > 0;

    } catch (err) {
      console.error(`Failed to delete step record by date ${date}: ${JSON.stringify(err)}`);
      return false;
    }
  }

  // 获取所有步数记录
  async getAllStepRecords(): Promise<StepRecord[]> {
    const rdbStore = await this.initDatabase();
    const records: StepRecord[] = [];

    try {
      const predicates = new relationalStore.RdbPredicates('step_records');
      predicates.orderByDesc('date'); // 按日期降序排列

      const resultSet = await rdbStore.query(
        predicates,
        ['id', 'date', 'steps', 'calories', 'distance', 'created_time', 'updated_time']
      );

      while (await resultSet.goToNextRow()) {
        records.push(this.rowToStep(resultSet));
      }

      resultSet.close();
      return records;

    } catch (err) {
      console.error(`Failed to get all step records: ${JSON.stringify(err)}`);
      return [];
    }
  }
}