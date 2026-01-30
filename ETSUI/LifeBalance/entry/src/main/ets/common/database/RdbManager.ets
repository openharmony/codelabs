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
import common from '@ohos.app.ability.common';

export class RdbManager {
  private static instance: RdbManager;
  private rdbStore: relationalStore.RdbStore | null = null;
  private tableName: string = 'diet_record';
  // 建表 SQL 语句
  // 注意：date 字段我们改名叫 record_date 以免和 SQL 关键字冲突
  private sqlCreateTable = `CREATE TABLE IF NOT EXISTS ${this.tableName} (
    id TEXT PRIMARY KEY,
    foodId INTEGER,
    foodName TEXT,
    mealTime TEXT,
    portion REAL,
    totalCalories INTEGER,
    record_date TEXT
  )`;
  private sqlCreateWaterTable = `CREATE TABLE IF NOT EXISTS water_record (
    record_date TEXT PRIMARY KEY,
    actual_water INTEGER,
    goal_water INTEGER
  )`;
  private sqlCreateExerciseTable = `CREATE TABLE IF NOT EXISTS exercise_record (
    id TEXT PRIMARY KEY,
    exerciseId INTEGER,
    exerciseName TEXT,
    portion INTEGER,
    totalCalories INTEGER,
    record_date TEXT
  )`;
  private sqlCreateHealthTable = `CREATE TABLE IF NOT EXISTS health_data (
    record_date TEXT PRIMARY KEY,
    height REAL,
    weight REAL,
    age INTEGER,
    bmi REAL,
    target_calories INTEGER,
    target_water INTEGER,
    target_exercise INTEGER
  )`;

  public static getInstance(): RdbManager {
    if (!RdbManager.instance) {
      RdbManager.instance = new RdbManager();
    }
    return RdbManager.instance;
  }

  // 初始化数据库 (通常在 UIAbility 或页面加载时调用)
  async init(context: common.Context): Promise<void> {
    if (this.rdbStore) {
      return;
    }

    const config: relationalStore.StoreConfig = {
      name: 'LifeBalance.db', // 数据库文件名
      securityLevel: relationalStore.SecurityLevel.S1
    };

    try {
      this.rdbStore = await relationalStore.getRdbStore(context, config);

      // 如果是第一次创建，版本号为0，执行建表
      if (this.rdbStore.version === 0) {
        await this.rdbStore.executeSql(this.sqlCreateHealthTable);
        await this.rdbStore.executeSql(this.sqlCreateTable);
        await this.rdbStore.executeSql(this.sqlCreateWaterTable);
        await this.rdbStore.executeSql(this.sqlCreateExerciseTable)
        // await this.insertMockData(); // 虚拟数据插入
        this.rdbStore.version = 1;
      }
      console.info('[RdbManager] Database initialized');
    } catch (err) {
      console.error(`[RdbManager] Init failed: ${JSON.stringify(err)}`);
    }
  }

  async insertMockData() {
    if (!this.rdbStore) {
      return;
    }

    // 1. 确定起点：昨天 (今天 - 1天)
    // 关键点：设置时间为中午 12:00，防止凌晨时区偏移问题
    let startDate = new Date();
    startDate.setHours(12, 0, 0, 0);
    startDate.setDate(startDate.getDate() - 1);

    // 检查昨天是否已有数据 (防止重复)
    let checkDateStr = this.getLocalDateStr(startDate);
    let predicates = new relationalStore.RdbPredicates('water_record');
    predicates.equalTo('record_date', checkDateStr);
    let resultSet = await this.rdbStore.query(predicates);

    if (resultSet.rowCount > 0) {
      console.info('[RdbManager] 昨天已有数据，跳过模拟');
      resultSet.close();
      return;
    }
    resultSet.close();

    console.info('[RdbManager] 开始模拟数据：从昨天(1.27)开始，往前推 6 天...');

    // 准备 6 天的数据 (索引0=昨天，索引1=前天... 索引5=大前天)
    // 假设今天是 1.28
    // i=0 -> 1.27 (昨天)
    // i=1 -> 1.26
    // ...
    // i=5 -> 1.22
    const mockWater = [2000, 1800, 2500, 1600, 1950, 1200];
    const mockWeight = [55.0, 55.1, 55.2, 54.8, 55.0, 55.1];
    const mockDietCals = [1580, 1450, 1600, 1300, 1650, 1250];
    const mockExCals = [150, 200, 0, 300, 0, 250];
    const mockExPortions = [2, 2, 0, 4, 0, 3];

    // 循环 6 次，生成 6 天的历史数据
    for (let i = 0; i < 6; i++) {
      let d = new Date(startDate); // 基于"昨天"
      d.setDate(d.getDate() - i); // 往前推 i 天

      let dateStr = this.getLocalDateStr(d);
      let timestamp = d.getTime();

      // --- 插入饮水 ---
      let waterBucket: relationalStore.ValuesBucket = {
        'record_date': dateStr,
        'actual_water': mockWater[i],
        'goal_water': 1925
      };
      await this.rdbStore.insert('water_record', waterBucket);

      // --- 插入健康数据 ---
      let healthBucket: relationalStore.ValuesBucket = {
        'record_date': dateStr,
        'height': 160,
        'weight': mockWeight[i],
        'age': 22,
        'bmi': parseFloat((mockWeight[i] / (1.60 * 1.60)).toFixed(1)),
        'target_calories': 1550,
        'target_water': 1925,
        'target_exercise': 3
      };
      await this.rdbStore.insert('health_data', healthBucket);

      // --- 插入饮食 ---
      if (mockDietCals[i] > 0) {
        let dietBucket: relationalStore.ValuesBucket = {
          'id': `${timestamp}_diet_mock`,
          'foodId': 999,
          'foodName': '历史饮食汇总',
          'mealTime': '晚餐',
          'portion': 1,
          'totalCalories': mockDietCals[i],
          'record_date': dateStr
        };
        await this.rdbStore.insert('diet_record', dietBucket);
      }

      // --- 插入运动 ---
      if (mockExCals[i] > 0) {
        let exBucket: relationalStore.ValuesBucket = {
          'id': `${timestamp}_ex_mock`,
          'exerciseId': 999,
          'exerciseName': '历史运动汇总',
          'portion': mockExPortions[i],
          'totalCalories': mockExCals[i],
          'record_date': dateStr
        };
        await this.rdbStore.insert('exercise_record', exBucket);
      }
    }
    console.info('[RdbManager] 6天历史数据模拟完成！');
  }

  getRdbStore(): relationalStore.RdbStore | null {
    return this.rdbStore;
  }

  private getLocalDateStr(date: Date): string {
    const y = date.getFullYear();
    const m = (date.getMonth() + 1).toString().padStart(2, '0');
    const d = date.getDate().toString().padStart(2, '0');
    return `${y}-${m}-${d}`;
  }
}