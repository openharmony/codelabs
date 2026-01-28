import { relationalStore } from '@kit.ArkData';
import { common } from '@kit.AbilityKit';
import { UserHandler } from './UserHandler';
import { FriendHandler } from './FriendHandler';

export class DatabaseManager {
  private store: relationalStore.RdbStore | null = null;

  // 核心：将子模块暴露出来
  public user: UserHandler | null = null;
  public friend: FriendHandler | null = null;
  // public chat: ChatHandler | null = null; // 后续可以加聊天记录表

  // 修改后的数据库名
  private readonly STORE_CONFIG: relationalStore.StoreConfig = {
    name: 'ChatDatabase.db',
    securityLevel: relationalStore.SecurityLevel.S1
  };

  //初始化数据库
  async initDB(context: common.Context) {
    if (this.store && this.user && this.friend) return;

    // 1. 定义建表语句
    const SQL_CREATE_USER = `CREATE TABLE IF NOT EXISTS User (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    avatarPath TEXT,
    nickName TEXT,
    country TEXT,
    phoneNumber TEXT,
    password TEXT
  )`;

    const SQL_CREATE_FRIEND_REQUEST = `CREATE TABLE IF NOT EXISTS FriendRequest (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    fromPhone TEXT,
    toPhone TEXT,
    fromName TEXT,
    message TEXT,
    status INTEGER
  )`;

    const SQL_CREATE_FRIEND = `CREATE TABLE IF NOT EXISTS Friend (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    myPhone TEXT,
    friendPhone TEXT
  )`;

    try {
      // 2. 获取 store 实例
      const storeInstance = await relationalStore.getRdbStore(context, this.STORE_CONFIG);
      this.store = storeInstance;

      // 3. 实例化处理器
      this.user = new UserHandler(storeInstance);
      this.friend = new FriendHandler(storeInstance);

      // 4. 依次执行建表语句
      await this.store.executeSql(SQL_CREATE_USER);
      await this.store.executeSql(SQL_CREATE_FRIEND_REQUEST);
      await this.store.executeSql(SQL_CREATE_FRIEND);

      console.info('ChatDatabase: 所有表初始化成功');
    } catch (err) {
      console.error('数据库初始化失败，错误码:', err.code);
    }
  }

}

// 导出单例，方便在不同页面使用同一个数据库实例
export const dbManager = new DatabaseManager();