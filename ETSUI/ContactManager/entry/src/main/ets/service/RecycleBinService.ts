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
import { DeletedContact } from '../model/DeletedContact';
import { Contact } from '../model/Contact';
import { ContactMetadata } from '../model/ContactMetadata';
import { StorageService } from './StorageService';
import { MetadataService } from './MetadataService';
import { ContactService } from './ContactService';

/**
 * 回收站服务
 * 负责管理已删除联系人的备份和恢复
 */
export class RecycleBinService {
  private static readonly DELETED_PREFIX = 'deleted_';
  private static readonly ALL_DELETED_KEY = 'all_deleted_ids';
  private static readonly STORAGE_KEY_PREFIX = 'recycle_bin_';
  private static storageService = StorageService.getInstance();

  /**
   * 初始化服务
   */
  static async init(): Promise<void> {
    await this.storageService.init();
  }

  /**
   * 添加删除记录（软删除）
   */
  static async addDeletedContact(
    contact: Contact,
    metadata?: ContactMetadata
  ): Promise<void> {
    const deletedContact = new DeletedContact(contact.key, contact, metadata);
    const storageKey = `${this.STORAGE_KEY_PREFIX}${deletedContact.backupId}`;
    
    // 保存删除记录
    await this.storageService.put(storageKey, deletedContact.toJSON());
    
    // 更新删除ID列表
    await this.addToDeletedList(deletedContact.backupId);
    
    console.info(`[RecycleBinService] 已备份删除的联系人: ${contact.name}, 备份ID: ${deletedContact.backupId}`);
  }

  /**
   * 获取所有已删除的联系人
   */
  static async getAllDeletedContacts(): Promise<DeletedContact[]> {
    const ids = await this.getAllDeletedIds();
    const deletedList: DeletedContact[] = [];

    for (const id of ids) {
      try {
        const storageKey = `${this.STORAGE_KEY_PREFIX}${id}`;
        const jsonData = await this.storageService.getObject<Record<string, string | number | object>>(
          storageKey,
          {}
        );

        if (jsonData && Object.keys(jsonData).length > 0) {
          const deleted = DeletedContact.fromJSON(jsonData);
          deletedList.push(deleted);
        }
      } catch (err) {
        console.error(`[RecycleBinService] 读取删除记录失败: ${id}`, err);
      }
    }

    // 按删除时间倒序排序
    return deletedList.sort((a, b) => b.deleteTime - a.deleteTime);
  }

  /**
   * 获取未过期的已删除联系人
   */
  static async getValidDeletedContacts(): Promise<DeletedContact[]> {
    const all = await this.getAllDeletedContacts();
    return all.filter(item => !item.isExpired());
  }

  /**
   * 恢复联系人
   */
  static async restoreContact(backupId: string): Promise<boolean> {
    try {
      // 1. 读取备份数据
      const storageKey = `${this.STORAGE_KEY_PREFIX}${backupId}`;
      const jsonData = await this.storageService.getObject<Record<string, string | number | object>>(
        storageKey,
        {}
      );

      if (!jsonData || Object.keys(jsonData).length === 0) {
        console.error(`[RecycleBinService] 未找到备份记录: ${backupId}`);
        return false;
      }

      const deleted = DeletedContact.fromJSON(jsonData);

      // 2. 重新创建联系人到系统
      const newKey = await ContactService.addContact(deleted.contactData);
      
      if (!newKey) {
        console.error('[RecycleBinService] 恢复联系人失败：系统创建失败');
        return false;
      }

      // 3. 恢复元数据（如果有）
      if (deleted.metadata) {
        const metadata = deleted.metadata;
        metadata.contactKey = newKey; // 更新为新的key
        await MetadataService.saveMetadata(metadata);
      } else {
        // 即使没有元数据，也初始化一个（用于记录创建时间）
        await MetadataService.getMetadata(newKey);
      }

      // 4. 从回收站删除
      await this.removeDeletedContact(backupId);

      console.info(`[RecycleBinService] 成功恢复联系人: ${deleted.contactData.name}`);
      return true;
    } catch (err) {
      console.error(`[RecycleBinService] 恢复联系人失败: ${backupId}`, err);
      return false;
    }
  }

  /**
   * 永久删除（从回收站移除）
   */
  static async removeDeletedContact(backupId: string): Promise<void> {
    const storageKey = `${this.STORAGE_KEY_PREFIX}${backupId}`;
    await this.storageService.delete(storageKey);
    await this.removeFromDeletedList(backupId);
    console.info(`[RecycleBinService] 已永久删除备份: ${backupId}`);
  }

  /**
   * 清空回收站（永久删除所有）
   */
  static async clearRecycleBin(): Promise<void> {
    const ids = await this.getAllDeletedIds();
    
    for (const id of ids) {
      const storageKey = `${this.STORAGE_KEY_PREFIX}${id}`;
      await this.storageService.delete(storageKey);
    }
    
    await this.storageService.delete(this.ALL_DELETED_KEY);
    console.info('[RecycleBinService] 已清空回收站');
  }

  /**
   * 清理过期的删除记录
   */
  static async cleanExpiredContacts(): Promise<number> {
    const all = await this.getAllDeletedContacts();
    const expired = all.filter(item => item.isExpired());
    
    let cleanedCount = 0;
    for (const item of expired) {
      await this.removeDeletedContact(item.backupId);
      cleanedCount++;
    }

    if (cleanedCount > 0) {
      console.info(`[RecycleBinService] 清理了 ${cleanedCount} 条过期记录`);
    }

    return cleanedCount;
  }

  /**
   * 获取回收站统计信息
   */
  static async getStatistics(): Promise<{
    total: number;
    valid: number;
    expired: number;
  }> {
    const all = await this.getAllDeletedContacts();
    const valid = all.filter(item => !item.isExpired());
    const expired = all.filter(item => item.isExpired());

    return {
      total: all.length,
      valid: valid.length,
      expired: expired.length
    };
  }

  // ========== 内部工具方法 ==========

  /**
   * 添加到删除列表
   */
  private static async addToDeletedList(backupId: string): Promise<void> {
    const ids = await this.getAllDeletedIds();
    if (!ids.includes(backupId)) {
      ids.push(backupId);
      await this.storageService.put(this.ALL_DELETED_KEY, JSON.stringify(ids));
    }
  }

  /**
   * 从删除列表中移除
   */
  private static async removeFromDeletedList(backupId: string): Promise<void> {
    const ids = await this.getAllDeletedIds();
    const newIds = ids.filter(id => id !== backupId);
    await this.storageService.put(this.ALL_DELETED_KEY, JSON.stringify(newIds));
  }

  /**
   * 获取所有删除ID
   */
  private static async getAllDeletedIds(): Promise<string[]> {
    const jsonStr = await this.storageService.getString(this.ALL_DELETED_KEY, '[]');
    try {
      return JSON.parse(jsonStr) as string[];
    } catch (err) {
      console.error('[RecycleBinService] 解析删除ID列表失败', err);
      return [];
    }
  }
}
