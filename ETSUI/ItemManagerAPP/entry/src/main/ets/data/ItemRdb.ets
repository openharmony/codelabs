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
import { Item, ItemUseStatus } from '../model/Item';

// 数据库错误类型定义
interface RdbError {
  code: number;
  message: string;
}

class ItemRdb {
  private rdbStore: relationalStore.RdbStore | null = null;
  private tableName: string = 'ITEM_TABLE';
  private isMock: boolean = false; // 预览器兼容模式
  private mockData: Item[] = []; // 预览器模拟数据

  // 建表语句（严格对齐Item接口字段）
  private sqlCreate: string = `CREATE TABLE IF NOT EXISTS ITEM_TABLE (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    category TEXT DEFAULT '其他',
    buyDate TEXT,
    price REAL DEFAULT 0.0,
    location TEXT,
    note TEXT,
    useStatus TEXT DEFAULT '在用',
    isLent INTEGER DEFAULT 0,
    lendTo TEXT,
    returnDate TEXT,
    isPrecious INTEGER DEFAULT 0,
    maintenanceDate TEXT,
    imagePath TEXT
  )`;

  private static instance: ItemRdb;

  // 单例模式（全局唯一数据库实例）
  static getInstance(): ItemRdb {
    if (!ItemRdb.instance) {
      ItemRdb.instance = new ItemRdb();
    }
    return ItemRdb.instance;
  }

  private constructor() {}

  // 初始化数据库
  async getRdbStore(context: common.UIAbilityContext): Promise<relationalStore.RdbStore | null> {
    if (this.rdbStore) {
      return this.rdbStore;
    }

    const STORE_CONFIG: relationalStore.StoreConfig = {
      name: 'ItemManager.db',
      securityLevel: relationalStore.SecurityLevel.S1
    };

    try {
      return new Promise((resolve, reject) => {
        relationalStore.getRdbStore(context, STORE_CONFIG, (err: RdbError | null, store: relationalStore.RdbStore) => {
          if (err) {
            console.warn(`获取数据库失败: Code=${err.code}, Msg=${err.message}`);
            console.warn('切换到预览器模拟模式');
            this.isMock = true;
            // 初始化模拟数据
            this.mockData = [
              {
                id: Date.now() - 10000,
                name: "小米充电宝",
                category: "数码产品",
                buyDate: "2024/5/12",
                price: 99,
                location: "书桌抽屉",
                note: "20000mAh",
                useStatus: "在用"
              },
              {
                id: Date.now() - 20000,
                name: "纯棉T恤",
                category: "衣物",
                buyDate: "2024/3/8",
                price: 79,
                location: "衣柜第二层",
                note: "白色 M码",
                useStatus: "闲置"
              }
            ];
            resolve(null);
            return;
          }

          // 创建数据表
          store.executeSql(this.sqlCreate).then(() => {
            console.info('物品数据表创建成功');
            this.rdbStore = store;
            resolve(store);
          }).catch((sqlErr:Error) => {
            console.error(`创建数据表失败: ${sqlErr.message}`);
            this.isMock = true;
            resolve(store);
          });
        });
      });
    } catch (e) {
      console.warn('数据库API异常（预览器环境），切换到模拟模式');
      this.isMock = true;
      return null;
    }
  }

  // 检查数据库是否初始化
  isInitialized(): boolean {
    return this.rdbStore !== null || this.isMock;
  }

  // 插入物品
  async insert(item: Item): Promise<number> {
    if (this.isMock) {
      this.mockData.push(item);
      return 1;
    }

    if (!this.rdbStore) {
      throw new Error('数据库未初始化');
    }

    const valueBucket: relationalStore.ValuesBucket = {
      id: item.id,
      name: item.name,
      category: item.category || "其他",
      buyDate: item.buyDate,
      price: item.price,
      location: item.location || "",
      note: item.note || "",
      useStatus: item.useStatus,
      isLent: item.isLent ? 1 : 0,
      lendTo: item.lendTo || "",
      returnDate: item.returnDate || "",
      isPrecious: item.isPrecious ? 1 : 0,
      maintenanceDate: item.maintenanceDate || "",
      imagePath: item.imagePath || ""
    };

    return this.rdbStore.insert(this.tableName, valueBucket);
  }

  // ========== 核心新增：更新物品 ==========
  async update(item: Item): Promise<number> {
    if (this.isMock) {
      // 预览器模拟更新
      const index = this.mockData.findIndex(d => d.id === item.id);
      if (index !== -1) {
        this.mockData[index] = item;
        return 1; // 更新成功返回1
      }
      return 0; // 未找到数据返回0
    }

    if (!this.rdbStore) {
      throw new Error('数据库未初始化');
    }

    // 构建更新的字段集合
    const valueBucket: relationalStore.ValuesBucket = {
      name: item.name,
      category: item.category || "其他",
      buyDate: item.buyDate,
      price: item.price,
      location: item.location || "",
      note: item.note || "",
      useStatus: item.useStatus,
      isLent: item.isLent ? 1 : 0,
      lendTo: item.lendTo || "",
      returnDate: item.returnDate || "",
      isPrecious: item.isPrecious ? 1 : 0,
      maintenanceDate: item.maintenanceDate || "",
      imagePath: item.imagePath || ""
    };

    // 条件：根据id更新
    const predicates = new relationalStore.RdbPredicates(this.tableName);
    predicates.equalTo('id', item.id);

    // 执行更新
    return this.rdbStore.update(valueBucket, predicates);
  }

  // 删除物品
  async delete(id: number): Promise<number> {
    if (this.isMock) {
      this.mockData = this.mockData.filter(item => item.id !== id);
      return 1;
    }

    if (!this.rdbStore) {
      throw new Error('数据库未初始化');
    }

    const predicates = new relationalStore.RdbPredicates(this.tableName);
    predicates.equalTo('id', id);
    return this.rdbStore.delete(predicates);
  }

  // 查询所有物品
  async queryAll(): Promise<Item[]> {
    if (this.isMock) {
      return [...this.mockData]; // 返回副本，避免外部修改
    }

    if (!this.rdbStore) {
      throw new Error('数据库未初始化');
    }

    const predicates = new relationalStore.RdbPredicates(this.tableName);
    predicates.orderByDesc('id'); // 按id倒序，最新添加的在前面

    const resultSet = await this.rdbStore.query(predicates);
    const items: Item[] = [];
    const count = resultSet.rowCount;

    if (count === 0) {
      resultSet.close();
      return items;
    }

    // 遍历结果集转换为Item对象
    resultSet.goToFirstRow();
    for (let i = 0; i < count; i++) {
      const useStatus = resultSet.getString(resultSet.getColumnIndex('useStatus')) as ItemUseStatus;
      items.push({
        id: resultSet.getLong(resultSet.getColumnIndex('id')),
        name: resultSet.getString(resultSet.getColumnIndex('name')),
        category: resultSet.getString(resultSet.getColumnIndex('category')),
        buyDate: resultSet.getString(resultSet.getColumnIndex('buyDate')),
        price: resultSet.getDouble(resultSet.getColumnIndex('price')),
        location: resultSet.getString(resultSet.getColumnIndex('location')),
        note: resultSet.getString(resultSet.getColumnIndex('note')),
        useStatus: useStatus,
        isLent: resultSet.getLong(resultSet.getColumnIndex('isLent')) === 1,
        lendTo: resultSet.getString(resultSet.getColumnIndex('lendTo')),
        returnDate: resultSet.getString(resultSet.getColumnIndex('returnDate')),
        isPrecious: resultSet.getLong(resultSet.getColumnIndex('isPrecious')) === 1,
        maintenanceDate: resultSet.getString(resultSet.getColumnIndex('maintenanceDate')),
        imagePath: resultSet.getString(resultSet.getColumnIndex('imagePath'))
      });
      resultSet.goToNextRow();
    }

    resultSet.close(); // 必须关闭结果集，避免内存泄漏
    return items;
  }

  // 清空所有数据（可选）
  async clearAll(): Promise<void> {
    if (this.isMock) {
      this.mockData = [];
      return;
    }

    if (!this.rdbStore) {
      throw new Error('数据库未初始化');
    }

    const predicates = new relationalStore.RdbPredicates(this.tableName);
    await this.rdbStore.delete(predicates);
  }
}

// 导出单例实例，全局唯一
export default ItemRdb.getInstance();