// entry/src/main/ets/db/ChatHandler.ets
import { relationalStore } from '@kit.ArkData';
import Message from '../viewmodel/Message';

export class ChatHandler {
  private store: relationalStore.RdbStore;

  constructor(store: relationalStore.RdbStore) {
    this.store = store;
  }

  /**
   * 插入新消息
   * 由于 avatar 是 ResourceStr，不管是 $r() 还是 string 都可以直接存储
   */
  async insertMessage(msg: Message) {
    const valueBucket: relationalStore.ValuesBucket = {
      'id': msg.id,
      'nickName': msg.nickName,
      'content': msg.content,
      'time': msg.time,
      'isSelf': msg.isSelf ? 1 : 0,
      'avatar': msg.avatar.toString() // 统一转为字符串存入数据库
    };
    return await this.store.insert('ChatHistory', valueBucket);
  }

  /**
   * 获取首页聊天列表（每个联系人仅显示最新一条）
   */
  async queryChatList(): Promise<Message[]> {
    const list: Message[] = [];
    // SQL 逻辑：按昵称分组取最新的消息 ID，再查出对应的完整记录
    const SQL = `SELECT * FROM ChatHistory WHERE id IN (SELECT MAX(id) FROM ChatHistory GROUP BY nickName) ORDER BY id DESC`;

    try {
      let res = await this.store.querySql(SQL);
      while (res.goToNextRow()) {
        const item: Message = {
          id: res.getString(res.getColumnIndex('id')),
          nickName: res.getString(res.getColumnIndex('nickName')),
          content: res.getString(res.getColumnIndex('content')),
          time: res.getString(res.getColumnIndex('time')),
          // 核心：直接读取 string，由于接口是 ResourceStr，这里可以直接赋值，不再报错
          avatar: res.getString(res.getColumnIndex('avatar')),
          isSelf: res.getLong(res.getColumnIndex('isSelf')) === 1
        };
        list.push(item);
      }
      res.close();
    } catch (e) {
      console.error('ChatHandler: queryChatList error', JSON.stringify(e));
    }
    return list;
  }

  /**
   * 获取与特定好友的所有历史记录
   */
  async queryHistoryByContact(nickName: string): Promise<Message[]> {
    const list: Message[] = [];
    const predicates = new relationalStore.RdbPredicates('ChatHistory');
    predicates.equalTo('nickName', nickName);
    predicates.orderByAsc('id'); // 聊天记录按时间正序排列

    try {
      let res = await this.store.query(predicates);
      while (res.goToNextRow()) {
        const item: Message = {
          id: res.getString(res.getColumnIndex('id')),
          nickName: res.getString(res.getColumnIndex('nickName')),
          content: res.getString(res.getColumnIndex('content')),
          time: res.getString(res.getColumnIndex('time')),
          avatar: res.getString(res.getColumnIndex('avatar')),
          isSelf: res.getLong(res.getColumnIndex('isSelf')) === 1
        };
        list.push(item);
      }
      res.close();
    } catch (e) {
      console.error('ChatHandler: queryHistoryByContact error', JSON.stringify(e));
    }
    return list;
  }
}