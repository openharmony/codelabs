// entry/src/main/ets/db/DatabaseManager.ts
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
  public chat: ChatHandler | null = null; // 暴露聊天处理器

  private readonly STORE_CONFIG: relationalStore.StoreConfig = {
    name: 'ChatDatabase.db',
    securityLevel: relationalStore.SecurityLevel.S1
  };

  async initDB(context: common.Context) {
    if (this.store && this.user && this.friend && this.chat) return;

    // 建表语句集
    const SQL_CREATE_USER = `CREATE TABLE IF NOT EXISTS User (id INTEGER PRIMARY KEY AUTOINCREMENT, avatarPath TEXT, nickName TEXT, country TEXT, phoneNumber TEXT, password TEXT)`;
    const SQL_CREATE_FRIEND_REQUEST = `CREATE TABLE IF NOT EXISTS FriendRequest (id INTEGER PRIMARY KEY AUTOINCREMENT, fromPhone TEXT, toPhone TEXT, fromName TEXT, message TEXT, status INTEGER)`;
    const SQL_CREATE_FRIEND = `CREATE TABLE IF NOT EXISTS Friend (id INTEGER PRIMARY KEY AUTOINCREMENT, myPhone TEXT, friendPhone TEXT)`;
    // 新增聊天记录表
    const SQL_CREATE_CHAT = `CREATE TABLE IF NOT EXISTS ChatHistory (id TEXT PRIMARY KEY, nickName TEXT, content TEXT, time TEXT, isSelf INTEGER)`;

    try {
      const storeInstance = await relationalStore.getRdbStore(context, this.STORE_CONFIG);
      this.store = storeInstance;

      // 实例化所有处理器
      this.user = new UserHandler(storeInstance);
      this.friend = new FriendHandler(storeInstance);
      this.chat = new ChatHandler(storeInstance);

      // 执行建表
      await this.store.executeSql(SQL_CREATE_USER);
      await this.store.executeSql(SQL_CREATE_FRIEND_REQUEST);
      await this.store.executeSql(SQL_CREATE_FRIEND);
      await this.store.executeSql(SQL_CREATE_CHAT);

      // 核心：完成初始化后，填充模拟数据保证一打开就有联系人
      await this.prePopulateChatData();

      console.info('ChatDatabase: 所有表初始化及数据填充成功');
    } catch (err) {
      console.error('数据库初始化失败', JSON.stringify(err));
    }
  }

  // 预填充数据逻辑（解决 arkts-no-noninferrable-arr-literals 报错）
  private async prePopulateChatData() {
    if (!this.chat) return;
    const currentList = await this.chat.queryChatList();
    if (currentList.length === 0) {
      // 显式声明 Message[] 类型，防止 ArkTS 无法推断类型
      const mockData: Message[] = [
        { id: 'm1', avatar: $r('app.media.user1'), nickName: '桑良翰', content: '代码已同步到数据库', time: '18:23', isSelf: false },
        { id: 'm2', avatar: $r('app.media.user2'), nickName: '张廖初蝶', content: '好的，我已经看到消息了', time: '15:10', isSelf: false },
        { id: 'm3', avatar: $r('app.media.user3'), nickName: '宇文雅彤', content: '今晚八点准时测试', time: '12:00', isSelf: false }
      ];
      for (const m of mockData) {
        await this.chat.insertMessage(m);
      }
    }
  }
}

export const dbManager = new DatabaseManager();