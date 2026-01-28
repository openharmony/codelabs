/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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
import relationalStore from '@ohos.data.relationalStore';
import { DietRecord, MealTime } from '../../model/DietModel';
import { RdbManager } from './RdbManager';

interface GeneratedTypeLiteralInterface_1 {
  date: string;
  total: number;
}

interface GeneratedTypeLiteralInterface_2 {
  date: string;
  total: number;
}

export class DietRecordDao {
  private tableName = 'diet_record';

  // 1. 插入一条记录
  async insertRecord(record: DietRecord): Promise<boolean> {
    const store = RdbManager.getInstance().getRdbStore();
    if (!store) return false;

    const valueBucket: relationalStore.ValuesBucket = {
      'id': record.id,
      'foodId': record.foodId,
      'foodName': record.foodName,
      'mealTime': record.mealTime,
      'portion': record.portion,
      'totalCalories': record.totalCalories,
      'record_date': record.date // 映射到数据库字段
    };

    try {
      await store.insert(this.tableName, valueBucket);
      return true;
    } catch (e) {
      console.error(`[DietRecordDao] Insert failed: ${JSON.stringify(e)}`);
      return false;
    }
  }

  // 2. 根据 ID 删除记录
  async deleteRecord(id: string): Promise<boolean> {
    const store = RdbManager.getInstance().getRdbStore();
    if (!store) return false;

    let predicates = new relationalStore.RdbPredicates(this.tableName);
    predicates.equalTo('id', id);

    try {
      await store.delete(predicates);
      return true;
    } catch (e) {
      console.error(`[DietRecordDao] Delete failed: ${JSON.stringify(e)}`);
      return false;
    }
  }

  // 3. 根据日期查询记录 (核心功能)
  async getRecordsByDate(dateStr: string): Promise<DietRecord[]> {
    const store = RdbManager.getInstance().getRdbStore();
    if (!store) return [];

    let predicates = new relationalStore.RdbPredicates(this.tableName);
    predicates.equalTo('record_date', dateStr); // 查询特定日期

    const resultSet = await store.query(predicates);
    const result: DietRecord[] = [];

    // 遍历结果集
    while (resultSet.goToNextRow()) {
      result.push({
        id: resultSet.getString(resultSet.getColumnIndex('id')),
        foodId: resultSet.getLong(resultSet.getColumnIndex('foodId')),
        foodName: resultSet.getString(resultSet.getColumnIndex('foodName')),
        mealTime: resultSet.getString(resultSet.getColumnIndex('mealTime')) as MealTime,
        portion: resultSet.getDouble(resultSet.getColumnIndex('portion')),
        totalCalories: resultSet.getLong(resultSet.getColumnIndex('totalCalories')),
        date: resultSet.getString(resultSet.getColumnIndex('record_date'))
      });
    }

    resultSet.close(); // 记得关闭
    return result;
  }
  // --- 获取过去7天的每日总热量 ---
  async getWeeklyStats(): Promise<GeneratedTypeLiteralInterface_1[]> {
    const store = RdbManager.getInstance().getRdbStore();
    if (!store) return [];

    const result: GeneratedTypeLiteralInterface_2[] = [];

    // 循环获取过去 7 天的日期字符串
    for (let i = 6; i >= 0; i--) {
      const d = new Date();
      d.setDate(d.getDate() - i);
      const dateStr = `${d.getFullYear()}-${(d.getMonth() + 1).toString().padStart(2, '0')}-${d.getDate().toString().padStart(2, '0')}`;

      // 查询那一天的总和
      // 注意：这里为了简化逻辑使用了循环查询，更高效的做法是使用 SQL 的 GROUP BY，但在 RdbPredicates 里比较麻烦
      let predicates = new relationalStore.RdbPredicates(this.tableName);
      predicates.equalTo('record_date', dateStr);

      const resultSet = await store.query(predicates);
      let dailyTotal = 0;
      while (resultSet.goToNextRow()) {
        dailyTotal += resultSet.getLong(resultSet.getColumnIndex('totalCalories'));
      }
      resultSet.close();

      result.push({ date: dateStr, total: dailyTotal });
    }

    return result;
  }
}