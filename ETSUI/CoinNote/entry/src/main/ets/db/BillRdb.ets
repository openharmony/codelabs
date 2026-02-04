/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import relationalStore from '@ohos.data.relationalStore';
import common from '@ohos.app.ability.common';
import { BillItem } from '../model/BillItem';

interface RdbError {
    code: number;
    message: string;
}

class BillRdb {
  private rdbStore: relationalStore.RdbStore | null = null;
  private tableName: string = 'BILL_TABLE';
  private isMock: boolean = false; // 是否为模拟模式（用于预览器）
  private mockData: BillItem[] = []; // 模拟数据存储
  
  // 建表语句
  private sqlCreate: string = `CREATE TABLE IF NOT EXISTS BILL_TABLE (
    id INTEGER PRIMARY KEY,
    type TEXT,
    amount REAL,
    date TEXT,
    payType TEXT,
    tags TEXT,
    isReimbursable INTEGER,
    isReimbursementReceived INTEGER,
    isAA INTEGER,
    isAaReceived INTEGER,
    originalAmount REAL,
    aaReceivedAmount REAL,
    note TEXT,
    direction TEXT
  )`;

  constructor() {}

  // 初始化数据库
  getRdbStore(context: common.UIAbilityContext): Promise<relationalStore.RdbStore | null> {
    if (this.rdbStore) {
      return Promise.resolve(this.rdbStore);
    }

    const STORE_CONFIG: relationalStore.StoreConfig = {
      name: 'CoinNote_v6.db', // 升级数据库版本以应用新表结构
      securityLevel: relationalStore.SecurityLevel.S1
    };

    return new Promise((resolve, reject) => {
      try {
        relationalStore.getRdbStore(context, STORE_CONFIG, (err: RdbError | null, store: relationalStore.RdbStore) => {
          if (err) {
            console.warn(`Failed to get RdbStore. Code:${err.code}, message:${err.message}`);
            console.warn('Switching to Mock Mode for Previewer');
            this.isMock = true;
            resolve(null); // 即使失败也 resolve，以便 UI 继续运行
            return;
          }
          console.info('Succeeded in getting RdbStore.');
          
          // 创建表
          if (store) {
              store.executeSql(this.sqlCreate).then(() => {
                  console.info('Succeeded in creating table.');
                  this.rdbStore = store;
                  resolve(store);
              }).catch((sqlErr: Error) => {
                  console.error(`Failed to create table. message:${sqlErr.message}`);
                  this.isMock = true; // 建表失败也切换到 Mock 模式
                  resolve(store);
              });
          } else {
              this.isMock = true;
              resolve(null);
          }
        });
      } catch (e) {
        console.warn('RDB API crashed (likely in Previewer), switching to Mock Mode');
        this.isMock = true;
        resolve(null);
      }
    });
  }

  // 检查是否已初始化
  isInitialized(): boolean {
    return this.rdbStore !== null || this.isMock;
  }

  // 插入数据
  insert(bill: BillItem): Promise<number> {
    if (this.isMock) {
      this.mockData.unshift(bill);
      return Promise.resolve(1);
    }

    if (!this.rdbStore) return Promise.reject(new Error('Store not initialized'));
    
    const valueBucket: relationalStore.ValuesBucket = {
      id: bill.id,
      type: bill.type,
      amount: bill.amount,
      date: bill.date,
      payType: bill.payType,
      tags: JSON.stringify(bill.tags),
      isReimbursable: bill.isReimbursable ? 1 : 0,
      isReimbursementReceived: bill.isReimbursementReceived ? 1 : 0,
      isAA: bill.isAA ? 1 : 0,
      isAaReceived: bill.isAaReceived ? 1 : 0,
      originalAmount: bill.originalAmount || 0,
      aaReceivedAmount: bill.aaReceivedAmount || 0,
      note: bill.note,
      direction: bill.direction
    };

    return this.rdbStore.insert(this.tableName, valueBucket);
  }

  // 更新数据
  update(bill: BillItem): Promise<number> {
    if (this.isMock) {
      const index = this.mockData.findIndex(item => item.id === bill.id);
      if (index !== -1) {
        this.mockData[index] = bill;
        return Promise.resolve(1);
      }
      return Promise.resolve(0);
    }

    if (!this.rdbStore) return Promise.reject(new Error('Store not initialized'));
    
    const valueBucket: relationalStore.ValuesBucket = {
      type: bill.type,
      amount: bill.amount,
      date: bill.date,
      payType: bill.payType,
      tags: JSON.stringify(bill.tags),
      isReimbursable: bill.isReimbursable ? 1 : 0,
      isReimbursementReceived: bill.isReimbursementReceived ? 1 : 0,
      isAA: bill.isAA ? 1 : 0,
      isAaReceived: bill.isAaReceived ? 1 : 0,
      originalAmount: bill.originalAmount || 0,
      aaReceivedAmount: bill.aaReceivedAmount || 0,
      note: bill.note,
      direction: bill.direction
    };

    let predicates = new relationalStore.RdbPredicates(this.tableName);
    predicates.equalTo('id', bill.id);
    return this.rdbStore.update(valueBucket, predicates);
  }

  // 删除数据
  delete(id: number): Promise<number> {
    if (this.isMock) {
      this.mockData = this.mockData.filter(item => item.id !== id);
      return Promise.resolve(1);
    }

    if (!this.rdbStore) return Promise.reject(new Error('Store not initialized'));
    
    let predicates = new relationalStore.RdbPredicates(this.tableName);
    predicates.equalTo('id', id);
    return this.rdbStore.delete(predicates);
  }

  // 查询所有数据
  queryAll(): Promise<BillItem[]> {
    if (this.isMock) {
      return Promise.resolve([...this.mockData]);
    }

    if (!this.rdbStore) return Promise.reject(new Error('Store not initialized'));
    
    let predicates = new relationalStore.RdbPredicates(this.tableName);
    predicates.orderByDesc('id');
    
    return this.rdbStore.query(predicates).then((resultSet) => {
      let count = resultSet.rowCount;
      let result: BillItem[] = [];
      if (count === 0) {
        resultSet.close();
        return result;
      }
      resultSet.goToFirstRow();
      for (let i = 0; i < count; i++) {
        let item: BillItem = {
          id: resultSet.getLong(resultSet.getColumnIndex('id')),
          type: resultSet.getString(resultSet.getColumnIndex('type')),
          amount: resultSet.getDouble(resultSet.getColumnIndex('amount')),
          date: resultSet.getString(resultSet.getColumnIndex('date')),
          payType: resultSet.getString(resultSet.getColumnIndex('payType')),
          tags: JSON.parse(resultSet.getString(resultSet.getColumnIndex('tags')) || '[]') as string[],
          isReimbursable: resultSet.getLong(resultSet.getColumnIndex('isReimbursable')) === 1,
          isReimbursementReceived: resultSet.getLong(resultSet.getColumnIndex('isReimbursementReceived')) === 1,
          isAA: resultSet.getLong(resultSet.getColumnIndex('isAA')) === 1,
          isAaReceived: resultSet.getLong(resultSet.getColumnIndex('isAaReceived')) === 1,
          originalAmount: resultSet.getDouble(resultSet.getColumnIndex('originalAmount')),
          aaReceivedAmount: resultSet.getDouble(resultSet.getColumnIndex('aaReceivedAmount')),
          note: resultSet.getString(resultSet.getColumnIndex('note')),
          direction: resultSet.getString(resultSet.getColumnIndex('direction')) as 'expense' | 'income'
        };
        result.push(item);
        resultSet.goToNextRow();
      }
      resultSet.close();
      return result;
    });
  }

  // 清空所有数据
  clearAll(): Promise<void> {
    if (this.isMock) {
      this.mockData = [];
      return Promise.resolve();
    }

    if (!this.rdbStore) return Promise.reject(new Error('Store not initialized'));

    let predicates = new relationalStore.RdbPredicates(this.tableName);
    return this.rdbStore.delete(predicates).then(() => {
      return;
    });
  }
}

export default new BillRdb();
