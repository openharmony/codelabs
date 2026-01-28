import { relationalStore } from '@kit.ArkData';
import { FriendRequestModel } from '../viewmodel/FriendRequestModel';
import { UserModel } from '../viewmodel/UserModel';
import { dbManager } from '../utils/DatabaseManager'

export class FriendHandler {
  private store: relationalStore.RdbStore;
  constructor(store: relationalStore.RdbStore) { this.store = store; }

  // 1. 发送好友申请
  async sendRequest(request: FriendRequestModel) {
    const valueBucket: relationalStore.ValuesBucket = {
      fromPhone: request.fromPhone,
      toPhone: request.toPhone,
      fromName: request.fromName,
      message: request.message,
      status: 0
    };
    try {
      return await this.store.insert('FriendRequest', valueBucket);
    } catch (error) {
      console.error('发送好友申请失败:', JSON.stringify(error));
    }
    return;
  }

  // 2. 获取发给我的所有申请
  async getMyRequests(myPhone: string): Promise<FriendRequestModel[]> {
    let predicates = new relationalStore.RdbPredicates('FriendRequest');
    predicates.equalTo('toPhone', myPhone);
    let list: FriendRequestModel[] = [];
    try {
      let resultSet = await this.store.query(predicates);

      while (resultSet.goToNextRow()) {
        list.push({
          id: resultSet.getLong(resultSet.getColumnIndex('id')),
          fromPhone: resultSet.getString(resultSet.getColumnIndex('fromPhone')),
          toPhone: resultSet.getString(resultSet.getColumnIndex('toPhone')),
          fromName: resultSet.getString(resultSet.getColumnIndex('fromName')),
          message: resultSet.getString(resultSet.getColumnIndex('message')),
          status: resultSet.getLong(resultSet.getColumnIndex('status'))
        });
      }
      resultSet.close();

    } catch (err) {
      console.error('查询好友申请失败:', JSON.stringify(err));
    }

    return list;
  }

  // 3. 处理申请 (接受/拒绝)
  async handleRequest(requestId: number, status: number, fromPhone: string, myPhone: string) {
    // A. 更新申请状态
    let predicates = new relationalStore.RdbPredicates('FriendRequest');
    predicates.equalTo('id', requestId);
    try {
      await this.store.update({ status: status }, predicates);

      // B. 如果是接受，则建立双向好友关系
      if (status === 1) {
        await this.store.insert('Friend', { myPhone: myPhone, friendPhone: fromPhone });
        await this.store.insert('Friend', { myPhone: fromPhone, friendPhone: myPhone });
      }
    } catch (err) {
      console.error('处理好友申请失败:', JSON.stringify(err));
    }

  }

  // 获取我的好友列表（关联 User 表查询头像和昵称）
  async getFriends(myPhone: string): Promise<UserModel[]> {
    if (!this.store) return [];

    // 1. 先从 Friend 表找到我所有的好友手机号
    let predicates = new relationalStore.RdbPredicates('Friend');
    predicates.equalTo('myPhone', myPhone);

    let friendPhones: string[] = [];
    try {
      let resultSet = await this.store.query(predicates);
      while (resultSet.goToNextRow()) {
        friendPhones.push(resultSet.getString(resultSet.getColumnIndex('friendPhone')));
      }
      resultSet.close();
    } catch (e) {
      console.error('获取好友手机号失败');
    }

    // 2. 如果没有好友，直接返回空数组
    if (friendPhones.length === 0) return [];

    // 3. 调用刚才在 UserHandler 里写好的批量查询方法
    // 注意：因为 dbManager 初始化时已经实例化了 user，我们可以直接跨模块调用
    const friendDetails = await dbManager.user!.queryUsersByPhones(friendPhones);

    return friendDetails;
  }

  async checkRequestExists(fromPhone: string, toPhone: string): Promise<boolean> {
    let predicates = new relationalStore.RdbPredicates('FriendRequest');
    predicates.equalTo('fromPhone', fromPhone);
    predicates.equalTo('toPhone', toPhone);
    predicates.equalTo('status', 0); // 只检查待处理的

    try {
      let resultSet = await this.store.query(predicates);
      let count = resultSet.rowCount;
      resultSet.close();
      return count > 0;
    } catch (e) {
      console.error('检查好友请求是否重复发送失败:', JSON.stringify(e));
      return true;
    }
  }

  /**
   * 检查两人是否已经是好友
   */
  async isFriend(myPhone: string, friendPhone: string): Promise<boolean> {
    if (!this.store) return false;

    let predicates = new relationalStore.RdbPredicates('Friend');
    predicates.equalTo('myPhone', myPhone);
    predicates.equalTo('friendPhone', friendPhone);

    try {
      let resultSet = await this.store.query(predicates);
      const count = resultSet.rowCount;
      resultSet.close();
      return count > 0; // 如果行数大于 0，说明已经是好友
    } catch (err) {
      return false;
    }
  }

  async queryFriendsPhones(myPhone: string): Promise<string[]> {
    if (!this.store) return [];

    let predicates = new relationalStore.RdbPredicates('Friend');
    predicates.equalTo('myPhone', myPhone);

    let friendPhones: string[] = [];
    try {
      let resultSet = await this.store.query(predicates);
      while (resultSet.goToNextRow()) {
        friendPhones.push(resultSet.getString(resultSet.getColumnIndex('friendPhone')));
      }
      resultSet.close();
    } catch (e) {
      console.error('获取好友手机号列表失败', JSON.stringify(e));
    }
    return friendPhones;
  }

}