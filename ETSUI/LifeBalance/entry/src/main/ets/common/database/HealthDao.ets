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
import { RdbManager } from './RdbManager';

export interface HealthRecord {
  date: string;
  height: number;
  weight: number;
  age: number;
  bmi: number;
  targetCalories: number;
  targetWater: number;
  targetExercise: number;
}

export class HealthDao {
  private tableName = 'health_data';

  async saveHealthRecord(record: HealthRecord): Promise<void> {
    const store = RdbManager.getInstance().getRdbStore();
    if (!store) return;

    const valueBucket: relationalStore.ValuesBucket = {
      'record_date': record.date,
      'height': record.height,
      'weight': record.weight,
      'age': record.age,
      'bmi': record.bmi,
      'target_calories': record.targetCalories,
      'target_water': record.targetWater,
      'target_exercise': record.targetExercise
    };

    await store.insert(this.tableName, valueBucket, relationalStore.ConflictResolution.ON_CONFLICT_REPLACE);
  }

  async getLatestRecord(): Promise<HealthRecord | null> {
    const store = RdbManager.getInstance().getRdbStore();
    if (!store) return null;

    let predicates = new relationalStore.RdbPredicates(this.tableName);
    // 关键：按日期倒序排列，确保拿到的是最新存入的那一条
    predicates.orderByDesc('record_date');

    const resultSet = await store.query(predicates);

    // 检查是否有数据
    if (resultSet.rowCount > 0 && resultSet.goToNextRow()) {
      const record: HealthRecord = {
        date: resultSet.getString(resultSet.getColumnIndex('record_date')),
        height: resultSet.getDouble(resultSet.getColumnIndex('height')),
        weight: resultSet.getDouble(resultSet.getColumnIndex('weight')),
        age: resultSet.getLong(resultSet.getColumnIndex('age')),
        bmi: resultSet.getDouble(resultSet.getColumnIndex('bmi')),
        targetCalories: resultSet.getLong(resultSet.getColumnIndex('target_calories')),
        targetWater: resultSet.getLong(resultSet.getColumnIndex('target_water')),
        targetExercise: resultSet.getLong(resultSet.getColumnIndex('target_exercise'))
      };
      resultSet.close();
      return record;
    }

    resultSet.close();
    return null;
  }

  async getHealthByDate(dateStr: string): Promise<HealthRecord | null> {
    const store = RdbManager.getInstance().getRdbStore();
    if (!store) return null;

    let predicates = new relationalStore.RdbPredicates(this.tableName);
    predicates.equalTo('record_date', dateStr);

    try {
      const resultSet = await store.query(predicates);

      if (resultSet.goToNextRow()) {
        const result: HealthRecord = {
          date: resultSet.getString(resultSet.getColumnIndex('record_date')),
          height: resultSet.getDouble(resultSet.getColumnIndex('height')),
          weight: resultSet.getDouble(resultSet.getColumnIndex('weight')),
          age: resultSet.getLong(resultSet.getColumnIndex('age')),
          bmi: resultSet.getDouble(resultSet.getColumnIndex('bmi')),
          targetCalories: resultSet.getLong(resultSet.getColumnIndex('target_calories')),
          targetWater: resultSet.getLong(resultSet.getColumnIndex('target_water')),
          targetExercise: resultSet.getLong(resultSet.getColumnIndex('target_exercise'))
        };

        resultSet.close();
        return result;
      }
      resultSet.close();
    } catch (err) {
      console.error(`Query health by date failed: ${JSON.stringify(err)}`);
    }

    return null;
  }
}