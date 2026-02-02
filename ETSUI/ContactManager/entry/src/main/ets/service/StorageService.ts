/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 *
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
import preferences from '@ohos.data.preferences';
import common from '@ohos.app.ability.common';
import { BusinessError } from '@ohos.base';

/**
 * 统一存储服务
 * 基于 Preferences 实现轻量级持久化存储
 */
export class StorageService {
  private static instance: StorageService;
  private dataStore: preferences.Preferences | null = null;
  private readonly STORE_NAME = 'contact_metadata_store';

  /**
   * 获取单例实例
   */
  static getInstance(): StorageService {
    if (!StorageService.instance) {
      StorageService.instance = new StorageService();
    }
    return StorageService.instance;
  }

  /**
   * 初始化存储
   */
  async init(): Promise<void> {
    if (this.dataStore) {
      return; // 已初始化
    }

    try {
      // @ts-ignore
      const context = getContext() as common.UIAbilityContext;
      this.dataStore = await preferences.getPreferences(context, this.STORE_NAME);
      console.info('[StorageService] 初始化成功');
    } catch (err) {
      const error = err as BusinessError;
      console.error(`[StorageService] 初始化失败: ${error.code} ${error.message}`);
      throw error;
    }
  }

  /**
   * 确保存储已初始化
   */
  private async ensureInit(): Promise<void> {
    if (!this.dataStore) {
      await this.init();
    }
  }

  /**
   * 保存数据
   */
  async put(key: string, value: string | number | boolean | object): Promise<void> {
    await this.ensureInit();
    
    try {
      if (!this.dataStore) {
        throw new Error('数据存储未初始化');
      }

      // 如果是对象，转换为JSON字符串
      const valueToStore = typeof value === 'object' ? JSON.stringify(value) : value;
      
      await this.dataStore.put(key, valueToStore);
      await this.dataStore.flush(); // 立即持久化
      
      console.info(`[StorageService] 保存成功: ${key}`);
    } catch (err) {
      const error = err as BusinessError;
      console.error(`[StorageService] 保存失败: ${error.code} ${error.message}`);
      throw error;
    }
  }

  /**
   * 获取数据
   */
  async get(key: string, defaultValue: preferences.ValueType): Promise<preferences.ValueType> {
    await this.ensureInit();
    
    try {
      if (!this.dataStore) {
        return defaultValue;
      }

      const value = await this.dataStore.get(key, defaultValue);
      return value;
    } catch (err) {
      const error = err as BusinessError;
      console.error(`[StorageService] 获取失败: ${error.code} ${error.message}`);
      return defaultValue;
    }
  }

  /**
   * 获取字符串
   */
  async getString(key: string, defaultValue: string = ''): Promise<string> {
    const value = await this.get(key, defaultValue);
    return value as string;
  }

  /**
   * 获取数字
   */
  async getNumber(key: string, defaultValue: number = 0): Promise<number> {
    const value = await this.get(key, defaultValue);
    return value as number;
  }

  /**
   * 获取对象
   */
  async getObject<T>(key: string, defaultValue: T): Promise<T> {
    const jsonStr = await this.getString(key, '');
    
    if (!jsonStr) {
      return defaultValue;
    }

    try {
      return JSON.parse(jsonStr) as T;
    } catch (err) {
      console.error(`[StorageService] JSON解析失败: ${key}`, err);
      return defaultValue;
    }
  }

  /**
   * 删除数据
   */
  async delete(key: string): Promise<void> {
    await this.ensureInit();
    
    try {
      if (!this.dataStore) {
        return;
      }

      await this.dataStore.delete(key);
      await this.dataStore.flush();
      console.info(`[StorageService] 删除成功: ${key}`);
    } catch (err) {
      const error = err as BusinessError;
      console.error(`[StorageService] 删除失败: ${error.code} ${error.message}`);
      throw error;
    }
  }

  /**
   * 检查键是否存在
   */
  async has(key: string): Promise<boolean> {
    await this.ensureInit();
    
    try {
      if (!this.dataStore) {
        return false;
      }

      return await this.dataStore.has(key);
    } catch (err) {
      console.error('[StorageService] 检查键失败:', err);
      return false;
    }
  }

  /**
   * 获取所有键
   */
  async getAllKeys(): Promise<string[]> {
    await this.ensureInit();
    
    try {
      if (!this.dataStore) {
        return [];
      }

      // 获取所有键值对
      const allData = await this.dataStore.getAll();
      return Object.keys(allData);
    } catch (err) {
      console.error('[StorageService] 获取所有键失败:', err);
      return [];
    }
  }

  /**
   * 清空所有数据
   */
  async clear(): Promise<void> {
    await this.ensureInit();
    
    try {
      if (!this.dataStore) {
        return;
      }

      await this.dataStore.clear();
      await this.dataStore.flush();
      console.info('[StorageService] 清空所有数据成功');
    } catch (err) {
      const error = err as BusinessError;
      console.error(`[StorageService] 清空失败: ${error.code} ${error.message}`);
      throw error;
    }
  }
}
