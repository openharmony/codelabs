import relationalStore from '@ohos.data.relationalStore';
import common from '@ohos.app.ability.common';
import { BusinessError } from '@ohos.base';
import User from '../model/User';
import AppAccount from '../model/AppAccount'; // 确保你已经创建了 AppAccount 模型

/**
 * RdbHelper.ets
 * 数据库核心工具类
 * 功能：
 * 1. 初始化数据库 (PasswordSafe.db)
 * 2. 创建用户表 (USER_TABLE)
 * 3. 创建账号记录表 (APP_ACCOUNT_TABLE)
 * 4. 提供所有表的增删改查方法
 */
export default class RdbHelper {
  private rdbStore: relationalStore.RdbStore | null = null;
  private context: common.UIAbilityContext;

  // 数据库配置
  private static readonly STORE_CONFIG: relationalStore.StoreConfig = {
    name: 'PasswordSafe.db', // 数据库文件名
    securityLevel: relationalStore.SecurityLevel.S1
  };

  constructor(context: common.UIAbilityContext) {
    this.context = context;
  }

  /**
   * 初始化数据库
   * 创建两张表：用户表 和 账号记录表
   */
  public initRdbStore(callback: Function): void {
    if (this.rdbStore) {
      callback(true);
      return;
    }

    relationalStore.getRdbStore(this.context, RdbHelper.STORE_CONFIG, (err, store) => {
      if (err) {
        console.error(`[RdbHelper] Get RdbStore failed: ${err.message}`);
        callback(false);
        return;
      }
      this.rdbStore = store;

      // 1. 创建用户表 SQL
      const SQL_CREATE_USER = 'CREATE TABLE IF NOT EXISTS USER_TABLE ('
        + 'id INTEGER PRIMARY KEY AUTOINCREMENT, '
        + 'username TEXT NOT NULL, '
        + 'password TEXT NOT NULL, '
        + 'phone TEXT, '
        + 'email TEXT, '
        + 'avatar TEXT, '
        + 'create_time INTEGER)';

      // 2. 创建账号记录表 SQL (owner_id 用于关联是谁存的密码)
      const SQL_CREATE_ACCOUNT = 'CREATE TABLE IF NOT EXISTS APP_ACCOUNT_TABLE ('
        + 'id INTEGER PRIMARY KEY AUTOINCREMENT, '
        + 'platform_name TEXT, '
        + 'account_name TEXT, '
        + 'password TEXT, '
        + 'owner_id INTEGER)';

      try {
        // 执行建表
        this.rdbStore.executeSql(SQL_CREATE_USER);
        this.rdbStore.executeSql(SQL_CREATE_ACCOUNT);
        console.info('[RdbHelper] Tables created successfully');
      } catch (e) {
        console.error(`[RdbHelper] Create tables error: ${JSON.stringify(e)}`);
      }

      callback(true);
    });
  }

  // ==========================================
  //  用户表 (USER_TABLE) 操作
  // ==========================================

  /**
   * 插入新用户 (注册)
   */
  public insertUser(user: User): Promise<number> {
    return new Promise((resolve) => {
      if (!this.rdbStore) { resolve(-1); return; }

      try {
        this.rdbStore.insert('USER_TABLE', user.toValuesBucket(), (err, rowId) => {
          if (err) {
            console.error(`[RdbHelper] Insert user failed: ${err.message}`);
            resolve(-1);
          } else {
            resolve(rowId);
          }
        });
      } catch (e) {
        console.error(`[RdbHelper] Insert user exception: ${e}`);
        resolve(-1);
      }
    });
  }

  /**
   * 查询用户 (登录验证 / 查重)
   */
  public queryUser(username: string): Promise<User | null> {
    return new Promise((resolve) => {
      if (!this.rdbStore) { resolve(null); return; }

      try {
        let predicates = new relationalStore.RdbPredicates('USER_TABLE');
        predicates.equalTo('username', username);

        this.rdbStore.query(predicates, ['id', 'username', 'password', 'phone'], (err, resultSet) => {
          if (err || resultSet.rowCount === 0) {
            resultSet?.close();
            resolve(null);
            return;
          }

          resultSet.goToFirstRow();
          let user = new User(
            resultSet.getString(resultSet.getColumnIndex('username')),
            resultSet.getString(resultSet.getColumnIndex('password')),
            resultSet.getString(resultSet.getColumnIndex('phone'))
          );
          user.setId(resultSet.getLong(resultSet.getColumnIndex('id')));

          resultSet.close();
          resolve(user);
        });
      } catch (e) {
        console.error(`[RdbHelper] Query user exception: ${e}`);
        resolve(null);
      }
    });
  }

  // ==========================================
  //  账号记录表 (APP_ACCOUNT_TABLE) 操作
  // ==========================================

  /**
   * [新增] 添加一条第三方账号记录
   * 例如：存一个微信账号
   */
  public insertAppAccount(account: AppAccount): Promise<number> {
    return new Promise((resolve) => {
      if (!this.rdbStore) { resolve(-1); return; }

      try {
        this.rdbStore.insert('APP_ACCOUNT_TABLE', account.toValuesBucket(), (err, rowId) => {
          if (err) {
            console.error(`[RdbHelper] Insert account failed: ${err.message}`);
            resolve(-1);
          } else {
            resolve(rowId);
          }
        });
      } catch (e) {
        console.error(`[RdbHelper] Insert account exception: ${e}`);
        resolve(-1);
      }
    });
  }

  /**
   * [新增] 查询某人的所有账号记录
   * @param ownerId 当前登录用户的ID (只查属于这个人的密码)
   */
  public queryAppAccounts(ownerId: number): Promise<Array<AppAccount>> {
    return new Promise((resolve) => {
      if (!this.rdbStore) { resolve([]); return; }

      try {
        let predicates = new relationalStore.RdbPredicates('APP_ACCOUNT_TABLE');
        predicates.equalTo('owner_id', ownerId); // 关键：权限隔离
        predicates.orderByDesc('id'); // 后添加的显示在前面

        this.rdbStore.query(predicates, ['id', 'platform_name', 'account_name', 'password', 'owner_id'], (err, resultSet) => {
          if (err) { resolve([]); return; }

          let list: Array<AppAccount> = [];
          while (resultSet.goToNextRow()) {
            let p = resultSet.getString(resultSet.getColumnIndex('platform_name'));
            let a = resultSet.getString(resultSet.getColumnIndex('account_name'));
            let pwd = resultSet.getString(resultSet.getColumnIndex('password'));
            let oId = resultSet.getLong(resultSet.getColumnIndex('owner_id'));
            let id = resultSet.getLong(resultSet.getColumnIndex('id'));

            let acc = new AppAccount(p, a, pwd, oId);
            acc.setId(id);
            list.push(acc);
          }
          resultSet.close();
          resolve(list);
        });
      } catch (e) {
        console.error(`[RdbHelper] Query accounts exception: ${e}`);
        resolve([]);
      }
    });
  }

  /**
   * [新增] 删除一条账号记录
   */
  public deleteAppAccount(id: number): Promise<number> {
    return new Promise((resolve) => {
      if (!this.rdbStore) { resolve(-1); return; }

      try {
        let predicates = new relationalStore.RdbPredicates('APP_ACCOUNT_TABLE');
        predicates.equalTo('id', id);

        this.rdbStore.delete(predicates, (err, rows) => {
          if (err) {
            console.error(`[RdbHelper] Delete account failed: ${err.message}`);
            resolve(-1);
          } else {
            resolve(rows);
          }
        });
      } catch (e) {
        console.error(`[RdbHelper] Delete account exception: ${e}`);
        resolve(-1);
      }
    });
  }
  public updateAppAccount(account: AppAccount): Promise<number> {
    return new Promise((resolve) => {
      if (!this.rdbStore) { resolve(-1); return; }

      try {
        // 1. 指定要修改哪一行 (根据 ID)
        let predicates = new relationalStore.RdbPredicates('APP_ACCOUNT_TABLE');
        predicates.equalTo('id', account.getId());

        // 2. 执行更新
        this.rdbStore.update(account.toValuesBucket(), predicates, (err, rows) => {
          if (err) {
            console.error(`[RdbHelper] Update account failed: ${err.message}`);
            resolve(-1);
          } else {
            console.info(`[RdbHelper] Updated rows: ${rows}`);
            resolve(rows);
          }
        });
      } catch (e) {
        console.error(`[RdbHelper] Update account exception: ${e}`);
        resolve(-1);
      }
    });
  }
}