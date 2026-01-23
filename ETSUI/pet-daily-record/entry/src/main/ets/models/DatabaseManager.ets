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
import relationalStore from '@ohos.data.relationalStore';
import common from '@ohos.app.ability.common';
import { Pet, PetCost, PetRecord } from '../models/PetModel';

// 1. 宠物基础信息表
const SQL_CREATE_PET = `
  CREATE TABLE IF NOT EXISTS PetTable (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    species TEXT,
    gender TEXT,
    birthday TEXT,
    photoPath TEXT,
     userId TEXT
  )`;

// 2. 日常记录表 (含外键逻辑说明)
const SQL_CREATE_RECORD = `
  CREATE TABLE IF NOT EXISTS RecordTable (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    petId INTEGER,
    type TEXT,
    date TEXT,
    content TEXT,
     userId TEXT
  )`;

// 3. 花销统计表
const SQL_CREATE_COST = `
  CREATE TABLE IF NOT EXISTS CostTable (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    petId INTEGER,
    amount REAL,
    category TEXT,
    date TEXT,
     userId TEXT
  )`;


export class DatabaseManager {
  private static instance: DatabaseManager;
  private rdbStore: relationalStore.RdbStore | null = null;
  // 数据库配置
  private readonly STORE_CONFIG: relationalStore.StoreConfig = {
    name: 'PetDiary.db', // 数据库文件名
    securityLevel: relationalStore.SecurityLevel.S1
  };

  public static getInstance(): DatabaseManager {
    if (!DatabaseManager.instance) {
      DatabaseManager.instance = new DatabaseManager();
    }
    return DatabaseManager.instance;
  }

  /**
   * 初始化数据库并创建所有表
   */
  async initDb(context: common.Context) {
    if (this.rdbStore !== null) {
      return;
    }

    try {
      this.rdbStore = await relationalStore.getRdbStore(context, this.STORE_CONFIG);

      // 执行建表语句
      await this.rdbStore.executeSql(SQL_CREATE_PET);
      await this.rdbStore.executeSql(SQL_CREATE_RECORD);
      await this.rdbStore.executeSql(SQL_CREATE_COST);

      console.info('PetDiary: 数据库及所有表初始化成功');
    } catch (err) {
      console.error('PetDiary: 数据库初始化失败', JSON.stringify(err));
    }
  }

  /**
   * 通用插入方法：宠物档案
   */
  async insertPet(name: string, species: string, birthday: string, gender: string, photoPath: string | null) {
    const valueBucket: relationalStore.ValuesBucket = {
      'name': name,
      'species': species,
      'birthday': birthday,
      'gender': gender,
      'photoPath': photoPath,
      'userId': this.getActiveUser() // 【核心改动】自动绑定当前用户
    };
    return await this.rdbStore?.insert('PetTable', valueBucket);
  }

  async deletePet(id: number) {
    if (!this.rdbStore) {
      console.error('数据库未初始化');
      return -1;
    }
    const activeUserId = this.getActiveUser();
    let predicates = new relationalStore.RdbPredicates('PetTable');

    predicates.equalTo('id', id);
    predicates.equalTo('userId', activeUserId);

    try {
      const rows = await this.rdbStore.delete(predicates);
      console.info(`成功删除宠物，ID: ${id}, 受影响行数: ${rows}`);
      return rows;
    } catch (err) {
      console.error(`删除失败: ${JSON.stringify(err)}`);
      return -1;
    }
  }

  async updatePet(id: number, name: string, species: string, birthday: string, gender: string,
    photoPath: string | null) {
    if (!this.rdbStore) {
      console.error('数据库未初始化');
      return -1;
    }
    const activeUserId = this.getActiveUser();
    // 1. 准备要更新的数据
    const valueBucket: relationalStore.ValuesBucket = {
      'name': name,
      'species': species,
      'birthday': birthday,
      'gender': gender,
      'photoPath': photoPath
    };

    // 2. 创建谓词来指定更新哪一条数据
    let predicates = new relationalStore.RdbPredicates('PetTable');
    predicates.equalTo('id', id);
    predicates.equalTo('userId', activeUserId); // 【关键】只能更新自己的宠物

    try {
      // 3. 正确调用 update：第一个参数是数据，第二个是条件
      const rows = await this.rdbStore.update(valueBucket, predicates);
      console.info(`成功更新宠物，ID: ${id}, 受影响行数: ${rows}`);
      return rows;
    } catch (err) {
      console.error(`更新失败: ${JSON.stringify(err)}`);
      return -1;
    }
  }

  /**
   * @description 获取指定宠物的最近历史记录
   * 需求：仅展示最近 5 条
   * @param targetPetId 宠物唯一标识符
   * @returns 异步返回宠物记录数组
   */
  async getRecentRecordsByPetId(targetPetId: number): Promise<PetRecord[]> {
    console.info('[Database] 正在执行限额查询：目标 PetID = ' + targetPetId);

    if (!this.rdbStore) {
      console.error('[Database] 数据库实例尚未初始化');
      return [];
    }

    // 1. 创建查询谓词
    let predicates = new relationalStore.RdbPredicates('RecordTable');

    // 2. 注入筛选条件：指定宠物 ID
    predicates.equalTo('petId', targetPetId);

    // 3. 排序逻辑：日期降序，ID 降序
    predicates.orderByDesc('date');
    predicates.orderByDesc('id');

    // 4. 关键逻辑：限制仅返回最近的 5 条记录
    // 增加此方法以满足“只展示最近5条”的需求
    predicates.limitAs(5);

    // 5. 执行异步查询
    let resultSet = await this.rdbStore.query(predicates);
    let results: PetRecord[] = [];

    try {
      if (resultSet && resultSet.rowCount > 0) {
        console.info(`[Database] 成功命中记录，共计 ${resultSet.rowCount} 条`);

        // 6. 遍历结果集并封装对象
        while (resultSet.goToNextRow()) {
          const recordItem: PetRecord = {
            id: resultSet.getLong(resultSet.getColumnIndex('id')),
            petId: resultSet.getLong(resultSet.getColumnIndex('petId')),
            type: resultSet.getString(resultSet.getColumnIndex('type')),
            date: resultSet.getString(resultSet.getColumnIndex('date')),
            content: resultSet.getString(resultSet.getColumnIndex('content'))
          };
          results.push(recordItem);
        }
      }
    } catch (error) {
      console.error('[Database] 查询过程发生异常: ' + JSON.stringify(error));
    } finally {
      // 7. 必须手动关闭结果集以释放系统资源
      if (resultSet) {
        resultSet.close();
      }
    }

    return results;
  }

  /**
   * @description 核心查询方法：支持多维度条件筛选并限制返回数量
   * 业务逻辑：为了详情页的性能与排版，仅提取最近的 5 笔消费记录
   * @param petId 宠物唯一标识符，若为 -1 则检索全局
   * @returns 异步返回结果集 ResultSet 或 undefined
   */
  async getRecentCosts(petId: number): Promise<PetCost[]> {
    console.info(`[Database] 开始执行花销筛选查询. PetID: ${petId}`);

    // 1. 结果容器
    let results: PetCost[] = [];

    if (!this.rdbStore) {
      console.warn('[Database] 数据库尚未就绪，查询指令已拦截');
      return results;
    }

    let predicates = new relationalStore.RdbPredicates('CostTable');
    if (petId !== -1) {
      predicates.equalTo('petId', petId);
    }
    predicates.orderByDesc('date').limitAs(5);

    let resultSet: relationalStore.ResultSet | undefined = undefined;

    try {
      resultSet = await this.rdbStore.query(predicates);

      if (resultSet && resultSet.rowCount > 0) {
        console.info(`[Database] 查询完成，记录命中总数: ${resultSet.rowCount}`);

        // 5. 核心逻辑：将 ResultSet 转换为对象数组
        while (resultSet.goToNextRow()) {
          const item: PetCost = {
            id: resultSet.getLong(resultSet.getColumnIndex('id')),
            petId: resultSet.getLong(resultSet.getColumnIndex('petId')),
            amount: resultSet.getDouble(resultSet.getColumnIndex('amount')),
            category: resultSet.getString(resultSet.getColumnIndex('category')),
            date: resultSet.getString(resultSet.getColumnIndex('date'))
          };
          results.push(item);
        }
      }
    } catch (error) {
      console.error(`[Database] 查询花销发生异常: ${JSON.stringify(error)}`);
    } finally {
      // 6. 关键：释放数据库游标资源
      if (resultSet) {
        resultSet.close();
      }
    }
    return results;
  }

  /**
   * 通用插入方法：记账
   */
  async insertCost(petId: number, amount: number, category: string, date: string) {
    const valueBucket: relationalStore.ValuesBucket = {
      'petId': petId,
      'amount': amount,
      'category': category,
      'date': date,
      'userId': this.getActiveUser() // 【核心改动】自动绑定当前用户
    };
    return await this.rdbStore?.insert('CostTable', valueBucket);
  }

  /**
   * 查询宠物列表
   */
  async queryAllPets() {
    let predicates = new relationalStore.RdbPredicates('PetTable');
    predicates.equalTo('userId', this.getActiveUser());
    let resultSet = await this.rdbStore?.query(predicates);
    return resultSet; // 注意：resultSet 需要手动解析，建议封装一个解析逻辑
  }

  /**
   * 按日期查询记录 (给组员C用)
   */
  async queryRecordsByDate(date: string) {
    let predicates = new relationalStore.RdbPredicates('RecordTable');

    predicates.equalTo('date', date);
    predicates.equalTo('userId', this.getActiveUser());
    return await this.rdbStore?.query(predicates);
  }

  /**
   * 功能：存入一条宠物行为记录
   * 对应 CalendarPage 的 "记录一笔" 按钮
   */
  async insertRecord(petId: number, type: string, content: string, date: string) {
    const valueBucket: relationalStore.ValuesBucket = {
      'petId': petId,
      'type': type,
      'content': content,
      'date': date,
      'userId': this.getActiveUser() // 【核心改动】自动绑定当前用户
    };
    // 插入到我们之前建好的 RecordTable 表
    return await this.rdbStore?.insert('RecordTable', valueBucket);
  }

  /**
   * 功能：根据日期查询当天的所有行为
   * 对应 CalendarPage 的 "refreshLogs" 方法
   */
  async getRecordsByDate(date: string): Promise<PetRecord[]> {
    const activeUserId = this.getActiveUser();
    let predicates = new relationalStore.RdbPredicates('RecordTable');

    predicates.equalTo('date', date); // 核心逻辑：SQL里的 WHERE date = ?
    predicates.equalTo('userId', activeUserId);

    let resultSet = await this.rdbStore?.query(predicates);
    let results: PetRecord[] = [];

    if (resultSet && resultSet.rowCount > 0) {
      while (resultSet.goToNextRow()) {
        results.push({
          id: resultSet.getLong(resultSet.getColumnIndex('id')),
          petId: resultSet.getLong(resultSet.getColumnIndex('petId')),
          type: resultSet.getString(resultSet.getColumnIndex('type')),
          date: resultSet.getString(resultSet.getColumnIndex('date')),
          content: resultSet.getString(resultSet.getColumnIndex('content'))
        });
      }
      resultSet.close();
    }
    return results;
  }

  /**
   * 需求升级：查询所有历史记录，并按日期从新到旧排序
   */
  async getAllRecords(): Promise<PetRecord[]> {
    let predicates = new relationalStore.RdbPredicates('RecordTable');
    const activeUserId = this.getActiveUser();
    predicates.equalTo('userId', activeUserId);
    // 降序：最新日期排在最前
    predicates.orderByDesc('date');
    // 如果日期相同，可以再按 id 降序，保证最后入库的在上面
    predicates.orderByDesc('id');

    let resultSet = await this.rdbStore?.query(predicates);
    let results: PetRecord[] = [];

    if (resultSet && resultSet.rowCount > 0) {
      while (resultSet.goToNextRow()) {
        results.push({
          id: resultSet.getLong(resultSet.getColumnIndex('id')),
          petId: resultSet.getLong(resultSet.getColumnIndex('petId')),
          type: resultSet.getString(resultSet.getColumnIndex('type')),
          date: resultSet.getString(resultSet.getColumnIndex('date')),
          content: resultSet.getString(resultSet.getColumnIndex('content'))
        });
      }
      resultSet.close();
    }
    return results;
  }

  /**
   * 需求升级：删除指定的行为记录
   * @param recordId 记录的唯一ID
   */
  async deleteRecord(recordId: number): Promise<number> {
    if (!this.rdbStore) {
      return -1;
    }

    let predicates = new relationalStore.RdbPredicates('RecordTable');
    // 关键逻辑：指定要删除哪一行
    predicates.equalTo('id', recordId);
    predicates.equalTo('userId', this.getActiveUser());
    // 执行删除，返回受影响的行数
    let rows = await this.rdbStore.delete(predicates);
    console.info(`PetDiary: 已删除记录ID为 ${recordId} 的数据`);
    return rows;
  }

  /**
   * 增加：获取所有宠物档案列表
   * 用于在日历页面的下拉框中展示宠物名称
   */
  async getAllPets(): Promise<Pet[]> {
    let petList: Pet[] = [];
    if (!this.rdbStore) {
      return petList;
    }
    const activeUserId = this.getActiveUser();
    // 1. 构建查询谓词，指向 PetTable
    let predicates = new relationalStore.RdbPredicates('PetTable');
    predicates.equalTo('userId', activeUserId);
    // 2. 执行查询
    let resultSet = await this.rdbStore.query(predicates);

    // 3. 循环解析结果集，将其转化为 Pet 对象数组
    if (resultSet && resultSet.rowCount > 0) {
      while (resultSet.goToNextRow()) {
        const pet: Pet = {
          id: resultSet.getLong(resultSet.getColumnIndex('id')),
          name: resultSet.getString(resultSet.getColumnIndex('name')),
          species: resultSet.getString(resultSet.getColumnIndex('species')),
          gender: resultSet.getString(resultSet.getColumnIndex('gender')),
          birthday: resultSet.getString(resultSet.getColumnIndex('birthday')),
          age: 0 // 年龄在业务层动态计算
        };
        petList.push(pet);
      }
      // 组长提醒：一定要关闭结果集，防止内存泄漏
      resultSet.close();
    }

    console.info(`PetDiary: 成功从数据库读取了 ${petList.length} 个宠物档案`);
    return petList;
  }

  /**
   * 核心查询方法：支持多条件筛选
   * @param petId 宠物ID，如果为 -1 则查询所有
   * @param category 类别，如果为 '全部' 则查询所有
   */
  async queryCostsFiltered(petId: number, category: string, startDate: string,
    endDate: string): Promise<relationalStore.ResultSet | undefined> {
    if (!this.rdbStore) {
      return undefined;
    }
    const activeUserId = this.getActiveUser();
    let predicates = new relationalStore.RdbPredicates('CostTable');
    predicates.equalTo('userId', activeUserId);
    // 1. 宠物过滤
    if (petId !== -1) {
      predicates.equalTo('petId', petId);
    }
    // 2. 类别过滤
    if (category !== '全部') {
      predicates.equalTo('category', category);
    }
    // 3. 时间区间过滤
    predicates.between('date', startDate, endDate);

    // 4. 按日期降序排列
    predicates.orderByDesc('date');

    return await this.rdbStore.query(predicates);
  }

  // **
  // * from zk
  // * 核心查询方法：支持多条件筛选
  // * @param petId 宠物ID，如果为 -1 则查询所有
  // * @param category 类别，如果为 '全部' 则查询所有
  // * @param startDate 开始日期
  // * @param endDate 结束日期
  // */
  // --- 需新增到 DatabaseManager 类中的代码 ---

  /**
   * 删除消费记录
   */
  async deleteCost(id: number): Promise<number | undefined> {
    if (!this.rdbStore) {
      return undefined;
    }
    let predicates = new relationalStore.RdbPredicates('CostTable');
    predicates.equalTo('id', id);
    predicates.equalTo('userId', this.getActiveUser());
    return await this.rdbStore.delete(predicates);
  }

  /**
   * 更新消费记录（修改功能用）
   */
  async updateCost(cost: PetCost): Promise<number | undefined> {
    if (!this.rdbStore) {
      return undefined;
    }
    const valueBucket: relationalStore.ValuesBucket = {
      'petId': cost.petId,
      'amount': cost.amount,
      'category': cost.category,
      'date': cost.date
    };
    let predicates = new relationalStore.RdbPredicates('CostTable');
    predicates.equalTo('id', cost.id);
    predicates.equalTo('userId', this.getActiveUser());
    return await this.rdbStore.update(valueBucket, predicates);
  }

  private getActiveUser(): string {
    const user: string | undefined = AppStorage.get('currentUserName');
    console.info('PetDiary DB_CHECK: 当前内存中的用户身份为 -> [' + user + ']');
    // 防御性编程：如果没有登录，给一个默认值或抛出警告
    if (user === undefined || user === '') {
      console.warn('PetDiary: 警告！当前处于未登录状态操作数据库，默认指向系统账号 admin');
      return 'admin';
    }

    return user;
  }
}

