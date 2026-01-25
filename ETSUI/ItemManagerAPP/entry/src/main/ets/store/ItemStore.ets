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
import ItemRdb from '../data/ItemRdb';

//物品全局状态存储，管理应用中的物品相关状态
export class ItemStore {
  private static instance: ItemStore;
  private itemCount: number = 0;
  private listeners: ((count: number) => void)[] = [];
  private clearListeners: (() => void)[] = []; // 新增：清空数据监听器

  // 单例模式
  static getInstance(): ItemStore {
    if (!ItemStore.instance) {
      ItemStore.instance = new ItemStore();
    }
    return ItemStore.instance;
  }

  private constructor() {
    console.info('ItemStore 初始化');
  }

  //获取当前物品数量
  getItemCount(): number {
    return this.itemCount;
  }

  //更新物品数量（从数据库读取并更新状态）
  async updateItemCount(): Promise<void> {
    try {
      const items = await ItemRdb.queryAll();
      const newCount = items.length;

      if (newCount !== this.itemCount) {
        console.info(`ItemStore: 物品数量更新 ${this.itemCount} -> ${newCount}`);
        this.itemCount = newCount;
        this.notifyListeners();
      }
    } catch (error) {
      console.error('ItemStore: 更新物品数量失败:', error);
      this.itemCount = 0;
      this.notifyListeners();
    }
  }

  //强制设置物品数量（供清空数据等操作使用）
  setItemCount(count: number): void {
    console.info(`ItemStore: 强制设置物品数量 ${this.itemCount} -> ${count}`);
    this.itemCount = count;
    this.notifyListeners();
  }

  //清空所有物品数据
  async clearAllItems(): Promise<void> {
    try {
      await ItemRdb.clearAll();
      console.info('ItemStore: 数据库清空成功');
      this.setItemCount(0); // 直接设置为0，触发监听

      // 新增：触发清空数据事件
      this.notifyClearListeners();
    } catch (error) {
      console.error('ItemStore: 清空数据库失败:', error);
      if (error instanceof Error) {
        throw error;
      } else {
        throw new Error(`清空数据库失败: ${String(error)}`);
      }
    }
  }

  //监听物品数量变化
  onItemCountChange(callback: (count: number) => void): void {
    console.info('ItemStore: 添加物品数量监听器');
    this.listeners.push(callback);
  }

  //移除物品数量监听
  offItemCountChange(callback: (count: number) => void): void {
    const index = this.listeners.indexOf(callback);
    if (index > -1) {
      console.info('ItemStore: 移除物品数量监听器');
      this.listeners.splice(index, 1);
    }
  }

  //监听数据清空事件
  onDataCleared(callback: () => void): void {
    console.info('ItemStore: 添加数据清空监听器');
    this.clearListeners.push(callback);
  }

  //移除数据清空监听
  offDataCleared(callback: () => void): void {
    const index = this.clearListeners.indexOf(callback);
    if (index > -1) {
      console.info('ItemStore: 移除数据清空监听器');
      this.clearListeners.splice(index, 1);
    }
  }

  //通知所有物品数量监听者
  private notifyListeners(): void {
    console.info(`ItemStore: 通知 ${this.listeners.length} 个物品数量监听器`);
    this.listeners.forEach(callback => {
      try {
        callback(this.itemCount);
      } catch (error) {
        console.error('ItemStore: 执行物品数量监听回调出错:', error);
      }
    });
  }

  //通知所有清空数据监听者（新增）
  private notifyClearListeners(): void {
    console.info(`ItemStore: 通知 ${this.clearListeners.length} 个清空数据监听器`);
    this.clearListeners.forEach(callback => {
      try {
        callback();
      } catch (error) {
        console.error('ItemStore: 执行清空数据监听回调出错:', error);
      }
    });
  }
}