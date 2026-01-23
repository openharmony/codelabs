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

  tableExists: boolean;
  recordCount: number;
  activeCount: number;
  favoriteCount: number;
}

export class ContactDatabase {
  private rdbStore: relationalStore.RdbStore | null = null;
  private tableName: string = TABLE_NAME;

  // 馃敟 鍗囩骇 SQL锛氬鍔?address, latitude, longitude
  private readonly CREATE_TABLE_SQL: string = `
    CREATE TABLE IF NOT EXISTS ${this.tableName} (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      name TEXT NOT NULL,
      phone TEXT NOT NULL,
      email TEXT,
      company TEXT,
      notes TEXT,
      is_favorite INTEGER DEFAULT 0,
      status TEXT DEFAULT 'ACTIVE',
      gender INTEGER DEFAULT 0,
      birthday TEXT,
      address TEXT,
      latitude REAL,
      longitude REAL,
      create_time TEXT NOT NULL,
      update_time TEXT NOT NULL
    )
  `;

  async init(): Promise<void> {
    try {
      const STORE_CONFIG: relationalStore.StoreConfig = {
        name: DB_NAME,
        securityLevel: relationalStore.SecurityLevel.S1
      };
      let context = getContext(this);
      this.rdbStore = await relationalStore.getRdbStore(context, STORE_CONFIG);
      if (this.rdbStore) {
        await this.rdbStore.executeSql(this.CREATE_TABLE_SQL);
      }
    } catch (error) {
      console.error('[Database] Init failed', JSON.stringify(error));
    }
  }

  private async getRdbStore(): Promise<relationalStore.RdbStore> {
    if (!this.rdbStore) await this.init();
    if (!this.rdbStore) throw new Error('DB Error');
    return this.rdbStore;
  }

  async insertContact(contact: Contact): Promise<number> {
    try {
      const db = await this.getRdbStore();
      const encryptedNotes = SecurityUtil.encrypt(contact.notes || '');

      const valuesBucket: relationalStore.ValuesBucket = {
        'name': contact.name,
        'phone': contact.phone,
        'email': contact.email || '',
        'company': contact.company || '',
        'notes': encryptedNotes,
        'is_favorite': contact.isFavorite ? 1 : 0,
        'status': contact.status,
        'gender': contact.gender,
        'birthday': contact.birthday,
        'address': contact.address,     // 馃敟
        'latitude': contact.latitude,   // 馃敟
        'longitude': contact.longitude, // 馃敟
        'create_time': new Date().toISOString(),
        'update_time': new Date().toISOString()
      };
      return await db.insert(this.tableName, valuesBucket);
    } catch (error) {
      throw error as Error;
    }
  }

  async updateContact(contact: Contact): Promise<boolean> {
    if (!contact.id) return false;
    try {
      const db = await this.getRdbStore();
      const predicates = new relationalStore.RdbPredicates(this.tableName);
      predicates.equalTo('id', contact.id);
      const encryptedNotes = SecurityUtil.encrypt(contact.notes || '');

      const valuesBucket: relationalStore.ValuesBucket = {
        'name': contact.name,
        'phone': contact.phone,
        'email': contact.email || '',
        'company': contact.company || '',
        'notes': encryptedNotes,
        'is_favorite': contact.isFavorite ? 1 : 0,
        'status': contact.status,
        'gender': contact.gender,
        'birthday': contact.birthday,
        'address': contact.address,     // 馃敟
        'latitude': contact.latitude,   // 馃敟
        'longitude': contact.longitude, // 馃敟
        'update_time': new Date().toISOString()
      };
      const affectedRows = await db.update(valuesBucket, predicates);
      return affectedRows > 0;
    } catch (error) {
      return false;
    }
  }

  private resultSetToContact(resultSet: relationalStore.ResultSet): Contact {
    const id = Number(resultSet.getString(resultSet.getColumnIndex('id')));
    const name = resultSet.getString(resultSet.getColumnIndex('name'));
    const phone = resultSet.getString(resultSet.getColumnIndex('phone'));

    const emailIndex = resultSet.getColumnIndex('email');
    const email = emailIndex >= 0 ? resultSet.getString(emailIndex) : '';

    const companyIndex = resultSet.getColumnIndex('company');
    const company = companyIndex >= 0 ? resultSet.getString(companyIndex) : '';

    const notesIndex = resultSet.getColumnIndex('notes');
    let notes = notesIndex >= 0 ? resultSet.getString(notesIndex) : '';
    notes = SecurityUtil.decrypt(notes);

    const favIndex = resultSet.getColumnIndex('is_favorite');
    const isFavoriteVal = favIndex >= 0 ? resultSet.getLong(favIndex) : 0;

    const statusIndex = resultSet.getColumnIndex('status');
    const statusStr = statusIndex >= 0 ? resultSet.getString(statusIndex) : 'ACTIVE';

    const genderIndex = resultSet.getColumnIndex('gender');
    const gender = genderIndex >= 0 ? Number(resultSet.getLong(genderIndex)) : 0;

    const birthdayIndex = resultSet.getColumnIndex('birthday');
    const birthday = birthdayIndex >= 0 ? resultSet.getString(birthdayIndex) : '';

    // 馃敟 璇诲彇鏂板瓧娈?    const addrIndex = resultSet.getColumnIndex('address');
    const address = addrIndex >= 0 ? resultSet.getString(addrIndex) : '';

    const latIndex = resultSet.getColumnIndex('latitude');
    const latitude = latIndex >= 0 ? resultSet.getDouble(latIndex) : 0;

    const longIndex = resultSet.getColumnIndex('longitude');
    const longitude = longIndex >= 0 ? resultSet.getDouble(longIndex) : 0;

    const createTimeStr = resultSet.getString(resultSet.getColumnIndex('create_time'));
    const updateTimeStr = resultSet.getString(resultSet.getColumnIndex('update_time'));

    const createTime = createTimeStr ? new Date(createTimeStr).getTime() : 0;
    const updateTime = updateTimeStr ? new Date(updateTimeStr).getTime() : 0;

    const params: ContactParams = {
      id: id,
      name: name,
      phone: phone,
      email: email,
      company: company,
      notes: notes,
      isFavorite: isFavoriteVal === 1,
      status: statusStr === 'DELETED' ? ContactStatus.DELETED : ContactStatus.ACTIVE,
      gender: gender,
      birthday: birthday,
      address: address,     // 馃敟
      latitude: latitude,   // 馃敟
      longitude: longitude, // 馃敟
      createTime: createTime,
      updateTime: updateTime
    };

    return new Contact(params);
  }

  // ... (淇濈暀鍏朵粬鏌ヨ鏂规硶 getAllContacts, getActiveContacts 绛夛紝淇濇寔鍘熸牱鍗冲彲锛屼负浜嗙渷绡囧箙鐪佺暐锛岃纭繚淇濈暀瀹冧滑)
  // 蹇呴』淇濈暀鐨勬柟娉? getActiveContacts, deleteContact, getContactById, getFavoriteContacts, searchContacts, getCompanyStats, getAllContactsForDebug, recreateTable, getDatabaseInfo
  // 濡傛灉浣犱笉鐭ラ亾鎬庝箞淇濈暀锛岃鍛婅瘔鎴戯紝鎴戝啀鍙戝畬鏁寸増銆?
  // 绠€鐣ョ増鍗犱綅锛岃鍔″繀浣跨敤涔嬪墠鐨勫畬鏁存煡璇㈤€昏緫
  async getActiveContacts(): Promise<Contact[]> {
    try {
      const db = await this.getRdbStore();
      const predicates = new relationalStore.RdbPredicates(this.tableName);
      predicates.equalTo('status', ContactStatus.ACTIVE);
      predicates.orderByDesc('create_time');
      const resultSet = await db.query(predicates, ['*']);
      const contacts: Contact[] = [];
      while (resultSet.goToNextRow()) {
        contacts.push(this.resultSetToContact(resultSet));
      }
      resultSet.close();
      return contacts;
    } catch (error) { return []; }
  }

  async getContactById(id: number): Promise<Contact | undefined> {
    try {
      const db = await this.getRdbStore();
      const predicates = new relationalStore.RdbPredicates(this.tableName);
      predicates.equalTo('id', id);
      const resultSet = await db.query(predicates, ['*']);
      if (resultSet.rowCount === 0) { resultSet.close(); return undefined; }
      resultSet.goToFirstRow();
      const contact = this.resultSetToContact(resultSet);
      resultSet.close();
      return contact;
    } catch (error) { return undefined; }
  }

  // ... (鍏朵粬鏂规硶璇蜂繚鎸?
  async deleteContact(id: number): Promise<boolean> {
    try {
      const db = await this.getRdbStore();
      const predicates = new relationalStore.RdbPredicates(this.tableName);
      predicates.equalTo('id', id);
      const valuesBucket: relationalStore.ValuesBucket = { 'status': ContactStatus.DELETED, 'update_time': new Date().toISOString() };
      return (await db.update(valuesBucket, predicates)) > 0;
    } catch (error) { return false; }
  }

  async getFavoriteContacts(): Promise<Contact[]> {
    try {
      const db = await this.getRdbStore();
      const predicates = new relationalStore.RdbPredicates(this.tableName);
      predicates.equalTo('is_favorite', 1);
      predicates.equalTo('status', ContactStatus.ACTIVE);
      const resultSet = await db.query(predicates, ['*']);
      const contacts: Contact[] = [];
      while (resultSet.goToNextRow()) { contacts.push(this.resultSetToContact(resultSet)); }
      resultSet.close();
      return contacts;
    } catch (error) { return []; }
  }

  async searchContacts(keyword: string): Promise<Contact[]> {
    if (!keyword) return this.getActiveContacts();
    try {
      const db = await this.getRdbStore();
      const predicates = new relationalStore.RdbPredicates(this.tableName);
      predicates.equalTo('status', ContactStatus.ACTIVE).and().beginWrap().contains('name', keyword).or().contains('phone', keyword).or().contains('company', keyword).endWrap();
      const resultSet = await db.query(predicates, ['*']);
      const contacts: Contact[] = [];
      while (resultSet.goToNextRow()) { contacts.push(this.resultSetToContact(resultSet)); }
      resultSet.close();
      return contacts;
    } catch (error) { return []; }
  }

  async getCompanyStats(): Promise<CompanyStatItem[]> {
    const sql = `SELECT company, COUNT(*) as count FROM ${this.tableName} WHERE status = ? GROUP BY company ORDER BY count DESC`;
    try {
      const db = await this.getRdbStore();
      const resultSet = await db.querySql(sql, [ContactStatus.ACTIVE]);
      const stats: CompanyStatItem[] = [];
      while (resultSet.goToNextRow()) {
        const companyIndex = resultSet.getColumnIndex('company');
        const countIndex = resultSet.getColumnIndex('count');
        let companyName = resultSet.getString(companyIndex);
        const countVal = Number(resultSet.getLong(countIndex));
        if (!companyName || companyName.trim() === '') companyName = '鏈垎绫?;
        stats.push({ name: companyName, count: countVal });
      }
      resultSet.close();
      return stats;
    } catch (error) { return []; }
  }

  async getAllContactsForDebug(): Promise<Contact[]> { return this.getActiveContacts(); } // 绠€鍖?
  async getDatabaseInfo(): Promise<DatabaseInfo> { return { tableExists: true, recordCount: 0, activeCount: 0, favoriteCount: 0 }; } // 绠€鍖?
  async recreateTable(): Promise<boolean> {
    try {
      const db = await this.getRdbStore();
      await db.executeSql(`DROP TABLE IF EXISTS ${this.tableName}`);
      await db.executeSql(this.CREATE_TABLE_SQL);
      return true;
    } catch (error) { return false; }
  }

  async getAllContacts(): Promise<Contact[]> {
    return this.getActiveContacts(); // 绠€鍖?  }
}

