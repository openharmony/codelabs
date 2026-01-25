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
import { UserInfo } from '../model/types'; // 导入你定义的 UserInfo 类型

// 数据库错误类型定义（和 ItemRdb 完全一致）
interface RdbError {
  code: number;
  message: string;
}

class UserInfoRdb {
  private rdbStore: relationalStore.RdbStore | null = null;
  private tableName: string = 'USER_INFO_TABLE'; // 用户信息表
  private isMock: boolean = false; // 预览器兼容模式
  private mockData: UserInfo = { // 预览器模拟数据（单用户）
    avatar: '👤',
    nickname: '轻物志用户',
    joinDate: '2026年1月'
  };

  // 彻底删除 id 字段
  private sqlCreate: string = `CREATE TABLE IF NOT EXISTS USER_INFO_TABLE (
    avatar TEXT DEFAULT '👤',
    nickname TEXT DEFAULT '轻物志用户',
    joinDate TEXT NOT NULL
  )`;

  private static instance: UserInfoRdb;
  // 新增：保存上下文，避免重复传递
  private context: common.UIAbilityContext | null = null;

  // 单例模式（保留）
  static getInstance(): UserInfoRdb {
    if (!UserInfoRdb.instance) {
      UserInfoRdb.instance = new UserInfoRdb();
    }
    return UserInfoRdb.instance;
  }

  private constructor() {}

  // 核心修复：初始化数据库（保存上下文，确保只初始化一次）
  async getRdbStore(context: common.UIAbilityContext): Promise<relationalStore.RdbStore | null> {
    // 保存上下文，后续操作无需重复传递
    this.context = context;
    if (this.rdbStore) {
      return this.rdbStore;
    }

    const STORE_CONFIG: relationalStore.StoreConfig = {
      name: 'ItemManager.db', // 和物品数据库共用
      securityLevel: relationalStore.SecurityLevel.S1
    };

    try {
      return new Promise((resolve, reject) => {
        relationalStore.getRdbStore(context, STORE_CONFIG, (err: RdbError | null, store: relationalStore.RdbStore) => {
          if (err) {
            console.warn(`获取用户信息数据库失败: Code=${err.code}, Msg=${err.message}`);
            console.warn('切换到预览器模拟模式');
            this.isMock = true;
            resolve(null);
            return;
          }

          // 创建用户信息表（无 id 字段）
          store.executeSql(this.sqlCreate).then(() => {
            console.info('用户信息数据表创建成功');
            this.rdbStore = store;
            // 初始化默认数据（无 id）
            this.initDefaultUserInfo().then(() => {
              resolve(store);
            });
          }).catch((sqlErr: Error) => {
            console.error(`创建用户信息数据表失败: ${sqlErr.message}`);
            this.isMock = true;
            resolve(store);
          });
        });
      });
    } catch (e) {
      console.warn('用户信息数据库API异常（预览器环境），切换到模拟模式');
      this.isMock = true;
      return null;
    }
  }

  // 初始化默认用户信息（彻底移除 id 逻辑）
  private async initDefaultUserInfo(): Promise<void> {
    if (this.isMock || !this.rdbStore) {
      return;
    }

    // 查询表中是否有数据（无 id 条件）
    const predicates = new relationalStore.RdbPredicates(this.tableName);
    const resultSet = await this.rdbStore.query(predicates);

    if (resultSet.rowCount === 0) {
      // 插入默认值（无 id 字段）
      const defaultInfo: UserInfo = {
        avatar: '👤',
        nickname: '轻物志用户',
        joinDate: new Date().getFullYear() + '年' + (new Date().getMonth() + 1) + '月'
      };
      const valueBucket: relationalStore.ValuesBucket = {
        avatar: defaultInfo.avatar,
        nickname: defaultInfo.nickname,
        joinDate: defaultInfo.joinDate
      };
      await this.rdbStore.insert(this.tableName, valueBucket);
      console.info('默认用户信息初始化成功（无 id）');
    }
    resultSet.close();
  }

  // 检查数据库是否初始化（保留）
  isInitialized(): boolean {
    return this.rdbStore !== null || this.isMock;
  }

  // ========== 核心修复：查询前先初始化数据库 ==========
  async queryUserInfo(): Promise<UserInfo> {
    // 关键：如果未初始化且有上下文，先初始化数据库
    if (!this.rdbStore && !this.isMock && this.context) {
      await this.getRdbStore(this.context);
    }

    if (this.isMock) {
      // 预览器模式：返回模拟数据
      return {
        avatar: this.mockData.avatar,
        nickname: this.mockData.nickname,
        joinDate: this.mockData.joinDate
      };
    }

    if (!this.rdbStore) {
      throw new Error('用户信息数据库未初始化（无有效上下文）');
    }

    // 无 id 条件，直接查询整张表
    const predicates = new relationalStore.RdbPredicates(this.tableName);
    const resultSet = await this.rdbStore.query(predicates);

    let userInfo: UserInfo = {
      avatar: '👤',
      nickname: '轻物志用户',
      joinDate: '2026年1月'
    };

    // 读取第一条数据（单用户仅一条）
    if (resultSet.rowCount > 0) {
      resultSet.goToFirstRow();
      userInfo = {
        avatar: resultSet.getString(resultSet.getColumnIndex('avatar')) || '👤',
        nickname: resultSet.getString(resultSet.getColumnIndex('nickname')) || '轻物志用户',
        joinDate: resultSet.getString(resultSet.getColumnIndex('joinDate')) || '2026年1月'
      };
    }

    resultSet.close();
    return userInfo;
  }

  // ========== 核心修复：更新头像前先初始化数据库 ==========
  async updateAvatar(avatar: string): Promise<number> {
    // 关键：如果未初始化且有上下文，先初始化数据库
    if (!this.rdbStore && !this.isMock && this.context) {
      await this.getRdbStore(this.context);
    }

    if (this.isMock) {
      this.mockData.avatar = avatar;
      return 1; // 更新成功返回1
    }

    if (!this.rdbStore) {
      throw new Error('用户信息数据库未初始化（无有效上下文）');
    }

    const valueBucket: relationalStore.ValuesBucket = {
      avatar: avatar
    };

    // 无 id 条件，直接更新整张表（仅一条数据）
    const predicates = new relationalStore.RdbPredicates(this.tableName);
    return this.rdbStore.update(valueBucket, predicates);
  }

  // ========== 核心修复：更新昵称前先初始化数据库 ==========
  async updateNickname(nickname: string): Promise<number> {
    if (nickname.trim() === '') {
      throw new Error('昵称不能为空');
    }
    if (nickname.length > 12) {
      throw new Error('昵称不能超过12个字符');
    }

    // 关键：如果未初始化且有上下文，先初始化数据库
    if (!this.rdbStore && !this.isMock && this.context) {
      await this.getRdbStore(this.context);
    }

    if (this.isMock) {
      this.mockData.nickname = nickname.trim();
      return 1;
    }

    if (!this.rdbStore) {
      throw new Error('用户信息数据库未初始化（无有效上下文）');
    }

    const valueBucket: relationalStore.ValuesBucket = {
      nickname: nickname.trim()
    };

    // 无 id 条件，直接更新
    const predicates = new relationalStore.RdbPredicates(this.tableName);
    return this.rdbStore.update(valueBucket, predicates);
  }

  // ========== 扩展方法：更新完整用户信息（移除 id） ==========
  async updateUserInfo(userInfo: UserInfo): Promise<number> {
    // 关键：如果未初始化且有上下文，先初始化数据库
    if (!this.rdbStore && !this.isMock && this.context) {
      await this.getRdbStore(this.context);
    }

    if (this.isMock) {
      this.mockData = {
        avatar: userInfo.avatar,
        nickname: userInfo.nickname,
        joinDate: userInfo.joinDate
      };
      return 1;
    }

    if (!this.rdbStore) {
      throw new Error('用户信息数据库未初始化（无有效上下文）');
    }

    const valueBucket: relationalStore.ValuesBucket = {
      avatar: userInfo.avatar,
      nickname: userInfo.nickname.trim(),
      joinDate: userInfo.joinDate
    };

    const predicates = new relationalStore.RdbPredicates(this.tableName);
    return this.rdbStore.update(valueBucket, predicates);
  }

  // ========== 清空用户信息（移除 id 条件） ==========
  async clearAll(): Promise<void> {
    // 关键：如果未初始化且有上下文，先初始化数据库
    if (!this.rdbStore && !this.isMock && this.context) {
      await this.getRdbStore(this.context);
    }

    if (this.isMock) {
      this.mockData = {
        avatar: '👤',
        nickname: '轻物志用户',
        joinDate: '2026年1月'
      };
      return;
    }

    if (!this.rdbStore) {
      throw new Error('用户信息数据库未初始化（无有效上下文）');
    }

    // 无 id 条件，清空整张表
    const predicates = new relationalStore.RdbPredicates(this.tableName);
    await this.rdbStore.delete(predicates);
    // 重新插入默认值
    await this.initDefaultUserInfo();
  }
}

// 导出单例实例（唯一默认导出，无重复）
export default UserInfoRdb.getInstance();