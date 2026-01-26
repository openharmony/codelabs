/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// src/main/ets/model/ContactModel.ts
import { RdbHelper, TABLE_CONTACT } from '../common/database/RdbHelper';
import relationalStore from '@ohos.data.relationalStore';
import { Logger } from '../common/utils/Logger';
import { AesCryptoUtils } from '../common/utils/AesCryptoUtils';

// Contact 类定义
export class Contact {
  id: number = 0;
  name: string;
  phone: string;
  relation: string;
  email: string;

  // 📝 [修改] .ts 文件中无法直接使用 $r，这里将默认值改为空字符串。
  // 请在创建 Contact 实例的 UI 层（如 Dialog）传入 $r('app.string.contact_default_relation') 解析后的值
  constructor(name: string, phone: string, relation: string = '', email: string = '') {
    this.name = name;
    this.phone = phone;
    this.relation = relation;
    this.email = email;
  }
}

export class ContactModel {
  private static logger = new Logger('ContactModel');

  /**
   * 插入
   */
  static async insert(contact: Contact): Promise<number> {
    try {
      this.logger.info(`=== [Insert Start] ===`);
      // 🔒 [加密] 修复：直接调用静态方法，去掉 getInstance()
      const encryptedPhone = await AesCryptoUtils.encrypt(contact.phone);

      const values: relationalStore.ValuesBucket = {
        'name': contact.name,
        'phone': encryptedPhone, // 存入密文
        'email': contact.email,
        'relation': contact.relation
      };

      const id = await RdbHelper.getInstance().insert(TABLE_CONTACT, values);
      this.logger.info(` Insert Success, ID: ${id}`);
      return id;
    } catch (err) {
      this.logger.error('Insert failed', err);
      return -1;
    }
  }
  static async update(contact: Contact): Promise<boolean> {
    try {
      this.logger.info(`=== [Update Start] ID: ${contact.id} ===`);

      // 1. 同样需要加密电话号码
      const encryptedPhone = await AesCryptoUtils.encrypt(contact.phone);

      const values: relationalStore.ValuesBucket = {
        'name': contact.name,
        'phone': encryptedPhone, // 存入密文
        'email': contact.email,
        'relation': contact.relation
      };

      // 2. 构造查询条件：根据 ID 更新
      let predicates = new relationalStore.RdbPredicates(TABLE_CONTACT);
      predicates.equalTo('id', contact.id);

      const rows = await RdbHelper.getInstance().update(values, predicates);
      this.logger.info(` Update Success, Rows affected: ${rows}`);

      return rows > 0;
    } catch (err) {
      this.logger.error('Update failed', err);
      return false;
    }
  }

  /**
   * 查询所有
   */
  static async queryAll(): Promise<Contact[]> {
    let predicates = new relationalStore.RdbPredicates(TABLE_CONTACT);
    predicates.orderByDesc('id');

    try {
      const resultSet = await RdbHelper.getInstance().query(predicates);
      return await this.resultSetToContacts(resultSet);
    } catch (err) {
      this.logger.error('Query failed', err);
      return [];
    }
  }

  /**
   * [新增] 搜索功能
   * 注意：由于 phone 是加密存储的，SQL LIKE 无法直接搜索手机号。
   * 这里只搜索：姓名、邮箱、关系。
   */
  static async search(keyword: string): Promise<Contact[]> {
    try {
      // 在 name, email, relation 中进行模糊匹配
      const resultSet = await RdbHelper.getInstance().queryByLike(
        TABLE_CONTACT,
        ['name', 'email', 'relation'],
        keyword
      );
      return await this.resultSetToContacts(resultSet);
    } catch (err) {
      this.logger.error('Search failed', err);
      return [];
    }
  }

  /**
   * 删除
   */
  static async deleteById(id: number): Promise<boolean> {
    let predicates = new relationalStore.RdbPredicates(TABLE_CONTACT);
    predicates.equalTo('id', id);
    try {
      const rows = await RdbHelper.getInstance().delete(predicates);
      return rows > 0;
    } catch (err) {
      this.logger.error('Delete failed', err);
      return false;
    }
  }

  /**
   * 🔧 [内部工具] 将 ResultSet 解析为 Contact 数组 (含解密逻辑)
   */
  private static async resultSetToContacts(resultSet: relationalStore.ResultSet): Promise<Contact[]> {
    // 1. 定义临时接口存储原始数据库数据
    interface RawContact {
      id: number;
      name: string;
      encryptedPhone: string;
      relation: string;
      email: string;
    }

    let rawData: RawContact[] = [];

    // 确保 resultSet 有效
    if (!resultSet) {
      return [];
    }

    try {
      this.logger.info(`=== [Parsing ResultSet] Found ${resultSet.rowCount} records ===`);

      // 步骤 A: 快速从数据库读取原始数据 (密文)
      while (resultSet.goToNextRow()) {
        rawData.push({
          id: resultSet.getLong(resultSet.getColumnIndex('id')),
          name: resultSet.getString(resultSet.getColumnIndex('name')),
          encryptedPhone: resultSet.getString(resultSet.getColumnIndex('phone')), // 这里读到的是密文
          relation: resultSet.getString(resultSet.getColumnIndex('relation')),
          email: resultSet.getString(resultSet.getColumnIndex('email'))
        });
      }
    } catch (e) {
      this.logger.error('Parse resultSet error', e);
    } finally {
      // 步骤 B: 立即关闭结果集，释放数据库资源
      resultSet.close();
    }

    // 步骤 C: 在内存中进行解密 (并发处理，提升性能)
    const contacts: Contact[] = await Promise.all(rawData.map(async (item) => {
      let finalPhone = item.encryptedPhone;
      try {
        // 尝试解密
        finalPhone = await AesCryptoUtils.decrypt(item.encryptedPhone);
      } catch (decryptErr) {
        // 🚨 关键：如果解密失败（比如密钥变了），这里要捕获，防止整个列表崩溃
        // 此时 finalPhone 保持为密文，或者你可以设置为 "解密失败"
        ContactModel.logger.error(`Decrypt failed for ID ${item.id}`, decryptErr);
      }

      let contact = new Contact(item.name, finalPhone, item.relation, item.email);
      contact.id = item.id;
      return contact;
    }));

    return contacts;
  }
}