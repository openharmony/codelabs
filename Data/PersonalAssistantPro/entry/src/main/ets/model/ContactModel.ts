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

  constructor(name: string, phone: string, relation: string = '朋友', email: string = '') {
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
      this.logger.info(`✅ Insert Success, ID: ${id}`);
      return id;
    } catch (err) {
      this.logger.error('Insert failed', err);
      return -1;
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
   * ✅ [新增] 搜索功能
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
    let contacts: Contact[] = [];

    // 确保 resultSet 有效
    if (!resultSet) {
      return contacts;
    }

    try {
      this.logger.info(`=== [Parsing ResultSet] Found ${resultSet.rowCount} records ===`);

      while (resultSet.goToNextRow()) {
        const id = resultSet.getLong(resultSet.getColumnIndex('id'));
        const name = resultSet.getString(resultSet.getColumnIndex('name'));
        const dbPhone = resultSet.getString(resultSet.getColumnIndex('phone'));

        // 🔓 [解密] 修复：直接调用静态方法，去掉 getInstance()
        const decryptedPhone = await AesCryptoUtils.decrypt(dbPhone);

        let c = new Contact(
          name,
          decryptedPhone,
          resultSet.getString(resultSet.getColumnIndex('relation')),
          resultSet.getString(resultSet.getColumnIndex('email'))
        );
        c.id = id;
        contacts.push(c);
      }
    } catch (e) {
      this.logger.error('Parse resultSet error', e);
    } finally {
      // 务必关闭 resultSet
      resultSet.close();
    }

    return contacts;
  }
}