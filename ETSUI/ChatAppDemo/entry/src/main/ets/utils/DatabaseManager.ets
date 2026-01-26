import { relationalStore } from '@kit.ArkData';
import { common } from '@kit.AbilityKit';
import { UserModel } from '../viewmodel/UserModel';
import { UserHandler } from './UserHandler';

export class DatabaseManager {
  private store: relationalStore.RdbStore | null = null;

  // 核心：将子模块暴露出来
  public user: UserHandler | null = null;
  // public chat: ChatHandler | null = null; // 后续可以加聊天记录表

  // 修改后的数据库名
  private readonly STORE_CONFIG: relationalStore.StoreConfig = {
    name: 'ChatDatabase.db',
    securityLevel: relationalStore.SecurityLevel.S1
  };

  async initDB(context: common.Context) {
    // 简单的判断：如果已经初始化过了，就不再重复执行
    if (this.store && this.user) return;

    const SQL_CREATE_TABLE = `CREATE TABLE IF NOT EXISTS User (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      avatarPath TEXT,
      nickName TEXT,
      country TEXT,
      phoneNumber TEXT,
      password TEXT
    )`;

    try {
      this.store = await relationalStore.getRdbStore(context, this.STORE_CONFIG);
      this.user = new UserHandler(this.store);
      await this.store.executeSql(SQL_CREATE_TABLE);
      console.info('ChatDatabase 初始化成功');
    } catch (err) {
      console.error('数据库初始化失败，错误码:', err.code);
    }
  }

}

// 导出单例，方便在不同页面使用同一个数据库实例
export const dbManager = new DatabaseManager();