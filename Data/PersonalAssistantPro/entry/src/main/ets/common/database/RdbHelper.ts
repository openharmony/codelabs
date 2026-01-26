/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
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
// File: entry/src/main/ets/common/database/RdbHelper.ts
import relationalStore from '@ohos.data.relationalStore';
import { Logger } from '../utils/Logger';
import { TABLE_CONTACT, TABLE_EVENT, SQL_CREATE_CONTACT, SQL_CREATE_EVENT } from './Tables';
import { Context } from '@kit.AbilityKit';

export { TABLE_CONTACT, TABLE_EVENT };

export class RdbHelper {
  private rdbStore: relationalStore.RdbStore | null = null;
  private static instance: RdbHelper;
  private logger = new Logger('RdbHelper');

  private constructor() {}

  public static getInstance(): RdbHelper {
    if (!RdbHelper.instance) {
      RdbHelper.instance = new RdbHelper();
    }
    return RdbHelper.instance;
  }

  public initRdb(context: Context): Promise<void> {
    const STORE_CONFIG: relationalStore.StoreConfig = {
      name: 'PersonalAssistant.db',
      securityLevel: relationalStore.SecurityLevel.S1
    };

    return new Promise<void>((resolve, reject) => {
      relationalStore.getRdbStore(context, STORE_CONFIG, async (err, store) => {
        if (err) {
          this.logger.error(`Get RdbStore failed. Code:${err.code}, message:${err.message}`);
          reject(err);
          return;
        }

        this.rdbStore = store;
        this.logger.info('Get RdbStore successfully.');

        try {
          await this.rdbStore.executeSql(SQL_CREATE_CONTACT);
          await this.rdbStore.executeSql(SQL_CREATE_EVENT);

          this.logger.info('Tables checked/created successfully.');
          resolve();
        } catch (sqlErr) {
          this.logger.error(`Execute SQL failed: ${JSON.stringify(sqlErr)}`);
          reject(sqlErr);
        }
      });
    });
  }

  public insert(tableName: string, values: relationalStore.ValuesBucket): Promise<number> {
    if (!this.rdbStore) return Promise.reject('RdbStore not init');
    return this.rdbStore.insert(tableName, values);
  }

  public update(values: relationalStore.ValuesBucket, predicates: relationalStore.RdbPredicates): Promise<number> {
    if (!this.rdbStore) return Promise.reject('RdbStore not init');
    return this.rdbStore.update(values, predicates);
  }

  public query(predicates: relationalStore.RdbPredicates): Promise<relationalStore.ResultSet> {
    if (!this.rdbStore) return Promise.reject('RdbStore not init');
    return this.rdbStore.query(predicates);
  }

  public delete(predicates: relationalStore.RdbPredicates): Promise<number> {
    if (!this.rdbStore) return Promise.reject('RdbStore not init');
    return this.rdbStore.delete(predicates);
  }

  // ✅ 【新增】 通用模糊查询接口
  // 支持在指定的 columns 数组中搜索 keyword (使用 OR 连接)
  public queryByLike(tableName: string, columns: Array<string>, keyword: string): Promise<relationalStore.ResultSet> {
    if (!this.rdbStore) return Promise.reject('RdbStore not init');

    const predicates = new relationalStore.RdbPredicates(tableName);

    // 如果 keyword 为空，直接返回所有数据 (或者根据业务需求返回空，这里暂定查所有)
    if (!keyword || keyword.trim() === '') {
      return this.rdbStore.query(predicates);
    }

    // 构造 SQL: WHERE (col1 LIKE %key% OR col2 LIKE %key%)
    if (columns.length > 0) {
      predicates.beginWrap(); // 开始括号
      columns.forEach((col, index) => {
        predicates.contains(col, keyword); // contains 等同于 LIKE %keyword%
        if (index < columns.length - 1) {
          predicates.or();
        }
      });
      predicates.endWrap(); // 结束括号
    }

    // 按 ID 倒序排列，符合一般列表习惯
    predicates.orderByDesc('id');

    return this.rdbStore.query(predicates);
  }
}