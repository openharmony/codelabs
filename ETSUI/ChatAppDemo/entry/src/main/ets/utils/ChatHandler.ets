// entry/src/main/ets/db/ChatHandler.ets
import { relationalStore } from '@kit.ArkData';
import Message from '../viewmodel/Message';

export class ChatHandler {
  private store: relationalStore.RdbStore;

  constructor(store: relationalStore.RdbStore) {
    this.store = store;
  }

  // 插入消息：增加 owner 字段用于标识这条记录属于谁
  async insertMessage(msg: Message, owner: string) {
    const valueBucket: relationalStore.ValuesBucket = {
      'id': msg.id,
      'owner': owner,
      'nickName': msg.nickName, // 对 owner 而言，对方的标识
      'content': msg.content,
      'time': msg.time,
      'isSelf': msg.isSelf ? 1 : 0,
      'avatar': msg.avatar.toString()
    };
    return await this.store.insert('ChatHistory', valueBucket);
  }

  // 查询首页列表：只查询属于“我”的记录，并按聊天对象分组
  async queryChatList(myPhone: string): Promise<Message[]> {
    const list: Message[] = [];
    const SQL = `SELECT * FROM ChatHistory
                 WHERE owner = '${myPhone}'
                 GROUP BY nickName
                 HAVING id = MAX(id)
                 ORDER BY id DESC`;

    let res = await this.store.querySql(SQL);
    while (res.goToNextRow()) {
      list.push(this.parseRow(res));
    }
    res.close();
    return list;
  }

  // 查询对话详情：查询“我”与“特定对象”的所有聊天记录
  async queryHistoryByContact(contactPhone: string, myPhone: string): Promise<Message[]> {
    const predicates = new relationalStore.RdbPredicates('ChatHistory');
    predicates.equalTo('owner', myPhone).and().equalTo('nickName', contactPhone);
    predicates.orderByAsc('id');

    let res = await this.store.query(predicates);
    const list: Message[] = [];
    while (res.goToNextRow()) {
      list.push(this.parseRow(res));
    }
    res.close();
    return list;
  }

  private parseRow(res: relationalStore.ResultSet): Message {
    return {
      id: res.getString(res.getColumnIndex('id')),
      nickName: res.getString(res.getColumnIndex('nickName')),
      content: res.getString(res.getColumnIndex('content')),
      time: res.getString(res.getColumnIndex('time')),
      avatar: res.getString(res.getColumnIndex('avatar')),
      isSelf: res.getLong(res.getColumnIndex('isSelf')) === 1
    };
  }
}