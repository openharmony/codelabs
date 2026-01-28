// entry/src/main/ets/db/ChatHandler.ts
import { relationalStore } from '@kit.ArkData';
import Message from '../viewmodel/Message';

export class ChatHandler {
  private store: relationalStore.RdbStore;

  constructor(store: relationalStore.RdbStore) {
    this.store = store;
  }

  // 插入消息
  async insertMessage(msg: Message) {
    const valueBucket: relationalStore.ValuesBucket = {
      'id': msg.id,
      'nickName': msg.nickName,
      'content': msg.content,
      'time': msg.time,
      'isSelf': msg.isSelf ? 1 : 0
    };
    return await this.store.insert('ChatHistory', valueBucket);
  }

  // 查询首页列表：按昵称分组取最新一条
  async queryChatList(): Promise<Message[]> {
    const list: Message[] = [];
    // SQL: 只有聊过天的人才会出现在这里
    const SQL = `SELECT * FROM ChatHistory WHERE id IN (SELECT MAX(id) FROM ChatHistory GROUP BY nickName) ORDER BY id DESC`;

    try {
      let res = await this.store.querySql(SQL);
      while (res.goToNextRow()) {
        const item: Message = {
          id: res.getString(res.getColumnIndex('id')),
          nickName: res.getString(res.getColumnIndex('nickName')),
          content: res.getString(res.getColumnIndex('content')),
          time: res.getString(res.getColumnIndex('time')),
          avatar: $r('app.media.user_default'), // 统一默认头像
          isSelf: res.getLong(res.getColumnIndex('isSelf')) === 1
        };
        list.push(item);
      }
      res.close();
    } catch (e) {
      console.error('ChatListQueryError', JSON.stringify(e));
    }
    return list;
  }
}