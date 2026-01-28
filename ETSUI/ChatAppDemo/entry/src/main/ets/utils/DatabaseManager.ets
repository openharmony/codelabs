// entry/src/main/ets/utils/DatabaseManager.ts
import { relationalStore } from '@kit.ArkData';
import { common } from '@kit.AbilityKit';
import { UserHandler } from '../utils/UserHandler';
import { FriendHandler } from '../utils/FriendHandler';
import { ChatHandler } from '../utils/ChatHandler';
import Message from '../viewmodel/Message';

export class DatabaseManager {
  private store: relationalStore.RdbStore | null = null;
  public user: UserHandler | null = null;
  public friend: FriendHandler | null = null;
  public chat: ChatHandler | null = null;

  private readonly STORE_CONFIG: relationalStore.StoreConfig = {
    name: 'ChatDatabase.db',
    securityLevel: relationalStore.SecurityLevel.S1
  };

  async initDB(context: common.Context) {
    if (this.store && this.user && this.friend && this.chat) return;

    // 1. 修改建表语句：增加 owner 字段（用于隔离不同账号的记录）
    const SQL_CREATE_USER = `CREATE TABLE IF NOT EXISTS User (id INTEGER PRIMARY KEY AUTOINCREMENT, avatarPath TEXT, nickName TEXT, country TEXT, phoneNumber TEXT, password TEXT)`;
    const SQL_CREATE_FRIEND_REQUEST = `CREATE TABLE IF NOT EXISTS FriendRequest (id INTEGER PRIMARY KEY AUTOINCREMENT, fromPhone TEXT, toPhone TEXT, fromName TEXT, message TEXT, status INTEGER)`;
    const SQL_CREATE_FRIEND = `CREATE TABLE IF NOT EXISTS Friend (id INTEGER PRIMARY KEY AUTOINCREMENT, myPhone TEXT, friendPhone TEXT)`;

    // 重点修改：ChatHistory 增加 owner 字段
    const SQL_CREATE_CHAT = `CREATE TABLE IF NOT EXISTS ChatHistory (
                              id TEXT PRIMARY KEY,
                              owner TEXT,      -- 新增：记录这条消息归属于哪个登录账号
                              nickName TEXT,   -- 对方的昵称/手机号
                              content TEXT,
                              time TEXT,
                              isSelf INTEGER,
                              avatar TEXT
                              )`;

    try {
      const storeInstance = await relationalStore.getRdbStore(context, this.STORE_CONFIG);
      this.store = storeInstance;

      // 执行建表
      await this.store.executeSql(SQL_CREATE_USER);
      await this.store.executeSql(SQL_CREATE_FRIEND_REQUEST);
      await this.store.executeSql(SQL_CREATE_FRIEND);
      await this.store.executeSql(SQL_CREATE_CHAT);

      // 实例化处理器
      this.user = new UserHandler(storeInstance);
      this.friend = new FriendHandler(storeInstance);
      this.chat = new ChatHandler(storeInstance);

      // 预填充数据逻辑改为按需填充（或在登录后填充）
      // await this.prePopulateChatData();

      console.info('ChatDatabase: 所有表（包含隔离字段）初始化成功');
    } catch (err) {
      console.error('数据库初始化失败', JSON.stringify(err));
    }
  }

  // 这里的 mock 数据逻辑需要适配 owner 字段，建议暂时注释或根据当前用户填充
  private async prePopulateChatData() {
    if (!this.chat) return;
    // 此时不知道 owner 是谁，建议由业务层根据登录用户调用填充
  }
}

export const dbManager = new DatabaseManager();