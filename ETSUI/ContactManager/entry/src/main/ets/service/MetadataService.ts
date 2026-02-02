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
import { ContactMetadata, ImportanceLevel } from '../model/ContactMetadata';
import { StorageService } from './StorageService';
import { Contact } from '../model/Contact';
import { ContactActivity, ActivityType } from '../model/ContactActivity';

/**
 * 联系人元数据管理服务
 * 负责管理联系人的扩展属性（重要性、活动记录等）
 */
export class MetadataService {
  private static readonly METADATA_PREFIX = 'metadata_';
  private static readonly ALL_METADATA_KEY = 'all_metadata_keys';
  private static storageService = StorageService.getInstance();

  /**
   * 初始化服务
   */
  static async init(): Promise<void> {
    await this.storageService.init();
  }

  /**
   * 生成存储键
   */
  private static getStorageKey(contactKey: string): string {
    return `${this.METADATA_PREFIX}${contactKey}`;
  }

  /**
   * 获取联系人元数据
   */
  static async getMetadata(contactKey: string): Promise<ContactMetadata> {
    const storageKey = this.getStorageKey(contactKey);
    const jsonData = await this.storageService.getObject<Record<string, string | number>>(
      storageKey,
      {}
    );

    // 如果没有数据，创建新的元数据
    if (!jsonData || Object.keys(jsonData).length === 0) {
      const newMetadata = new ContactMetadata(contactKey);
      await this.saveMetadata(newMetadata);
      return newMetadata;
    }

    return ContactMetadata.fromJSON(jsonData);
  }

  /**
   * 保存联系人元数据
   */
  static async saveMetadata(metadata: ContactMetadata): Promise<void> {
    const storageKey = this.getStorageKey(metadata.contactKey);
    await this.storageService.put(storageKey, metadata.toJSON());

    // 更新键列表
    await this.addToKeyList(metadata.contactKey);
  }

  /**
   * 删除联系人元数据
   */
  static async deleteMetadata(contactKey: string): Promise<void> {
    const storageKey = this.getStorageKey(contactKey);
    await this.storageService.delete(storageKey);

    // 从键列表中移除
    await this.removeFromKeyList(contactKey);
  }

  /**
   * 获取所有元数据
   */
  static async getAllMetadata(): Promise<ContactMetadata[]> {
    const keys = await this.getAllContactKeys();
    const metadataList: ContactMetadata[] = [];

    for (const key of keys) {
      try {
        const metadata = await this.getMetadata(key);
        metadataList.push(metadata);
      } catch (err) {
        console.error(`[MetadataService] 获取元数据失败: ${key}`, err);
      }
    }

    return metadataList;
  }

  /**
   * 设置联系人重要性级别
   */
  static async setImportance(contactKey: string, importance: ImportanceLevel): Promise<void> {
    const metadata = await this.getMetadata(contactKey);
    metadata.importance = importance;
    await this.saveMetadata(metadata);
    
    console.info(`[MetadataService] 设置重要性: ${contactKey} -> ${importance}`);
  }

  /**
   * 切换重要性级别（普通 -> 重要 -> 紧急 -> 普通）
   */
  static async toggleImportance(contactKey: string): Promise<ImportanceLevel> {
    const metadata = await this.getMetadata(contactKey);
    
    // 循环切换
    switch (metadata.importance) {
      case ImportanceLevel.NORMAL:
        metadata.importance = ImportanceLevel.IMPORTANT;
        break;
      case ImportanceLevel.IMPORTANT:
        metadata.importance = ImportanceLevel.EMERGENCY;
        break;
      case ImportanceLevel.EMERGENCY:
        metadata.importance = ImportanceLevel.NORMAL;
        break;
      default:
        metadata.importance = ImportanceLevel.NORMAL;
    }

    await this.saveMetadata(metadata);
    return metadata.importance;
  }

  /**
   * 记录查看操作
   */
  static async recordView(contactKey: string): Promise<void> {
    const metadata = await this.getMetadata(contactKey);
    metadata.updateViewRecord();
    await this.saveMetadata(metadata);
  }

  /**
   * 记录编辑操作
   */
  static async recordEdit(contactKey: string): Promise<void> {
    const metadata = await this.getMetadata(contactKey);
    metadata.updateEditRecord();
    await this.saveMetadata(metadata);
  }

  /**
   * 获取重要联系人列表
   */
  static async getImportantContacts(): Promise<string[]> {
    const allMetadata = await this.getAllMetadata();
    return allMetadata
      .filter(m => m.importance === ImportanceLevel.IMPORTANT)
      .map(m => m.contactKey);
  }

  /**
   * 获取紧急联系人列表
   */
  static async getEmergencyContacts(): Promise<string[]> {
    const allMetadata = await this.getAllMetadata();
    return allMetadata
      .filter(m => m.importance === ImportanceLevel.EMERGENCY)
      .map(m => m.contactKey);
  }

  /**
   * 获取所有已标记的联系人（重要+紧急）
   */
  static async getAllMarkedContacts(): Promise<Map<string, ImportanceLevel>> {
    const allMetadata = await this.getAllMetadata();
    const markedMap = new Map<string, ImportanceLevel>();

    allMetadata.forEach(metadata => {
      if (metadata.isImportant()) {
        markedMap.set(metadata.contactKey, metadata.importance);
      }
    });

    return markedMap;
  }

  /**
   * 清理孤立的元数据（联系人已删除但元数据还在）
   */
  static async cleanOrphanMetadata(validContactKeys: string[]): Promise<void> {
    const allKeys = await this.getAllContactKeys();
    const validKeySet = new Set(validContactKeys);
    
    let cleanedCount = 0;
    for (const key of allKeys) {
      if (!validKeySet.has(key)) {
        await this.deleteMetadata(key);
        cleanedCount++;
      }
    }

    if (cleanedCount > 0) {
      console.info(`[MetadataService] 清理了 ${cleanedCount} 条孤立元数据`);
    }
  }

  // ========== 内部工具方法 ==========

  /**
   * 添加到键列表
   */
  private static async addToKeyList(contactKey: string): Promise<void> {
    const keys = await this.getAllContactKeys();
    if (!keys.includes(contactKey)) {
      keys.push(contactKey);
      await this.storageService.put(this.ALL_METADATA_KEY, JSON.stringify(keys));
    }
  }

  /**
   * 从键列表中移除
   */
  private static async removeFromKeyList(contactKey: string): Promise<void> {
    const keys = await this.getAllContactKeys();
    const newKeys = keys.filter(k => k !== contactKey);
    await this.storageService.put(this.ALL_METADATA_KEY, JSON.stringify(newKeys));
  }

  /**
   * 获取所有联系人键
   */
  private static async getAllContactKeys(): Promise<string[]> {
    const jsonStr = await this.storageService.getString(this.ALL_METADATA_KEY, '[]');
    try {
      return JSON.parse(jsonStr) as string[];
    } catch (err) {
      console.error('[MetadataService] 解析键列表失败', err);
      return [];
    }
  }

  /**
   * 获取最近查看的联系人（按时间倒序，最多返回30条）
   */
  static async getRecentViewedContacts(contactMap: Map<string, Contact>): Promise<ContactActivity[]> {
    const allMetadata = await this.getAllMetadata();
    const activities: ContactActivity[] = [];

    allMetadata.forEach(metadata => {
      if (metadata.lastViewTime > 0) {
        const contact = contactMap.get(metadata.contactKey);
        if (contact) {
          activities.push(new ContactActivity(
            contact,
            metadata,
            ActivityType.VIEW,
            metadata.lastViewTime
          ));
        }
      }
    });

    return activities
      .sort((a, b) => b.activityTime - a.activityTime)
      .slice(0, 30);
  }


  /**
   * 获取最近编辑的联系人（按时间倒序，最多返回30条）
   */
  static async getRecentEditedContacts(contactMap: Map<string, Contact>): Promise<ContactActivity[]> {
    const allMetadata = await this.getAllMetadata();
    const activities: ContactActivity[] = [];

    allMetadata.forEach(metadata => {
      if (metadata.lastEditTime > 0) {
        const contact = contactMap.get(metadata.contactKey);
        if (contact) {
          activities.push(new ContactActivity(
            contact,
            metadata,
            ActivityType.EDIT,
            metadata.lastEditTime
          ));
        }
      }
    });

    return activities
      .sort((a, b) => b.activityTime - a.activityTime)
      .slice(0, 30);
  }


  /**
   * 获取最近添加的联系人（按创建时间倒序，最多返回30条）
   */
  static async getRecentCreatedContacts(contactMap: Map<string, Contact>): Promise<ContactActivity[]> {
    const allMetadata = await this.getAllMetadata();
    const activities: ContactActivity[] = [];

    allMetadata.forEach(metadata => {
      if (metadata.createTime > 0) {
        const contact = contactMap.get(metadata.contactKey);
        if (contact) {
          activities.push(new ContactActivity(
            contact,
            metadata,
            ActivityType.CREATE,
            metadata.createTime
          ));
        }
      }
    });

    return activities
      .sort((a, b) => b.activityTime - a.activityTime)
      .slice(0, 30);
  }


  /**
   * 获取所有最近活动（合并查看、编辑、创建，按时间倒序）
   */
  static async getAllRecentActivities(contactMap: Map<string, Contact>): Promise<ContactActivity[]> {
    const [viewed, edited, created] = await Promise.all([
      this.getRecentViewedContacts(contactMap),
      this.getRecentEditedContacts(contactMap),
      this.getRecentCreatedContacts(contactMap)
    ]);

    const allActivities = [...viewed, ...edited, ...created];

    const activityMap: Map<string, ContactActivity> = new Map();
    allActivities.forEach(activity => {
      const key = activity.contact.key;
      const existing = activityMap.get(key);
      if (!existing || activity.activityTime > existing.activityTime) {
        activityMap.set(key, activity);
      }
    });

    return Array.from(activityMap.values())
      .sort((a, b) => b.activityTime - a.activityTime)
      .slice(0, 30);
  }

}
