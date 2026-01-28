// entry/src/main/ets/db/ChatHandler.ets
import { relationalStore } from '@kit.ArkData';
import Message from '../viewmodel/Message';

export class ChatHandler {
  private store: relationalStore.RdbStore;

  constructor(store: relationalStore.RdbStore) {
    this.store = store;
  }

  async insertMessage(msg: Message, owner: string) {
    const valueBucket: relationalStore.ValuesBucket = {
      'id': msg.id,
      'owner': owner, // 标记这条记录属于哪个手机号的用户
      'nickName': msg.nickName,
      'content': msg.content,
      'time': msg.time,
      'isSelf': msg.isSelf ? 1 : 0,
      'avatar': msg.avatar.toString()
    };
    return await this.store.insert('ChatHistory', valueBucket);
  }

  /**
   * 获取首页列表：只查属于我自己的记录
   */
  async queryChatList(myPhone: string): Promise<Message[]> {
    const list: Message[] = [];
    // 更加严谨的 SQL：先按人分组找最后一笔，再关联出完整行
    const SQL = `SELECT * FROM ChatHistory
               WHERE owner = '${myPhone}'
               GROUP BY nickName
               HAVING id = MAX(id)
               ORDER BY id DESC`;

    try {
      let res = await this.store.querySql(SQL);
      console.info(`查询 owner=${myPhone} 的列表，查到条数: ${res.rowCount}`);

      while (res.goToNextRow()) {
        list.push({
          id: res.getString(res.getColumnIndex('id')),
          nickName: res.getString(res.getColumnIndex('nickName')),
          content: res.getString(res.getColumnIndex('content')),
          time: res.getString(res.getColumnIndex('time')),
          avatar: res.getString(res.getColumnIndex('avatar')),
          isSelf: res.getLong(res.getColumnIndex('isSelf')) === 1
        });
      }
      res.close();
    } catch (e) {
      console.error("SQL查询失败: " + JSON.stringify(e));
    }
    return list;
  }

  /**
   * 修复点：确保参数名与函数体内的变量名一致
   * 这里我们把参数名定为 contactId (即对方的手机号/标识)
   */
  async queryHistoryByContact(contactId: string): Promise<Message[]> {
    const list: Message[] = [];
    const predicates = new relationalStore.RdbPredicates('ChatHistory');

    // 关键：这里使用传进来的参数 contactId
    predicates.equalTo('nickName', contactId);
    predicates.orderByAsc('id');

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