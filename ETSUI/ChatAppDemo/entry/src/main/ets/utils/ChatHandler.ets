/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { relationalStore } from '@kit.ArkData';
import Message from '../viewmodel/Message';

export class ChatHandler {
  private store: relationalStore.RdbStore;

  constructor(store: relationalStore.RdbStore) {
    this.store = store;
  }

  // 显式声明返回类型 Promise<number>
  async insertMessage(msg: Message, owner: string, targetPhone: string): Promise<number> {
    const valueBucket: relationalStore.ValuesBucket = {
      'id': msg.id,
      'owner': owner,
      'targetPhone': targetPhone,
      'nickName': msg.nickName,
      'content': msg.content,
      'time': msg.time,
      'isSelf': msg.isSelf ? 1 : 0,
      'avatar': msg.avatar.toString()
    };
    return await this.store.insert('ChatHistory', valueBucket);
  }

  // 显式声明返回类型 Promise<Message[]>
  // async queryChatList(myPhone: string): Promise<Message[]> {
  //   const list: Message[] = [];
  //   // 核心逻辑：先过滤 owner，然后通过子查询找到每个 targetPhone 对应的最大 ID（即最新消息）
  //   const SQL = `SELECT * FROM ChatHistory
  //              WHERE owner = '${myPhone}'
  //              AND id IN (
  //                SELECT MAX(id) FROM ChatHistory
  //                WHERE owner = '${myPhone}'
  //                GROUP BY targetPhone
  //              )
  //              ORDER BY id DESC`; // 确保最新的聊天排在列表最上方
  //
  //   let res = await this.store.querySql(SQL);
  //   while (res.goToNextRow()) {
  //     list.push(this.parseRow(res));
  //   }
  //   res.close();
  //   return list;
  // }

  async queryChatList(myPhone: string): Promise<Message[]> {
    const list: Message[] = [];
    // 核心：查询 owner 是我的记录中，每个 targetPhone 对应的最新一条（无论是我发的还是对方发的）
    const SQL = `SELECT * FROM ChatHistory
               WHERE owner = '${myPhone}'
               AND id IN (
                 SELECT MAX(id) FROM ChatHistory
                 WHERE owner = '${myPhone}'
                 GROUP BY targetPhone
               )
               ORDER BY id DESC`;

    let res = await this.store.querySql(SQL);
    while (res.goToNextRow()) {
      list.push(this.parseRow(res));
    }
    res.close();
    return list;
  }

  // 显式声明返回类型 Promise<Message[]>
  async queryHistoryByContact(targetPhone: string, myPhone: string): Promise<Message[]> {
    const list: Message[] = [];
    // 必须使用谓词查询，严禁使用带 GROUP BY 的原生 SQL 查历史
    const predicates = new relationalStore.RdbPredicates('ChatHistory');

    predicates.equalTo('owner', myPhone)        // 我看到的
      .and()
      .equalTo('targetPhone', targetPhone); // 对方发给我的，或我发给对方的

    // 必须按 ID 升序，否则消息顺序会乱
    predicates.orderByAsc('id');

    let res = await this.store.query(predicates);
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
    return list;
  }

  private parseRow(res: relationalStore.ResultSet): Message {
    return {
      // 核心修复：列表显示的 ID 必须设为对方的手机号
      id: res.getString(res.getColumnIndex('targetPhone')), // 这里极其重要！确保跳转传的是手机号
      nickName: res.getString(res.getColumnIndex('nickName')),
      content: res.getString(res.getColumnIndex('content')),
      time: res.getString(res.getColumnIndex('time')),
      avatar: res.getString(res.getColumnIndex('avatar')),
      isSelf: res.getLong(res.getColumnIndex('isSelf')) === 1
    };
  }
}