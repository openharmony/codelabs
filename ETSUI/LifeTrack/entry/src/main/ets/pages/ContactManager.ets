/*
 * Copyright (c) 2026 BIT-Group23
 * Licensed under the Apache License, Version 2.0 (the");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-20
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { ContactDao } from '../dao/ContactDao';
import { ContactDBConfig } from '../dao/ContactDBConfig';
import common from '@ohos.app.ability.common'
import {
  ContactsItem,
  InsertContactData,
  OperationResult,
  BatchContactData
} from '../model/ContactModel';

// 联系人管理器 - 业务逻辑层
export class ContactManager {
  private contactDao: ContactDao;
  private static instance: ContactManager | null = null;

  private constructor() {
    this.contactDao = new ContactDao();
  }

  // 单例模式
  public static getInstance(): ContactManager {
    if (!ContactManager.instance) {
      ContactManager.instance = new ContactManager();
    }
    return ContactManager.instance;
  }

  // 初始化数据库（需要在UI组件中调用，因为需要context）
  public async initDatabase(context: common.Context): Promise<boolean> {
    try {
      const dbConfig = ContactDBConfig.getInstance();
      return await dbConfig.initDB(context);
    } catch (error) {
      console.error('[ContactManager] 初始化数据库失败:', error);
      return false;
    }
  }

  // 获取所有联系人（已按首字母排序）
  public async getAllContacts(): Promise<ContactsItem[]> {
    try {
      return await this.contactDao.getAllContacts();
    } catch (error) {
      console.error('[ContactManager] 获取所有联系人失败:', error);
      return [];
    }
  }

  // 搜索联系人
  public async searchContacts(keyword: string): Promise<ContactsItem[]> {
    if (!keyword || keyword.trim() === '') {
      return await this.getAllContacts();
    }

    try {
      return await this.contactDao.searchContacts(keyword);
    } catch (error) {
      console.error('[ContactManager] 搜索联系人失败:', error);
      return [];
    }
  }

  // 根据ID获取联系人
  public async getContactById(id: number): Promise<ContactsItem | null> {
    if (!id || id <= 0) {
      return null;
    }

    try {
      return await this.contactDao.getContactById(id);
    } catch (error) {
      console.error('[ContactManager] 获取联系人失败:', error);
      return null;
    }
  }

  // 添加联系人
  public async addContact(name: string, phone: string): Promise<OperationResult> {
    // 验证输入
    if (!name || name.trim() === '') {
      return { success: false, message: '姓名不能为空' };
    }

    if (!phone || phone.trim() === '') {
      return { success: false, message: '电话号码不能为空' };
    }

    // 验证电话号码格式（简单验证）
    if (!/^[\d\-\+\(\)\s]{7,20}$/.test(phone.trim())) {
      return { success: false, message: '电话号码格式不正确' };
    }

    // 检查电话号码是否已存在
    const phoneExist = await this.contactDao.isPhoneExist(phone.trim());
    if (phoneExist) {
      return { success: false, message: '该电话号码已存在' };
    }

    try {
      const contactData: InsertContactData = {
        name: name.trim(),
        phone: phone.trim(),
        initial: '' // 会自动生成
      };

      return await this.contactDao.insertContact(contactData);
    } catch (error) {
      console.error('[ContactManager] 添加联系人失败:', error);
      return { success: false, message: `添加失败: ${error.message || '未知错误'}` };
    }
  }

  // 更新联系人
  public async updateContact(contact: ContactsItem): Promise<OperationResult> {
    // 验证输入
    if (!contact.name || contact.name.trim() === '') {
      return { success: false, message: '姓名不能为空' };
    }

    if (!contact.phone || contact.phone.trim() === '') {
      return { success: false, message: '电话号码不能为空' };
    }

    // 验证电话号码格式
    if (!/^[\d\-\+\(\)\s]{7,20}$/.test(contact.phone.trim())) {
      return { success: false, message: '电话号码格式不正确' };
    }

    // 检查电话号码是否已存在（排除自己）
    const phoneExist = await this.contactDao.isPhoneExist(contact.phone.trim(), contact.id);
    if (phoneExist) {
      return { success: false, message: '该电话号码已被其他联系人使用' };
    }

    try {
      return await this.contactDao.updateContact(contact);
    } catch (error) {
      console.error('[ContactManager] 更新联系人失败:', error);
      return { success: false, message: `更新失败: ${error.message || '未知错误'}` };
    }
  }

  // 删除联系人
  public async deleteContact(id: number): Promise<OperationResult> {
    if (!id || id <= 0) {
      return { success: false, message: '无效的联系人ID' };
    }

    try {
      return await this.contactDao.deleteContact(id);
    } catch (error) {
      console.error('[ContactManager] 删除联系人失败:', error);
      return { success: false, message: `删除失败: ${error.message || '未知错误'}` };
    }
  }

  // 获取联系人数量
  public async getContactCount(): Promise<number> {
    try {
      return await this.contactDao.getContactCount();
    } catch (error) {
      console.error('[ContactManager] 获取联系人数量失败:', error);
      return 0;
    }
  }

  // 批量插入联系人（用于初始化示例数据）
  public async batchInsertContacts(contacts: BatchContactData[]): Promise<OperationResult> {
    try {
      return await this.contactDao.batchInsertContacts(contacts);
    } catch (error) {
      console.error('[ContactManager] 批量插入联系人失败:', error);
      return { success: false, message: `批量插入失败: ${error.message || '未知错误'}` };
    }
  }

  // 清空所有联系人（用于测试）
  public async clearAllContacts(): Promise<OperationResult> {
    try {
      return await this.contactDao.clearAllContacts();
    } catch (error) {
      console.error('[ContactManager] 清空联系人失败:', error);
      return { success: false, message: `清空失败: ${error.message || '未知错误'}` };
    }
  }

  // 检查电话号码是否已存在
  public async isPhoneExist(phone: string, excludeId?: number): Promise<boolean> {
    try {
      return await this.contactDao.isPhoneExist(phone, excludeId);
    } catch (error) {
      console.error('[ContactManager] 检查电话号码失败:', error);
      return false;
    }
  }

  // 初始化示例数据
  public async initSampleData(): Promise<OperationResult> {
    try {
      // 先检查是否有数据
      const count = await this.getContactCount();
      if (count > 0) {
        return { success: true, message: '已有联系人数据，无需初始化' };
      }

      // 示例数据
      const sampleContacts: BatchContactData[] = [
        { name: 'Alice Johnson', phone: '123-456-7890' },
        { name: 'Bob Chen', phone: '123-456-7891' },
        { name: 'Charlie Wang', phone: '123-456-7892' },
        { name: 'Daisy Liu', phone: '123-456-7893' },
        { name: 'Eric Zhang', phone: '123-456-7894' },
        { name: 'Amy Lee', phone: '123-456-7895' },
        { name: 'Brian Smith', phone: '123-456-7896' },
        { name: 'Catherine Zhao', phone: '123-456-7897' },
        { name: '张三', phone: '13800138000' },
        { name: '李四', phone: '13800138001' },
        { name: '王五', phone: '13800138002' },
      ];

      return await this.batchInsertContacts(sampleContacts);
    } catch (error) {
      console.error('[ContactManager] 初始化示例数据失败:', error);
      return { success: false, message: `初始化失败: ${error.message || '未知错误'}` };
    }
  }

  // 分组联系人（按首字母） - 供UI层使用
  public groupContactsByInitial(contacts: ContactsItem[]): Map<string, ContactsItem[]> {
    const grouped = new Map<string, ContactsItem[]>();

    for (const contact of contacts) {
      const initial = contact.initial || '#';
      if (!grouped.has(initial)) {
        grouped.set(initial, []);
      }
      grouped.get(initial)?.push(contact);
    }

    // 按字母排序
    const sortedEntries = Array.from(grouped.entries());
    sortedEntries.sort((entryA, entryB) => {
      const initialA = entryA[0];
      const initialB = entryB[0];
      return initialA.localeCompare(initialB);
    });

    return new Map(sortedEntries);
  }

  // 获取字母索引列表 - 供UI层使用
  public getInitialsList(contacts: ContactsItem[]): string[] {
    const initials = new Set<string>();

    for (const contact of contacts) {
      const initial = contact.initial || '#';
      initials.add(initial);
    }

    return Array.from(initials).sort();
  }
}