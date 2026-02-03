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

import relationalStore from '@ohos.data.relationalStore'
import common from '@ohos.app.ability.common'
import { Contact } from '../model/Contact'

/**
 * 数据库配置
 */
const STORE_CONFIG: relationalStore.StoreConfig = {
  name: 'MedicineBox.db',
  securityLevel: relationalStore.SecurityLevel.S1
}

/**
 * 建表 SQL
 */
const SQL_CREATE_TABLE = `
CREATE TABLE IF NOT EXISTS CONTACT_TABLE_V2 (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL,
  phone TEXT,
  company TEXT,
  position TEXT,
  email TEXT,
  image TEXT,
  userId INTEGER
)`

/**
 * RdbStore 数据访问层（增强版）
 */
export class RdbStore {
  private rdbStore: relationalStore.RdbStore | null = null
  private readonly tableName: string = 'CONTACT_TABLE_V2'

  /* ==================== 初始化 ==================== */

  async getRdbStore(context: common.Context): Promise<relationalStore.RdbStore> {
    if (this.rdbStore !== null) {
      return this.rdbStore
    }

    return new Promise((resolve, reject) => {
      relationalStore.getRdbStore(context, STORE_CONFIG, (err, store) => {
        if (err) {
          reject(err)
          return
        }

        this.rdbStore = store
        try {
          this.rdbStore.executeSql(SQL_CREATE_TABLE)
        } catch (e) {
          console.error('[RdbStore] executeSql failed')
        }

        resolve(store)
      })
    })
  }

  /* ==================== 基础 CRUD（原样保留） ==================== */

  async insert(contact: Contact): Promise<number> {
    if (!this.rdbStore) {
      return -1
    }

    const values: relationalStore.ValuesBucket = {
      name: contact.name,
      phone: contact.phone,
      company: contact.company ?? '',
      position: contact.position ?? '',
      email: contact.email ?? '',
      image: contact.image ?? '',
      userId: contact.userId ?? 0
    }

    return this.rdbStore.insert(this.tableName, values)
  }

  async update(contact: Contact): Promise<number> {
    if (!this.rdbStore || contact.id === undefined) {
      return -1
    }

    const values: relationalStore.ValuesBucket = {
      name: contact.name,
      phone: contact.phone,
      company: contact.company ?? '',
      position: contact.position ?? '',
      email: contact.email ?? '',
      image: contact.image ?? ''
    }

    const predicates = new relationalStore.RdbPredicates(this.tableName)
    predicates.equalTo('id', contact.id)

    return this.rdbStore.update(values, predicates)
  }

  async delete(id: number): Promise<number> {
    if (!this.rdbStore) {
      return -1
    }

    const predicates = new relationalStore.RdbPredicates(this.tableName)
    predicates.equalTo('id', id)

    return this.rdbStore.delete(predicates)
  }

  async queryAll(userId: number): Promise<Contact[]> {
    if (!this.rdbStore) {
      return []
    }

    const predicates = new relationalStore.RdbPredicates(this.tableName)
    predicates.equalTo('userId', userId)

    return this.queryByPredicates(predicates)
  }

  async search(keyword: string, userId: number): Promise<Contact[]> {
    if (!this.rdbStore) {
      return []
    }

    const predicates = new relationalStore.RdbPredicates(this.tableName)
    predicates.equalTo('userId', userId).and().contains('name', keyword)

    return this.queryByPredicates(predicates)
  }

  /* ==================== 🔥 新增：分类查询 ==================== */

  async queryByCategory(category: string, userId: number): Promise<Contact[]> {
    if (!this.rdbStore) {
      return []
    }

    const predicates = new relationalStore.RdbPredicates(this.tableName)
    predicates
      .equalTo('userId', userId)
      .and()
      .equalTo('company', category)

    return this.queryByPredicates(predicates)
  }

  /* ==================== 🔥 新增：过期判断工具 ==================== */

  async queryExpired(userId: number): Promise<Contact[]> {
    const all = await this.queryAll(userId)
    return all.filter(item => this.isExpired(item.phone))
  }

  async queryNearExpired(userId: number, days: number = 30): Promise<Contact[]> {
    const all = await this.queryAll(userId)
    return all.filter(item => this.isNearExpired(item.phone, days))
  }

  async countAll(userId: number): Promise<number> {
    const all = await this.queryAll(userId)
    return all.length
  }

  /* ==================== 🔥 新增：临期 / 过期查询 ==================== */

  async countNearAndExpired(userId: number): Promise<number> {
    const all = await this.queryAll(userId)
    return all.filter(
      item =>
      this.isExpired(item.phone) ||
      this.isNearExpired(item.phone, 30)
    ).length
  }

  private parseDate(dateStr: string): Date | null {
    const parts = dateStr.split('-')
    if (parts.length !== 3) {
      return null
    }

    const year = Number(parts[0])
    const month = Number(parts[1]) - 1
    const day = Number(parts[2])

    return new Date(year, month, day)
  }

  /* ==================== 🔥 新增：首页统计 ==================== */

  private isExpired(phone: string): boolean {
    const date = this.parseDate(phone)
    if (!date) {
      return false
    }
    return date.getTime() < Date.now()
  }

  private isNearExpired(phone: string, days: number): boolean {
    const date = this.parseDate(phone)
    if (!date) {
      return false
    }

    const diff = date.getTime() - Date.now()
    const diffDays = diff / (1000 * 60 * 60 * 24)
    return diffDays >= 0 && diffDays <= days
  }

  /* ==================== 内部通用查询 ==================== */

  private async queryByPredicates(
    predicates: relationalStore.RdbPredicates
  ): Promise<Contact[]> {
    if (!this.rdbStore) {
      return []
    }

    return new Promise((resolve, reject) => {
      this.rdbStore!.query(
        predicates,
        ['id', 'name', 'phone', 'company', 'position', 'email', 'image', 'userId'],
        (err, resultSet) => {
          if (err) {
            reject(err)
            return
          }

          const result: Contact[] = []

          if (resultSet.rowCount > 0) {
            resultSet.goToFirstRow()
            do {
              const contact = new Contact(
                resultSet.getString(resultSet.getColumnIndex('name')),
                resultSet.getString(resultSet.getColumnIndex('phone'))
              )

              contact.id = resultSet.getLong(resultSet.getColumnIndex('id'))
              contact.company = resultSet.getString(resultSet.getColumnIndex('company'))
              contact.position = resultSet.getString(resultSet.getColumnIndex('position'))
              contact.email = resultSet.getString(resultSet.getColumnIndex('email'))
              contact.image = resultSet.getString(resultSet.getColumnIndex('image'))
              contact.userId = resultSet.getLong(resultSet.getColumnIndex('userId'))

              result.push(contact)
            } while (resultSet.goToNextRow())
          }

          resultSet.close()
          resolve(result)
        }
      )
    })
  }
}

/**
 * 单例导出（所有页面统一使用）
 */
export default new RdbStore()
