/*
 * Copyright (c) 2025 Beijing Institute of Technology.
 *
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

securityLevel: relationalStore.SecurityLevel.S1
};

 * 鏁版嵁搴撳缓琛ㄨ鍙? * 鍖呭惈瀛楁锛氫富閿甶d, 鑽悕, 鏈夋晥鏈? 鍒嗙被, 鐢ㄦ硶, 浣嶇疆, 鍥剧墖, 鐢ㄦ埛ID
const SQL_CREATE_TABLE = `CREATE TABLE IF NOT EXISTS CONTACT_TABLE_V2 (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL,
  phone TEXT,
  company TEXT,
  position TEXT,
  email TEXT,
  image TEXT,
  userId INTEGER
)`;

 * RDB 鏁版嵁搴撶鐞嗗伐鍏风被
 * 灏佽浜嗗鍒犳敼鏌?(CRUD) 鎿嶄綔
 * 瀹炵幇浜嗗崟渚嬫ā寮? */
export class RdbStore {
  private rdbStore: relationalStore.RdbStore | null = null;
  private tableName: string = 'CONTACT_TABLE_V2';

  
  getRdbStore(context: common.Context): Promise<relationalStore.RdbStore> {
    return new Promise((resolve, reject) => {
      // 濡傛灉宸插垵濮嬪寲锛岀洿鎺ヨ繑鍥?      if (this.rdbStore) {
        console.info('[RdbStore] Store already initialized.');
        resolve(this.rdbStore);
        return;
      }

      console.info('[RdbStore] Start getting RdbStore...');
      // 璋冪敤绯荤粺 API 鑾峰彇鏁版嵁搴?      relationalStore.getRdbStore(context, STORE_CONFIG, (err, store) => {
        if (err) {
          console.error(`[RdbStore] Failed to get RdbStore. Code:${err.code}, Message:${err.message}`);
          reject(err);
        } else {
          console.info('[RdbStore] Get RdbStore successfully.');
          this.rdbStore = store;

          // 鎵ц寤鸿〃 SQL
          try {
            this.rdbStore.executeSql(SQL_CREATE_TABLE);
          } catch (e) {
            console.error('[RdbStore] Execute SQL error: ' + JSON.stringify(e));
          }

          // 妫€鏌ユ槸鍚︿负绌哄簱锛屽鏋滄槸鍒欐彃鍏ユ紨绀烘暟鎹?          this.queryAll(1).then(list => {
            if (list.length === 0) {
              console.info('[RdbStore] No data found, initializing default data...');
              this.initDefaultData();
            }
          })
          resolve(store);
        }
      });
    });
  }

  
  private initDefaultData() {
    let c1 = new Contact('999鎰熷啋鐏?, '2025-12-01');
    c1.company = '鎰熷啋鍙戠儳';
    c1.position = '涓€鏃ヤ笁娆?;
    c1.email = '鑽涓€灞?;
    c1.userId = 1;
    this.insert(c1);

    let c2 = new Contact('甯冩礇鑺紦閲婅兌鍥?, '2026-05-20');
    c2.company = '姝㈢棝閫€鐑?;
    c2.position = '鍙戠儳38.5搴︿互涓?;
    c2.email = '鑽浜屽眰';
    c2.userId = 1;
    this.insert(c2);

    let c3 = new Contact('闃胯帿瑗挎灄', '2024-11-15');
    c3.company = '娑堢値鎶楃敓绱?;
    c3.position = '楗悗鏈嶇敤';
    c3.email = '鎶藉眽';
    c3.userId = 1;
    this.insert(c3);
  }

  
  async insert(contact: Contact): Promise<number> {
    if (!this.rdbStore) {
      console.error('[RdbStore] Error: RdbStore is NOT initialized.');
      return -1;
    }

    // 鏋勫缓閿€煎
    const valueBucket: relationalStore.ValuesBucket = {
      name: contact.name,
      phone: contact.phone,
      company: contact.company,
      position: contact.position,
      email: contact.email,
      image: contact.image,
      userId: contact.userId
    };

    console.info(`[RdbStore] Inserting: ${contact.name}`);
    return this.rdbStore.insert(this.tableName, valueBucket);
  }

  
  update(contact: Contact): Promise<number> {
    if (!this.rdbStore) return Promise.resolve(-1);

    const valueBucket: relationalStore.ValuesBucket = {
      name: contact.name,
      phone: contact.phone,
      company: contact.company,
      position: contact.position,
      email: contact.email,
      image: contact.image
    };
    // 鏍规嵁 ID 鏇存柊
    let predicates = new relationalStore.RdbPredicates(this.tableName);
    predicates.equalTo('id', contact.id);
    return this.rdbStore.update(valueBucket, predicates);
  }

  
  delete(id: number): Promise<number> {
    if (!this.rdbStore) return Promise.resolve(-1);

    let predicates = new relationalStore.RdbPredicates(this.tableName);
    predicates.equalTo('id', id);
    return this.rdbStore.delete(predicates);
  }

  
  queryAll(userId: number): Promise<Array<Contact>> {
    if (!this.rdbStore) {
      return Promise.resolve([]);
    }

    let predicates = new relationalStore.RdbPredicates(this.tableName);
    predicates.equalTo('userId', userId);

    return new Promise((resolve, reject) => {
      this.rdbStore!.query(predicates, ['id', 'name', 'phone', 'company', 'position', 'email', 'image', 'userId'], (err, resultSet) => {
        if (err) {
          console.error(`[RdbStore] Query failed: ${err.message}`);
          reject(err);
        } else {
          let count = resultSet.rowCount;
          let result: Array<Contact> = [];
          if (count > 0) {
            resultSet.goToFirstRow();
            do {
              let contact = new Contact(
                resultSet.getString(resultSet.getColumnIndex('name')),
                resultSet.getString(resultSet.getColumnIndex('phone'))
              );
              contact.id = resultSet.getLong(resultSet.getColumnIndex('id'));
              contact.company = resultSet.getString(resultSet.getColumnIndex('company'));
              contact.position = resultSet.getString(resultSet.getColumnIndex('position'));
              contact.email = resultSet.getString(resultSet.getColumnIndex('email'));
              contact.image = resultSet.getString(resultSet.getColumnIndex('image'));
              contact.userId = resultSet.getLong(resultSet.getColumnIndex('userId'));
              result.push(contact);
            } while (resultSet.goToNextRow());
          }
          resultSet.close();
          resolve(result);
        }
      });
    });
  }

  
  search(keyword: string, userId: number): Promise<Array<Contact>> {
    if (!this.rdbStore) return Promise.resolve([]);

    let predicates = new relationalStore.RdbPredicates(this.tableName);
    // SQL: WHERE userId = ? AND name LIKE %keyword%
    predicates.equalTo('userId', userId).and().contains('name', keyword);

    return new Promise((resolve, reject) => {
      this.rdbStore!.query(predicates, ['id', 'name', 'phone', 'company', 'position', 'email', 'image', 'userId'], (err, resultSet) => {
        if (err) {
          reject(err);
        } else {
          let count = resultSet.rowCount;
          let result: Array<Contact> = [];
          if (count > 0) {
            resultSet.goToFirstRow();
            do {
              let contact = new Contact(
                resultSet.getString(resultSet.getColumnIndex('name')),
                resultSet.getString(resultSet.getColumnIndex('phone'))
              );
              contact.id = resultSet.getLong(resultSet.getColumnIndex('id'));
              contact.company = resultSet.getString(resultSet.getColumnIndex('company'));
              contact.position = resultSet.getString(resultSet.getColumnIndex('position'));
              contact.email = resultSet.getString(resultSet.getColumnIndex('email'));
              contact.image = resultSet.getString(resultSet.getColumnIndex('image'));
              contact.userId = resultSet.getLong(resultSet.getColumnIndex('userId'));
              result.push(contact);
            } while (resultSet.goToNextRow());
          }
          resultSet.close();
          resolve(result);
        }
      });
    });
  }
}

export default new RdbStore();

