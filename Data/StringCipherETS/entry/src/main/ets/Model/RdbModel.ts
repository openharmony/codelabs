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
import prompt from '@system.prompt'

export class RdbModel {
    private rdbStore: any = null
    private promiseExecSql: any = null
    private tableName: string = ''
    private sqlCreateTable: string = ''
    private columns: Array<string> = []
    private STORE_CONFIG = { name: 'user.db', encryptKey: null }

    constructor(tableName: string, sqlCreateTable: string, columns: Array<string>) {
        this.tableName = tableName
        this.sqlCreateTable = sqlCreateTable
        this.columns = columns
    }

    // 获取rdbStore
    async getRdbStore() {
        if (this.rdbStore != null) {
            return this.rdbStore.executeSql(this.sqlCreateTable);
        }
        let getPromiseRdb = dataRdb.getRdbStore(globalThis.context, this.STORE_CONFIG, 1);
        await getPromiseRdb.then(async (rdbStore) => {
            this.rdbStore = rdbStore;
            this.promiseExecSql = rdbStore.executeSql(this.sqlCreateTable);
        }).catch((err) => {
            console.log("getRdbStore err." + JSON.stringify(err));
        });
        return this.promiseExecSql;
    }

    // 插入数据
    async insertData(user) {
        const valueBucket = JSON.parse(JSON.stringify(user));
        await this.rdbStore.insert(this.tableName, valueBucket, function (err, ret) {
            console.log('insert done: ' + ret);
        });
    }

    // 更新数据
    updateData(user) {
        const valueBucket = JSON.parse(JSON.stringify(user));
        let predicates = new dataRdb.RdbPredicates(this.tableName);
        predicates.equalTo('id', user.id);
        this.rdbStore.update(valueBucket, predicates, function (err, ret) {
            prompt.showToast({ message: 'updated row done:' + ret });
        });
    }

    // 删除数据
    deleteById(user) {
        let predicates = new dataRdb.RdbPredicates(this.tableName);
        predicates.equalTo('id', user.id);
        this.rdbStore.delete(predicates, function (err, rows) {
            prompt.showToast({ message: 'delete user' + rows });
        });
    }

    // 查询数据
    async query(predicates) {
        let resultList;
        let promiseQuery = this.rdbStore.query(predicates, this.columns);
        await promiseQuery.then((resultSet) => {
            resultList = resultSet;
        }).catch((err) => {
            console.log("query err" + JSON.stringify(err));
        });
        return resultList;
    }
}