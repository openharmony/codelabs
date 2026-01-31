/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
import {
  BatchContactData,
  ContactsItem,
  InsertContactData,
  OperationData,
  OperationResult
} from '../model/ContactModel';
import { ContactDBConfig } from './ContactDBConfig';

// 数据访问对象
export class ContactDao {
  private dbConfig: ContactDBConfig;

  constructor() {
    this.dbConfig = ContactDBConfig.getInstance();
  }

  // 确保数据库已初始化
  private async ensureDBInitialized(): Promise<relationalStore.RdbStore> {
    if (!this.dbConfig.isInitialized()) {
      console.error('[ContactDao] 数据库未初始化，请先调用initDB()方法初始化数据库');
      throw new Error('数据库未初始化');
    }

    const rdbStore = this.dbConfig.getRdbStore();
    if (!rdbStore) {
      throw new Error('数据库实例为空');
    }

    return rdbStore;
  }

  // 生成姓名的首字母（支持中文拼音首字母）
  private generateInitial(name: string): string {
    if (!name || name.trim().length === 0) {
      return '#';
    }

    const firstChar = name.trim().charAt(0);

    // 处理英文字符
    if (/[A-Za-z]/.test(firstChar)) {
      return firstChar.toUpperCase();
    }

    // 处理中文字符
    if (/[\u4e00-\u9fa5]/.test(firstChar)) {
      return this.getChineseInitial(firstChar);
    }

    // 其他字符返回#
    return '#';
  }

  // 获取中文字符的拼音首字母
  private getChineseInitial(char: string): string {
    // 常见中文姓氏拼音首字母映射表
    const pinyinMap: Record<string, string> = {
      '张': 'Z',
      '王': 'W',
      '李': 'L',
      '赵': 'Z',
      '刘': 'L',
      '陈': 'C',
      '杨': 'Y',
      '黄': 'H',
      '周': 'Z',
      '吴': 'W',
      '徐': 'X',
      '孙': 'S',
      '胡': 'H',
      '朱': 'Z',
      '高': 'G',
      '林': 'L',
      '何': 'H',
      '郭': 'G',
      '马': 'M',
      '罗': 'L',
      '梁': 'L',
      '宋': 'S',
      '郑': 'Z',
      '谢': 'X',
      '韩': 'H',
      '唐': 'T',
      '冯': 'F',
      '于': 'Y',
      '董': 'D',
      '萧': 'X',
      '程': 'C',
      '曹': 'C',
      '袁': 'Y',
      '邓': 'D',
      '许': 'X',
      '傅': 'F',
      '沈': 'S',
      '曾': 'Z',
      '彭': 'P',
      '吕': 'L',
      '苏': 'S',
      '卢': 'L',
      '蒋': 'J',
      '蔡': 'C',
      '贾': 'J',
      '丁': 'D',
      '魏': 'W',
      '薛': 'X',
      '叶': 'Y',
      '阎': 'Y',
      '余': 'Y',
      '潘': 'P',
      '杜': 'D',
      '戴': 'D',
      '夏': 'X',
      '钟': 'Z',
      '汪': 'W',
      '田': 'T',
      '任': 'R',
      '姜': 'J',
      '范': 'F',
      '方': 'F',
      '石': 'S',
      '姚': 'Y',
      '谭': 'T',
      '廖': 'L',
      '邹': 'Z',
      '熊': 'X',
      '金': 'J',
      '陆': 'L',
      '郝': 'H',
      '孔': 'K',
      '白': 'B',
      '崔': 'C',
      '康': 'K',
      '毛': 'M',
      '邱': 'Q',
      '秦': 'Q',
      '江': 'J',
      '史': 'S',
      '顾': 'G',
      '侯': 'H',
      '邵': 'S',
      '孟': 'M',
      '龙': 'L',
      '万': 'W',
      '段': 'D',
      '雷': 'L',
      '钱': 'Q',
      '汤': 'T',
      '尹': 'Y',
      '黎': 'L',
      '易': 'Y',
      '常': 'C',
      '武': 'W',
      '乔': 'Q',
      '贺': 'H',
      '赖': 'L',
      '龚': 'G',
      '文': 'W',
      '庞': 'P',
      '樊': 'F',
      '兰': 'L',
      '殷': 'Y',
      '施': 'S',
      '陶': 'T',
      '洪': 'H',
      '翟': 'Z',
      '安': 'A',
      '颜': 'Y',
      '倪': 'N',
      '严': 'Y',
      '牛': 'N',
      '温': 'W',
      '芦': 'L',
      '季': 'J',
      '晏': 'Y',
      '柴': 'C',
      '瞿': 'Q',
      '连': 'L',
      '艾': 'A',
      '阳': 'Y',
      '佟': 'T',
      '俞': 'Y',
      '牟': 'M',
      '宗': 'Z',
      '卞': 'B',
      '邬': 'W',
      '祁': 'Q',
      '寇': 'K',
      '隋': 'S',
      '商': 'S',
      '包': 'B',
      '应': 'Y',
      '宣': 'X',
      '贲': 'B',
      '郁': 'Y',
      '单': 'S',
      '杭': 'H',
      '诸': 'Z',
      '左': 'Z',
      '吉': 'J',
      '钮': 'N',
      '嵇': 'J',
      '邢': 'X',
      '滑': 'H',
      '裴': 'P',
      '荣': 'R',
      '翁': 'W',
      '荀': 'X',
      '羊': 'Y',
      '於': 'Y',
      '惠': 'H',
      '储': 'C',
      '甄': 'Z',
      '麴': 'Q',
      '家': 'J',
      '封': 'F',
      '芮': 'R',
      '羿': 'Y',
      '牧': 'M',
      '靳': 'J',
      '汲': 'J',
      '邴': 'B',
      '糜': 'M',
      '松': 'S',
      '井': 'J',
      '伊': 'Y',
      '富': 'F',
      '巫': 'W',
      '乌': 'W',
      '焦': 'J',
      '巴': 'B',
      '弓': 'G',
      '钭': 'T',
      '隗': 'W',
      '山': 'S',
      '谷': 'G',
      '车': 'C',
      '宓': 'M',
      '蓬': 'P',
      '咸': 'X',
      '全': 'Q',
      '郗': 'X',
      '班': 'B',
      '仰': 'Y',
      '秋': 'Q',
      '仲': 'Z',
      '宰': 'Z',
      '宫': 'G',
      '宁': 'N',
      '仇': 'Q',
      '栾': 'L',
      '暴': 'B',
      '甘': 'G',
      '濮': 'P',
      '厉': 'L',
      '戎': 'R',
      '祖': 'Z',
      '符': 'F',
      '景': 'J',
      '充': 'C',
      '慕': 'M',
      '詹': 'Z',
      '束': 'S',
      '幸': 'X',
      '司': 'S',
      '韶': 'S',
      '耿': 'G',
      '满': 'M',
      '郜': 'G',
      '蓟': 'J',
      '薄': 'B',
      '印': 'Y',
      '宿': 'S',
      '索': 'S',
      '池': 'C',
      '怀': 'H',
      '蒲': 'P',
      '邰': 'T',
      '从': 'C',
      '鄂': 'E',
      '双': 'S',
      '冉': 'R',
      '籍': 'J',
      '卓': 'Z',
      '蔺': 'L',
      '屠': 'T',
      '蒙': 'M',
      '桂': 'G',
      '冀': 'J',
      '阴': 'Y',
      '鬱': 'Y',
      '胥': 'X',
      '能': 'N',
      '苍': 'C',
      '庄': 'Z',
      '向': 'X',
      '闻': 'W',
      '莘': 'S',
      '党': 'D',
      '贡': 'G',
      '劳': 'L',
      '利': 'L',
      '蔚': 'W',
      '逄': 'P',
      '姬': 'J',
      '申': 'S',
      '扶': 'F',
      '堵': 'D',
      '厍': 'S',
      '聂': 'N',
      '郦': 'L',
      '雍': 'Y',
      '郤': 'X',
      '璩': 'Q',
      '桑': 'S',
      '訾': 'Z',
      '辛': 'X',
      '寿': 'S',
      '通': 'T',
      '边': 'B',
      '扈': 'H',
      '燕': 'Y',
      '母': 'M',
      '公': 'G',
      '郏': 'J',
      '浦': 'P',
      '尚': 'S',
      '农': 'N',
      '别': 'B',
      '丰': 'F',
      '巢': 'C',
      '茹': 'R',
      '习': 'X',
      '宦': 'H',
      '鱼': 'Y',
      '容': 'R',
      '荆': 'J',
      '红': 'H',
      '古': 'G',
      '慎': 'S',
      '戈': 'G',
      '庾': 'Y',
      '终': 'Z',
      '益': 'Y',
      '桓': 'H',
      '暨': 'J',
      '居': 'J',
      '衡': 'H',
      '步': 'B',
      '都': 'D',
      '逯': 'L',
      '盖': 'G',
      '弘': 'H',
      '匡': 'K',
      '国': 'G',
      '广': 'G',
      '禄': 'L',
      '竺': 'Z',
      '权': 'Q',
      '阙': 'Q',
      '东': 'D',
      '欧': 'O',
      '殳': 'S',
      '沃': 'W',
      '游': 'Y',
      '后': 'H',
      '越': 'Y',
      '夔': 'K',
      '隆': 'L',
      '师': 'S',
      '巩': 'G',
      '相': 'X',
      '查': 'Z',
      '晁': 'C',
      '勾': 'G',
      '敖': 'A',
      '融': 'R',
      '冷': 'L',
      '关': 'G',
      '蒯': 'K',
      '阚': 'K',
      '那': 'N',
      '简': 'J',
      '饶': 'R',
      '空': 'K',
      '鞠': 'J',
      '须': 'X',
      '沙': 'S',
      '乜': 'N',
      '养': 'Y',
      '休': 'X',
      '华': 'H',
      '呼': 'H',
      '上': 'S',
      '独': 'D',
      '尉': 'W',
      '皇': 'H',
      '端': 'D',
      '南': 'N',
      '宇': 'Y',
      '百': 'B',
      '小': 'X',
      '宝': 'B',
      '神': 'S',
      '凯': 'K',
      '花': 'H',
      '天': 'T',
      '儿': 'E',
      '爸': 'B',
      '妈': 'M',
      '奶': 'N',
      '爷': 'Y',
      '舅': 'J',
      '主': 'Z',
      '女': 'N',
      '弟': 'D',
      '妹': 'M',
      '哥': 'G',
      '姐': 'J',
      '表': 'B',
      '堂': 'T',
      '干': 'G',
    };

    const initial = pinyinMap[char];
    if (initial) {
      return initial;
    }
    return '#';
  }

  // 插入联系人
  public async insertContact(contactData: InsertContactData): Promise<OperationResult> {
    try {
      const rdbStore = await this.ensureDBInitialized();

      // 生成首字母（如果未提供）
      const initial = contactData.initial || this.generateInitial(contactData.name);

      // 创建ValuesBucket
      const valueBucket: relationalStore.ValuesBucket = {
        name: contactData.name,
        phone: contactData.phone,
        initial: initial
      };

      // 插入数据
      const insertId = await rdbStore.insert('contacts', valueBucket);

      console.info('[ContactDao] 插入联系人成功，ID: ' + insertId);

      const data: OperationData = { id: insertId };

      return {
        success: true,
        message: '联系人添加成功',
        data: data
      };
    } catch (error) {
      console.error('[ContactDao] 插入联系人失败: ' + JSON.stringify(error));

      return {
        success: false,
        message: '添加失败: ' + (error.message || '未知错误')
      };
    }
  }

  // 更新联系人
  public async updateContact(contact: ContactsItem): Promise<OperationResult> {
    try {
      const rdbStore = await this.ensureDBInitialized();

      // 生成首字母
      const initial = this.generateInitial(contact.name);

      // 创建更新条件
      const predicates = new relationalStore.RdbPredicates('contacts');
      predicates.equalTo('id', contact.id);

      // 创建ValuesBucket
      const valueBucket: relationalStore.ValuesBucket = {
        name: contact.name,
        phone: contact.phone,
        initial: initial
      };

      // 更新数据
      const rowsUpdated = await rdbStore.update(valueBucket, predicates);

      if (rowsUpdated > 0) {
        console.info('[ContactDao] 更新联系人成功，ID: ' + contact.id);

        const data: OperationData = { rowsUpdated: rowsUpdated };

        return {
          success: true,
          message: '联系人更新成功',
          data: data
        };
      } else {
        return {
          success: false,
          message: '联系人不存在'
        };
      }
    } catch (error) {
      console.error('[ContactDao] 更新联系人失败: ' + JSON.stringify(error));

      return {
        success: false,
        message: '更新失败: ' + (error.message || '未知错误')
      };
    }
  }

  // 删除联系人
  public async deleteContact(id: number): Promise<OperationResult> {
    try {
      const rdbStore = await this.ensureDBInitialized();

      // 创建删除条件
      const predicates = new relationalStore.RdbPredicates('contacts');
      predicates.equalTo('id', id);

      // 删除数据
      const rowsDeleted = await rdbStore.delete(predicates);

      if (rowsDeleted > 0) {
        console.info('[ContactDao] 删除联系人成功，ID: ' + id);

        const data: OperationData = { rowsDeleted: rowsDeleted };

        return {
          success: true,
          message: '联系人删除成功',
          data: data
        };
      } else {
        return {
          success: false,
          message: '联系人不存在'
        };
      }
    } catch (error) {
      console.error('[ContactDao] 删除联系人失败: ' + JSON.stringify(error));

      return {
        success: false,
        message: '删除失败: ' + (error.message || '未知错误')
      };
    }
  }

  // 根据ID查询联系人
  public async getContactById(id: number): Promise<ContactsItem | null> {
    try {
      const rdbStore = await this.ensureDBInitialized();

      // 创建查询条件
      const predicates = new relationalStore.RdbPredicates('contacts');
      predicates.equalTo('id', id);

      // 查询数据
      const resultSet = await rdbStore.query(
        predicates,
        ['id', 'name', 'phone', 'initial']
      );

      // 处理查询结果
      if (resultSet.rowCount > 0) {
        resultSet.goToFirstRow();
        const contact = this.convertResultSetToContact(resultSet);
        resultSet.close();
        return contact;
      }

      resultSet.close();
      return null;
    } catch (error) {
      console.error('[ContactDao] 查询联系人失败: ' + JSON.stringify(error));
      return null;
    }
  }

  // 查询所有联系人（按首字母排序）
  public async getAllContacts(): Promise<ContactsItem[]> {
    try {
      const rdbStore = await this.ensureDBInitialized();

      // 创建查询条件，按首字母和姓名排序
      const predicates = new relationalStore.RdbPredicates('contacts');
      predicates.orderByAsc('initial')
        .orderByAsc('name');

      // 查询数据
      const resultSet = await rdbStore.query(
        predicates,
        ['id', 'name', 'phone', 'initial']
      );

      // 处理查询结果
      const contacts: ContactsItem[] = [];
      while (resultSet.goToNextRow()) {
        const contact = this.convertResultSetToContact(resultSet);
        if (contact) {
          contacts.push(contact);
        }
      }

      resultSet.close();
      return contacts;
    } catch (error) {
      console.error('[ContactDao] 查询所有联系人失败: ' + JSON.stringify(error));
      return [];
    }
  }

  // 搜索联系人（按姓名或电话）
  public async searchContacts(keyword: string): Promise<ContactsItem[]> {
    try {
      const rdbStore = await this.ensureDBInitialized();

      // 创建查询条件
      const predicates = new relationalStore.RdbPredicates('contacts');
      predicates.contains('name', keyword)
        .or()
        .contains('phone', keyword)
        .orderByAsc('initial')
        .orderByAsc('name');

      // 查询数据
      const resultSet = await rdbStore.query(
        predicates,
        ['id', 'name', 'phone', 'initial']
      );

      // 处理查询结果
      const contacts: ContactsItem[] = [];
      while (resultSet.goToNextRow()) {
        const contact = this.convertResultSetToContact(resultSet);
        if (contact) {
          contacts.push(contact);
        }
      }

      resultSet.close();
      return contacts;
    } catch (error) {
      console.error('[ContactDao] 搜索联系人失败: ' + JSON.stringify(error));
      return [];
    }
  }

  // 获取联系人数量
  public async getContactCount(): Promise<number> {
    try {
      const rdbStore = await this.ensureDBInitialized();

      // 创建查询条件
      const predicates = new relationalStore.RdbPredicates('contacts');

      // 查询数量
      const resultSet = await rdbStore.query(
        predicates,
        ['COUNT(id) as count']
      );

      let count = 0;
      if (resultSet.rowCount > 0) {
        resultSet.goToFirstRow();
        count = resultSet.getLong(resultSet.getColumnIndex('count'));
      }

      resultSet.close();
      return count;
    } catch (error) {
      console.error('[ContactDao] 查询联系人数量失败: ' + JSON.stringify(error));
      return 0;
    }
  }

  // 批量插入联系人
  public async batchInsertContacts(contacts: BatchContactData[]): Promise<OperationResult> {
    try {
      const rdbStore = await this.ensureDBInitialized();

      let successCount = 0;
      let failCount = 0;
      const failMessages: string[] = [];

      // 开始事务
      await this.dbConfig.beginTransaction();

      try {
        for (const contact of contacts) {
          try {
            const initial = contact.initial || this.generateInitial(contact.name);
            const valueBucket: relationalStore.ValuesBucket = {
              name: contact.name,
              phone: contact.phone,
              initial: initial
            };

            await rdbStore.insert('contacts', valueBucket);
            successCount++;
          } catch (error) {
            failCount++;
            const errorMsg = `${contact.name}: ${error.message || '未知错误'}`;
            failMessages.push(errorMsg);
            console.error('[ContactDao] 批量插入单个联系人失败: ' + errorMsg);
          }
        }

        // 提交事务
        await this.dbConfig.commit();

        console.info('[ContactDao] 批量插入完成，成功: ' + successCount + ', 失败: ' + failCount);

        const data: OperationData = {
          successCount: successCount,
          failCount: failCount,
          failMessages: failMessages
        };

        return {
          success: true,
          message: '批量插入完成，成功 ' + successCount + ' 条，失败 ' + failCount + ' 条',
          data: data
        };
      } catch (error) {
        // 回滚事务
        await this.dbConfig.rollback();
        console.error('[ContactDao] 批量插入失败，已回滚: ' + JSON.stringify(error));
        throw new Error('批量插入失败: ' + (error.message || '未知错误'));
      }
    } catch (error) {
      console.error('[ContactDao] 批量插入联系人失败: ' + JSON.stringify(error));

      return {
        success: false,
        message: '批量插入失败: ' + (error.message || '未知错误')
      };
    }
  }

  // 清空所有联系人
  public async clearAllContacts(): Promise<OperationResult> {
    try {
      const rdbStore = await this.ensureDBInitialized();

      // 创建删除条件（删除所有）
      const predicates = new relationalStore.RdbPredicates('contacts');

      // 删除数据
      const rowsDeleted = await rdbStore.delete(predicates);

      console.info('[ContactDao] 清空联系人表，删除 ' + rowsDeleted + ' 条记录');

      const data: OperationData = { rowsDeleted: rowsDeleted };

      return {
        success: true,
        message: '已清空所有联系人 (' + rowsDeleted + ' 条)',
        data: data
      };
    } catch (error) {
      console.error('[ContactDao] 清空联系人失败: ' + JSON.stringify(error));

      return {
        success: false,
        message: '清空失败: ' + (error.message || '未知错误')
      };
    }
  }

  // 将ResultSet转换为ContactsItem对象
  private convertResultSetToContact(resultSet: relationalStore.ResultSet): ContactsItem | null {
    try {
      const id = resultSet.getLong(resultSet.getColumnIndex('id'));
      const name = resultSet.getString(resultSet.getColumnIndex('name'));
      const phone = resultSet.getString(resultSet.getColumnIndex('phone'));
      const initial = resultSet.getString(resultSet.getColumnIndex('initial'));

      return {
        id: id,
        name: name,
        phone: phone,
        initial: initial
      };
    } catch (error) {
      console.error('[ContactDao] 转换ResultSet失败: ' + JSON.stringify(error));
      return null;
    }
  }

  // 检查电话号码是否已存在
  public async isPhoneExist(phone: string, excludeId?: number): Promise<boolean> {
    try {
      const rdbStore = await this.ensureDBInitialized();

      // 创建查询条件
      const predicates = new relationalStore.RdbPredicates('contacts');
      predicates.equalTo('phone', phone);

      if (excludeId !== undefined) {
        predicates.notEqualTo('id', excludeId);
      }

      // 查询数据
      const resultSet = await rdbStore.query(
        predicates,
        ['id']
      );

      const exists = resultSet.rowCount > 0;
      resultSet.close();

      return exists;
    } catch (error) {
      console.error('[ContactDao] 检查电话号码失败: ' + JSON.stringify(error));
      return false;
    }
  }
}