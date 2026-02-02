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
import { Contact as LocalContact } from '../model/Contact'
import { StorageService } from './StorageService'

/**
 * 联系人服务 - 基于本地存储实现
 * 不依赖系统联系人API，完全使用本地数据库
 */
export class ContactService {
  private static readonly CONTACT_PREFIX = 'contact_';
  private static readonly ALL_CONTACTS_KEY = 'all_contact_keys';
  private static storageService = StorageService.getInstance();

  /**
   * 初始化服务
   */
  static async init(): Promise<void> {
    await this.storageService.init();
  }

  /**
   * 生成唯一ID
   */
  private static generateId(): string {
    return 'contact_' + Date.now() + '_' + Math.random().toString(36).substr(2, 9);
  }

  /**
   * 生成存储键
   */
  private static getStorageKey(key: string): string {
    return `${this.CONTACT_PREFIX}${key}`;
  }

  /**
   * 【增】添加联系人
   */
  static async addContact(c: LocalContact): Promise<string> {
    try {
      // 生成唯一的key
      const newKey = this.generateId();
      
      // 设置联系人的key和id
      c.key = newKey;
      c.id = newKey;

      // 保存到本地存储
      const storageKey = this.getStorageKey(newKey);
      await this.storageService.put(storageKey, this.contactToJSON(c));

      // 更新联系人列表
      await this.addToContactList(newKey);

      console.info('[ContactService] 添加联系人成功:', newKey);
      return newKey;

    } catch (err) {
      console.error(`[ContactService] 添加联系人失败:`, err);
      throw err;
    }
  }

  /**
   * 【查】查询所有联系人
   */
  static async getAllContacts(): Promise<LocalContact[]> {
    try {
      const keys = await this.getAllContactKeys();
      const contacts: LocalContact[] = [];

      for (const key of keys) {
        try {
          const storageKey = this.getStorageKey(key);
          const jsonData = await this.storageService.getString(storageKey, '');
          
          if (jsonData) {
            const contact = this.jsonToContact(jsonData);
            contacts.push(contact);
          }
        } catch (err) {
          console.error(`[ContactService] 读取联系人失败: ${key}`, err);
        }
      }

      console.info(`[ContactService] 查询到 ${contacts.length} 个联系人`);
      return contacts;

    } catch (err) {
      console.error('[ContactService] 查询联系人失败:', err);
      return [];
    }
  }

  /**
   * 【改】更新联系人
   */
  static async updateContact(c: LocalContact): Promise<void> {
    try {
      if (!c.key || c.key.trim() === '') {
        throw new Error('联系人key不能为空');
      }

      // 检查联系人是否存在
      const exists = await this.isContactExist(c.key);
      if (!exists) {
        throw new Error('联系人不存在');
      }

      // 更新到本地存储
      const storageKey = this.getStorageKey(c.key);
      await this.storageService.put(storageKey, this.contactToJSON(c));

      console.info('[ContactService] 更新联系人成功:', c.key);

    } catch (err) {
      console.error(`[ContactService] 更新联系人失败:`, err);
      throw err;
    }
  }

  /**
   * 【删】删除联系人
   */
  static async deleteContact(key: string): Promise<void> {
    if (!key || key.trim() === '') {
      console.error('[ContactService] 删除失败: key为空');
      return;
    }

    try {
      // 从存储中删除
      const storageKey = this.getStorageKey(key);
      await this.storageService.delete(storageKey);

      // 从联系人列表中移除
      await this.removeFromContactList(key);

      console.info(`[ContactService] 删除联系人成功, key: ${key}`);
    } catch (err) {
      console.error(`[ContactService] 删除联系人失败:`, err);
      throw err;
    }
  }

  /**
   * 验证联系人是否存在
   */
  static async isContactExist(key: string): Promise<boolean> {
    try {
      const storageKey = this.getStorageKey(key);
      return await this.storageService.has(storageKey);
    } catch (err) {
      console.error('[ContactService] 验证联系人失败:', err);
      return false;
    }
  }

  /**
   * 根据key获取单个联系人
   */
  static async getContactByKey(key: string): Promise<LocalContact | null> {
    try {
      const storageKey = this.getStorageKey(key);
      const jsonData = await this.storageService.getString(storageKey, '');
      
      if (jsonData) {
        return this.jsonToContact(jsonData);
      }
      return null;
    } catch (err) {
      console.error('[ContactService] 获取联系人失败:', err);
      return null;
    }
  }

  // ========== 内部工具方法 ==========

  /**
   * 联系人对象转JSON
   */
  private static contactToJSON(c: LocalContact): string {
    return JSON.stringify({
      id: c.id,
      key: c.key,
      name: c.name,
      phone: c.phone,
      email: c.email || '',
      remark: c.remark || ''
    });
  }

  /**
   * JSON转联系人对象
   */
  private static jsonToContact(json: string): LocalContact {
    const data = JSON.parse(json);
    const contact = new LocalContact(data.name || '', data.phone || '');
    contact.id = data.id || '';
    contact.key = data.key || '';
    contact.email = data.email || '';
    contact.remark = data.remark || '';
    return contact;
  }

  /**
   * 添加到联系人列表
   */
  private static async addToContactList(key: string): Promise<void> {
    const keys = await this.getAllContactKeys();
    if (!keys.includes(key)) {
      keys.push(key);
      await this.storageService.put(this.ALL_CONTACTS_KEY, JSON.stringify(keys));
    }
  }

  /**
   * 从联系人列表中移除
   */
  private static async removeFromContactList(key: string): Promise<void> {
    const keys = await this.getAllContactKeys();
    const newKeys = keys.filter(k => k !== key);
    await this.storageService.put(this.ALL_CONTACTS_KEY, JSON.stringify(newKeys));
  }

  /**
   * 获取所有联系人键
   */
  private static async getAllContactKeys(): Promise<string[]> {
    const jsonStr = await this.storageService.getString(this.ALL_CONTACTS_KEY, '[]');
    try {
      return JSON.parse(jsonStr) as string[];
    } catch (err) {
      console.error('[ContactService] 解析联系人键列表失败', err);
      return [];
    }
  }
}
