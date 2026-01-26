import { relationalStore } from '@kit.ArkData';
import { UserModel } from '../viewmodel/UserModel';

export class UserHandler {
  private store: relationalStore.RdbStore;

  constructor(store: relationalStore.RdbStore) {
    this.store = store;
  }

  // 将之前的插入逻辑移到这里
  async insertUser(userData: UserModel) {
    if (!this.store) {
      console.error('数据库尚未初始化，请先调用 initDB');
      return;
    }

    const valueBucket: relationalStore.ValuesBucket = {
      avatarPath: userData.avatarPath,
      nickName: userData.nickName,
      country: userData.country,
      phoneNumber: userData.phoneNumber,
      password: userData.password,
    };

    try {
      return await this.store.insert('User', valueBucket);
    } catch (err) {
      console.error('插入用户失败，错误码:', err.code);
    }

    return;
  }

  async queryUserByPhone(phoneNumber: string): Promise<UserModel | null> {
    if (!this.store) return null;

    // 1. 构造谓词（查询条件）
    let predicates = new relationalStore.RdbPredicates('User');
    predicates.equalTo('phoneNumber', phoneNumber);

    try {
      // 2. 执行查询
      let resultSet = await this.store.query(predicates);

      // 3. 处理结果
      if (resultSet.rowCount > 0 && resultSet.goToFirstRow()) {
        const user: UserModel = {
          avatarPath: resultSet.getString(resultSet.getColumnIndex('avatarPath')),
          nickName: resultSet.getString(resultSet.getColumnIndex('nickName')),
          country: resultSet.getString(resultSet.getColumnIndex('country')),
          phoneNumber: resultSet.getString(resultSet.getColumnIndex('phoneNumber')),
          password: resultSet.getString(resultSet.getColumnIndex('password'))
        };
        resultSet.close(); // 记得关闭结果集释放内存
        return user;
      }
      resultSet.close();
    } catch (err) {
      console.error('查询失败:', JSON.stringify(err));
    }
    return null;
  }

  async queryAllUsers(): Promise<UserModel[]> {
    const users: UserModel[] = [];
    if (!this.store) return users;

    // 1. 构造谓词，不加任何条件代表查询全表
    let predicates = new relationalStore.RdbPredicates('User');

    try {
      let resultSet = await this.store.query(predicates);

      // 2. 循环遍历结果集
      while (resultSet.goToNextRow()) {
        const user: UserModel = {
          avatarPath: resultSet.getString(resultSet.getColumnIndex('avatarPath')),
          nickName: resultSet.getString(resultSet.getColumnIndex('nickName')),
          country: resultSet.getString(resultSet.getColumnIndex('country')),
          phoneNumber: resultSet.getString(resultSet.getColumnIndex('phoneNumber')),
          password: resultSet.getString(resultSet.getColumnIndex('password'))
        };
        users.push(user);
      }
      resultSet.close();
      console.info(`测试脚本：共查询到 ${users.length} 条数据`);
    } catch (err) {
      console.error('测试脚本执行失败:', JSON.stringify(err));
    }
    return users;
  }

  // UserHandler.ets

  /**
   * 检查手机号是否已存在
   * @param phoneNumber 待检查的手机号
   * @returns true 表示已存在，false 表示不存在
   */
  async isPhoneNumberExists(phoneNumber: string): Promise<boolean> {
    if (!this.store) return false;

    let predicates = new relationalStore.RdbPredicates('User');
    predicates.equalTo('phoneNumber', phoneNumber);

    try {
      let resultSet = await this.store.query(predicates);
      let count = resultSet.rowCount;
      resultSet.close(); // 查完立即关闭
      return count > 0;
    } catch (err) {
      console.error('校验手机号失败:', JSON.stringify(err));
      return false;
    }
  }
}