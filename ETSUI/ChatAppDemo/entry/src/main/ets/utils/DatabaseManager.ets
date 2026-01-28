// entry/src/main/ets/utils/DatabaseManager.ts
import { relationalStore } from '@kit.ArkData';
import { common } from '@kit.AbilityKit';
import { UserHandler } from './UserHandler';
import { FriendHandler } from './FriendHandler';
import { ChatHandler } from './ChatHandler';
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
    // 如果已经初始化过，直接返回
    if (this.store && this.user && this.friend && this.chat) return;

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

    const SQL_CREATE_CHAT = `CREATE TABLE IF NOT EXISTS ChatHistory (
      id TEXT PRIMARY KEY,
      nickName TEXT,
      content TEXT,
      time TEXT,
      isSelf INTEGER,
      avatar TEXT
    )`;

    try {
      // 2. 获取数据库实例
      const storeInstance = await relationalStore.getRdbStore(context, this.STORE_CONFIG);
      this.store = storeInstance;

      // 3. 执行建表
      await this.store.executeSql(SQL_CREATE_USER);
      await this.store.executeSql(SQL_CREATE_FRIEND_REQUEST);
      await this.store.executeSql(SQL_CREATE_FRIEND);
      await this.store.executeSql(SQL_CREATE_CHAT);

      // 4. 实例化处理器
      this.user = new UserHandler(storeInstance);
      this.friend = new FriendHandler(storeInstance);
      this.chat = new ChatHandler(storeInstance);

      // 5. 填充初始模拟数据
      await this.prePopulateChatData();

      console.info('ChatDatabase: 数据库所有表初始化成功');
    } catch (err) {
      console.error('ChatDatabase: 初始化失败', JSON.stringify(err));
    }
  }

  /**
   * 预填充模拟数据逻辑
   * 只有在 ChatHistory 为空时才会执行，避免重复插入
   */
  private async prePopulateChatData() {
    if (!this.chat) return;

    try {
      const currentList = await this.chat.queryChatList();
      if (currentList.length === 0) {
        // 显式声明类型，确保符合 Message 接口定义
        const mockData: Message[] = [
          {
            id: 'm1',
            avatar: $r('app.media.user1'),
            nickName: '桑良翰',
            content: '你好，代码已经更新了',
            time: '10:30',
            isSelf: false
          },
          {
            id: 'm2',
            avatar: $r('app.media.user2'),
            nickName: '张廖初蝶',
            content: '好的，稍后我测试一下',
            time: '11:15',
            isSelf: false
          },
          {
            id: 'm3',
            avatar: $r('app.media.user3'),
            nickName: '宇文雅彤',
            content: '数据库表结构已经同步',
            time: '14:20',
            isSelf: false
          }
        ];

        for (const m of mockData) {
          await this.chat.insertMessage(m);
        }
        console.info('ChatDatabase: 模拟数据预填充完成');
      }
    } catch (e) {
      console.error('ChatDatabase: 预填充数据失败', JSON.stringify(e));
    }
  }
}

// 导出单例
export const dbManager = new DatabaseManager();