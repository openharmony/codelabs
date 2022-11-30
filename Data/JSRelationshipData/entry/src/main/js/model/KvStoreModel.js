/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

import dataAbility from '@ohos.data.dataAbility';
import ohos_data_rdb from '@ohos.data.rdb';

const STORE_CONFIG = { name: "RdbTest.db" }
const SQL_CREATE_TABLE = "CREATE TABLE IF NOT EXISTS EMPLOYEE (ID INTEGER PRIMARY KEY AUTOINCREMENT, NAME TEXT NOT NULL, AGE INTEGER, GENDER TEXT NOT NULL)"

export default class KvStoreModel {
    rdbStore;

    createKvStore(callback) {
       if (typeof (this.rdbStore) === 'undefined') {
           let self = this;
           let promise = dataAbility.getRdbStore(STORE_CONFIG, 1)
           promise.then(async (rdbStore) => {
               self.rdbStore = rdbStore;
               await rdbStore.executeSql(SQL_CREATE_TABLE, null);
               console.info("rdbStore" + 'create table done.')
               callback();
           }).catch((err) => {
               console.info("kvStore" + err)
               callback();
           })
       }else {
           callback();
       }
    }

    insertValue(valueBucket) {
        this.createKvStore(() => {
            this.insert(valueBucket);
        })
    }

    updateValue(valueBucket,index) {
        this.createKvStore(() => {
            this.update(valueBucket,index);
        })
    }

    deleteValue(index){
        this.createKvStore(() => {
            this.update(index);
        })
    }

    queryValue(callback) {
        this.createKvStore(() => {
            var contactList = []
            let predicates = new ohos_data_rdb.RdbPredicates("EMPLOYEE")
            let promise = this.rdbStore.query(predicates, ["ID", "NAME", "AGE", "GENDER"])
            console.log("rdbStore query start")
            promise.then((resultSet) => {
                if (resultSet.rowCount > 0) {
                    while (resultSet.goToNextRow()) {
                        let id = resultSet.getLong(resultSet.getColumnIndex("ID"));
                        let userName = resultSet.getString(resultSet.getColumnIndex("NAME"));
                        let userAge = resultSet.getLong(resultSet.getColumnIndex("AGE"));
                        let userGender = resultSet.getLong(resultSet.getColumnIndex("GENDER"));
                        const obj = {
                            id:id,name:userName,age:userAge,gender:userGender
                        };
                        contactList.push(obj);
                    }
                }
                resultSet.close();
                resultSet = null;
                callback(contactList);
                console.log("resultSet column names:" + resultSet.columnNames)
                console.log("resultSet column count:" + resultSet.columnCount)}
            )
        })
    }

    insert(valueBucket) {
        console.info('rdbStore.insert ' +valueBucket[0].name+"---"+valueBucket[0].gender);
        let promise = this.rdbStore.insert("EMPLOYEE", valueBucket)
        promise.then(async (rows) => {
            await console.log("rdbStore.insert first done: " + rows)
        }).catch((err) => {})
    }

    update(valueBucket,index){
        console.info('rdbStore.update ' +index+"=="+valueBucket);
        let predicates = new ohos_data_rdb.RdbPredicates("EMPLOYEE");
        predicates.equalTo("ID", index);
        this.rdbStore.update(valueBucket, predicates, function (err, rows) {
            console.log("rdbStore.update updated row count: " + rows)})
    }

    delete(index) {
        console.info('rdbStore.delete ' +index);
        let predicates = new ohos_data_rdb.RdbPredicates("EMPLOYEE");
        predicates.equalTo("ID", index);
        this.rdbStore.delete(predicates, function (err, rows) {
            console.log("rdbStore.delete rows: " + rows)})
    }
}