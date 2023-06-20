/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import dataRdb from '@ohos.data.relationalStore';
import { User } from '../common/bean/User';
import { RdbModel } from '../model/RdbModel'
import CommonConstants from '../common/constants/CommonConstants';
import Logger from '../common/utils/Logger';

const TABLE_NAME = CommonConstants.TABLE_NAME;
const CREATE_USER_TABLE = CommonConstants.CREATE_TABLE_SQL;
const COLUMNS = CommonConstants.TABLE_COLUMNS;

export class UserTableApi {
  private userTable = new RdbModel(TABLE_NAME, CREATE_USER_TABLE, COLUMNS);

  /**
   * Save data to the database.
   *
   * @param user Data objects of the user type to be saved.
   */
  insertUserData(user: User) {
    this.userTable.insertData(user);
  }

  /**
   * Querying user information by user name.
   *
   * @param username Querying user name.
   * @returns Query result set.
   */
  async queryUserByUsername(username: string): Promise<User[]> {
    let resultList: Array<User>;
    let predicates = new dataRdb.RdbPredicates(TABLE_NAME);
    predicates.equalTo('username', username);
    let ret = await this.userTable.query(predicates);
    resultList = this.getListFromResultSet(ret);
    return resultList;
  }

  /**
   * Encapsulates the query result into the user information corresponding to the user.
   *
   * @param resultSet Query result set.
   * @returns Result set of the user object type.
   */
  getListFromResultSet(resultSet: dataRdb.ResultSet): User[] {
    if (!resultSet) {
      Logger.error(`resultSet is null or undefined`);
    }
    let userList = [];
    for (let i = 0; i < resultSet.rowCount; i++) {
      resultSet.goToNextRow();
      let user = new User(resultSet.getDouble(resultSet.getColumnIndex('id')),
        resultSet.getString(resultSet.getColumnIndex('username')),
        resultSet.getString(resultSet.getColumnIndex('password')),
        resultSet.getString(resultSet.getColumnIndex('authTag')));
      userList.push(user);
    }
    return userList;
  }
}