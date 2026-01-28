import relationalStore from '@ohos.data.relationalStore';
import { RdbManager } from './RdbManager';

export interface WaterRecord {
  date: string;
  actual: number;
  goal: number;
}

export class WaterDao {
  private tableName = 'water_record';

  async saveWaterRecord(record: WaterRecord): Promise<boolean> {
    const store = RdbManager.getInstance().getRdbStore();
    if (!store) return false;

    const valueBucket: relationalStore.ValuesBucket = {
      'record_date': record.date,
      'actual_water': record.actual,
      'goal_water': record.goal
    };

    try {
      await store.insert(this.tableName, valueBucket, relationalStore.ConflictResolution.ON_CONFLICT_REPLACE);
      return true;
    } catch (e) {
      console.error(`[WaterDao] Save failed: ${JSON.stringify(e)}`);
      return false;
    }
  }

  async getWaterByDate(dateStr: string): Promise<WaterRecord | null> {
    const store = RdbManager.getInstance().getRdbStore();
    if (!store) return null;

    let predicates = new relationalStore.RdbPredicates(this.tableName);
    predicates.equalTo('record_date', dateStr);

    try {
      const resultSet = await store.query(predicates);
      if (resultSet.goToNextRow()) {
        const result: WaterRecord = {
          date: resultSet.getString(resultSet.getColumnIndex('record_date')),
          actual: resultSet.getLong(resultSet.getColumnIndex('actual_water')),
          goal: resultSet.getLong(resultSet.getColumnIndex('goal_water'))
        };
        resultSet.close();
        return result;
      }
      resultSet.close();
    } catch (e) {
      console.error(`[WaterDao] Query failed: ${JSON.stringify(e)}`);
    }
    return null;
  }
  // --- 新增方法：批量查询日期列表 ---
  async getWaterBatch(dateList: string[]): Promise<WaterRecord[]> {
    const results: WaterRecord[] = [];
    for (const date of dateList) {
      const record = await this.getWaterByDate(date);
      if (record) {
        results.push(record);
      } else {
        // 如果当天没数据，补一个 0
        results.push({
          date: date,
          actual: 0,
          goal: 2000
        });
      }
    }
    return results;
  }
}