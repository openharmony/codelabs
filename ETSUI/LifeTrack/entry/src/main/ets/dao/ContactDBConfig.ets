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

import relationalStore from '@ohos.data.relationalStore';
import common from '@ohos.app.ability.common'

// 数据库配置类
export class ContactDBConfig {
  // 数据库名称和配置
  private static readonly DB_NAME: string = 'Contacts.db';
  private static readonly DB_VERSION: number = 1;

  // 数据库配置
  private static readonly STORE_CONFIG: relationalStore.StoreConfig = {
    name: ContactDBConfig.DB_NAME,
    securityLevel: relationalStore.SecurityLevel.S1
  };

  // 创建表的SQL语句
  private static readonly CREATE_TABLE_SQL: string =
    'CREATE TABLE IF NOT EXISTS contacts (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, phone TEXT NOT NULL, initial TEXT NOT NULL)';

  // 创建索引的SQL语句
  private static readonly CREATE_INDEX_SQL: string =
    'CREATE INDEX IF NOT EXISTS idx_name ON contacts(name); CREATE INDEX IF NOT EXISTS idx_initial ON contacts(initial); CREATE INDEX IF NOT EXISTS idx_phone ON contacts(phone);';

  // 数据库实例
  private rdbStore: relationalStore.RdbStore | null = null;

  // 单例模式
  private static instance: ContactDBConfig | null = null;

  private constructor() {}

  public static getInstance(): ContactDBConfig {
    if (!ContactDBConfig.instance) {
      ContactDBConfig.instance = new ContactDBConfig();
    }
    return ContactDBConfig.instance;
  }

  // 初始化数据库（现在需要传递上下文）
  public async initDB(context: common.Context): Promise<boolean> {
    try {
      console.info('[ContactDBConfig] 开始初始化联系人数据库...');

      // 使用传入的上下文打开或创建数据库
      this.rdbStore = await relationalStore.getRdbStore(
        context,
        ContactDBConfig.STORE_CONFIG
      );

      console.info('[ContactDBConfig] 数据库打开成功');

      // 设置数据库版本
      this.rdbStore.version = ContactDBConfig.DB_VERSION;

      // 创建表
      await this.executeSQL(ContactDBConfig.CREATE_TABLE_SQL);
      console.info('[ContactDBConfig] 联系人表创建成功');

      // 创建索引
      await this.executeSQL(ContactDBConfig.CREATE_INDEX_SQL);
      console.info('[ContactDBConfig] 索引创建成功');

      return true;
    } catch (error) {
      console.error('[ContactDBConfig] 数据库初始化失败: ' + JSON.stringify(error));
      return false;
    }
  }

  // 执行SQL语句
  private async executeSQL(sql: string): Promise<void> {
    if (!this.rdbStore) {
      console.error('[ContactDBConfig] 数据库未初始化');
      return;
    }
    await this.rdbStore.executeSql(sql);
  }

  // 获取数据库实例
  public getRdbStore(): relationalStore.RdbStore | null {
    if (!this.rdbStore) {
      console.error('[ContactDBConfig] 数据库未初始化，请先调用initDB()');
      return null;
    }
    return this.rdbStore;
  }

  // 检查数据库是否已初始化
  public isInitialized(): boolean {
    return this.rdbStore !== null;
  }

  // 数据库事务操作
  public async beginTransaction(): Promise<void> {
    if (this.rdbStore) {
      await this.rdbStore.beginTransaction();
    }
  }

  public async commit(): Promise<void> {
    if (this.rdbStore) {
      await this.rdbStore.commit();
    }
  }

  public async rollback(): Promise<void> {
    if (this.rdbStore) {
      await this.rdbStore.rollback(0);
    }
  }

  // 关闭数据库（可选，通常在应用退出时调用）
  public async closeDB(): Promise<void> {
    if (this.rdbStore) {
      try {
        await this.rdbStore.close();
        console.info('[ContactDBConfig] 数据库关闭成功');
        this.rdbStore = null;
      } catch (error) {
        console.error('[ContactDBConfig] 关闭数据库失败: ' + JSON.stringify(error));
      }
    }
  }
}