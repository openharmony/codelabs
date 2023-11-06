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
import CommonConstants from '../common/constants/CommonConstants';
import Logger from '../common/utils/Logger';
import { User } from '../viewmodel/User';

export class RdbModel {
  private rdbStore: dataRdb.RdbStore | null = null;
  private tableName: string = '';
  private sqlCreateTable: string = '';
  private columns: Array<string> = [];

  constructor(tableName: string, sqlCreateTable: string, columns: Array<string>) {
    this.tableName = tableName;
    this.sqlCreateTable = sqlCreateTable;
    this.columns = columns;
    this.getRdbStore();
  }

  /**
   * Obtaining rdbStore.
   */
  getRdbStore() {
    let getPromiseRdb = dataRdb.getRdbStore(getContext(), { name: CommonConstants.DATABASE_NAME, securityLevel: dataRdb.SecurityLevel.S1 });
    getPromiseRdb.then(rdbStore => {
      this.rdbStore = rdbStore;
      this.rdbStore.executeSql(this.sqlCreateTable);
    }).catch((err: Error) => {
      Logger.error(`getRdbStore err ${JSON.stringify(err)}`);
    });
  }

  /**
   * Save data to the database.
   *
   * @param user Data objects of the user type to be saved.
   */
  insertData(user: User) {
    try {
      (this.rdbStore as dataRdb.RdbStore).insert(this.tableName, JSON.parse(JSON.stringify(user)));
    } catch (err) {
      Logger.error(`insert data failed due to ${JSON.stringify(err)}`);
    }
  }

  /**
   * Querying the database based on predicate.
   *
   * @param predicates Query condition.
   * @returns resultList Query result set.
   */
  async query(predicates: dataRdb.RdbPredicates): Promise<dataRdb.ResultSet>{
    let resultList: null | dataRdb.ResultSet = null;
    try {
      resultList = await (this.rdbStore as dataRdb.RdbStore).query(predicates, this.columns);
    } catch (err) {
      Logger.error(`query data failed due to ${JSON.stringify(err)}`);
    }
    return resultList as dataRdb.ResultSet;
  }
}