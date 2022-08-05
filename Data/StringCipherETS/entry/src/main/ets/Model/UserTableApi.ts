/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

import dataRdb from '@ohos.data.rdb'
import { User } from '../model/User'
import { RdbModel } from '../model/RdbModel'

const TABLE_NAME = 'user';
const CREATE_TABLE_USER = 'CREATE TABLE IF NOT EXISTS user(' +
'id INTEGER PRIMARY KEY AUTOINCREMENT, ' +
'username TEXT NOT NULL, ' +
'nickname TEXT NOT NULL, ' +
'password TEXT NOT NULL)';
const COLUMNS = ['id', 'username', 'nickname', 'password'];

export class UserTableApi {
    private userTable = new RdbModel(TABLE_NAME, CREATE_TABLE_USER, COLUMNS)

    constructor() {
    }

    // 插入数据
    async insertUserData(user) {
        await this.userTable.getRdbStore().then(async () => {
            this.userTable.insertData(user);
        });
    }

    // 根据用户名查询用户信息
    async queryUserByUsername(username) {
        let resultList;
        await this.userTable.getRdbStore().then(async () => {
            let predicates = new dataRdb.RdbPredicates(TABLE_NAME);
            predicates.equalTo('username', username);
            let ret = await this.userTable.query(predicates);
            resultList = getListFromResultSet(ret);
        });
        return resultList;
    }
}

// 将查询到的结果封装成User对应的用户信息
function getListFromResultSet(resultSet) {
    let userList = [];
    for (let i = 0; i < resultSet.rowCount; i++) {
        resultSet.goToNextRow();
        let user = new User(resultSet.getDouble(resultSet.getColumnIndex('id')),
            resultSet.getString(resultSet.getColumnIndex('username')),
            resultSet.getString(resultSet.getColumnIndex('nickname')),
            resultSet.getString(resultSet.getColumnIndex('password')));
        userList.push(user);
    }
    return userList;
}
