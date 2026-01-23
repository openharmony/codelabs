/*
 * Copyright (c) 2026 Beijing Institute of Technology.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE--2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

}

export class ContactViewModel {
  // ... (灞炴€т繚鎸佷笉鍙?
  private contactDatabase: ContactDatabase = new ContactDatabase();
  private contacts: Contact[] = [];
  private isInitialized: boolean = false;

  constructor() {
    console.info('[ContactViewModel] 鏋勯€犲嚱鏁拌皟鐢?);
    this.initDatabase();
  }

  // ... (涓棿鐨勬柟娉曞 initDatabase, loadContacts, addContact 绛変繚鎸佷笉鍙橈紝鍙互鐩存帴澶嶅埗浠ュ墠鐨勶紝鎴栬€呭彧瑕佽鐩?addTestContacts)
  // 涓鸿妭鐪佺瘒骞咃紝杩欓噷鍋囪浣犱繚鐣欎簡涓棿鐨勬柟娉曪紝鍙浛鎹?addTestContacts
  // 濡傛灉浣犻渶瑕佸畬鏁存枃浠讹紝璇峰憡璇夋垜锛屾垜鍐嶅彂瀹屾暣鐨?
  private async initDatabase(): Promise<void> {
    if (this.isInitialized) {
      console.info('[ContactViewModel] 鏁版嵁搴撳凡鍒濆鍖栵紝璺宠繃');
      return;
    }
    console.info('[ContactViewModel] 寮€濮嬪垵濮嬪寲鏁版嵁搴?);
    try {
      await this.contactDatabase.init();
      console.info('[ContactViewModel] 鏁版嵁搴撳垵濮嬪寲鎴愬姛');
      await this.loadContacts();
      console.info('[ContactViewModel] 鑱旂郴浜哄姞杞藉畬鎴?);
      this.isInitialized = true;
    } catch (error) {
      console.error('[ContactViewModel] 鏁版嵁搴撳垵濮嬪寲澶辫触:', JSON.stringify(error));
      const err = new Error('ViewModel鍒濆鍖栧け璐?);
      err.name = 'ViewModelInitError';
      throw err;
    }
  }

  async loadContacts(): Promise<void> {
    try {
      this.contacts = await this.contactDatabase.getActiveContacts();
    } catch (error) {
      this.contacts = [];
    }
  }

  async addContact(contact: Contact): Promise<number> {
    if (!this.isInitialized) await this.initDatabase();
    try {
      // 淇锛氭樉寮忔瀯閫犲弬鏁板璞?      const params: ContactParams = {
        name: contact.name,
        phone: contact.phone,
        email: contact.email,
        company: contact.company,
        notes: contact.notes,
        isFavorite: contact.isFavorite,
        status: contact.status,
        gender: contact.gender,
        birthday: contact.birthday
      };

      const contactToAdd = new Contact(params);
      const insertedId = await this.contactDatabase.insertContact(contactToAdd);
      await this.loadContacts();
      return insertedId;
    } catch (error) {
      throw new Error('AddContactError');
    }
  }

  // ... (updateContact, deleteContact, getContactById 绛夋柟娉曚繚鎸佷笉鍙?
  async updateContact(contact: Contact): Promise<boolean> {
    if (!contact.id) return false;
    try {
      const success = await this.contactDatabase.updateContact(contact);
      if (success) await this.loadContacts();
      return success;
    } catch (error) { return false; }
  }

  async deleteContact(id: number): Promise<boolean> {
    try {
      const success = await this.contactDatabase.deleteContact(id);
      if (success) await this.loadContacts();
      return success;
    } catch (error) { return false; }
  }

  async getContactById(id: number): Promise<Contact | undefined> {
    if (!this.isInitialized) await this.initDatabase();
    try { return await this.contactDatabase.getContactById(id); }
    catch (error) { return undefined; }
  }

  async getContacts(): Promise<Contact[]> {
    if (!this.isInitialized) await this.initDatabase();
    return this.contacts;
  }

  async getFavoriteContacts(): Promise<Contact[]> {
    if (!this.isInitialized) await this.initDatabase();
    try { return await this.contactDatabase.getFavoriteContacts(); }
    catch (error) { return []; }
  }

  async searchContacts(keyword: string): Promise<Contact[]> {
    if (!this.isInitialized) await this.initDatabase();
    try { return await this.contactDatabase.searchContacts(keyword); }
    catch (error) { return []; }
  }

  async getCompanyStats(): Promise<CompanyStatItem[]> {
    if (!this.isInitialized) await this.initDatabase();
    try { return await this.contactDatabase.getCompanyStats(); }
    catch (error) { return []; }
  }

  async toggleFavorite(id: number): Promise<boolean> {
    try {
      const contact = await this.getContactById(id);
      if (!contact) return false;
      contact.isFavorite = !contact.isFavorite;
      return await this.updateContact(contact);
    } catch (error) { return false; }
  }

  async getAllContactsForDebug(): Promise<Contact[]> {
    if (!this.isInitialized) await this.initDatabase();
    try { return await this.contactDatabase.getAllContactsForDebug(); }
    catch (error) { return []; }
  }

  async getDatabaseInfo(): Promise<DatabaseInfo> {
    if (!this.isInitialized) await this.initDatabase();
    try { return await this.contactDatabase.getDatabaseInfo(); }
    catch (error) { return { tableExists: false, recordCount: 0, activeCount: 0, favoriteCount: 0 }; }
  }

  async recreateTable(): Promise<boolean> {
    try {
      const success = await this.contactDatabase.recreateTable();
      if (success) {
        this.isInitialized = false;
        this.contacts = [];
        await this.initDatabase();
      }
      return success;
    } catch (error) { return false; }
  }

  // 馃敟 淇閲嶇偣锛歛ddTestContacts 浣跨敤鏂扮殑鎺ュ彛鏂瑰紡
  async addTestContacts(): Promise<void> {
    console.info('[ContactViewModel] 娣诲姞娴嬭瘯鑱旂郴浜?);

    // 淇锛氭樉寮忓畾涔夊弬鏁板璞★紝涓嶄娇鐢ㄥ尶鍚嶅璞?    const p1: ContactParams = {
      name: '寮犱笁', phone: '13800138000', email: 'zhangsan@example.com',
      company: '鍗庝负鎶€鏈湁闄愬叕鍙?, notes: '鎶€鏈€荤洃', isFavorite: true, gender: 1, birthday: '1990-01-01'
    };
    const p2: ContactParams = {
      name: '鏉庡洓', phone: '13900139000', email: 'lisi@example.com',
      company: '闃块噷宸村反闆嗗洟', notes: '浜у搧缁忕悊', isFavorite: false, gender: 2, birthday: '1992-05-20'
    };
    const p3: ContactParams = {
      name: '鐜嬩簲', phone: '13700137000', email: 'wangwu@example.com',
      company: '鑵捐绉戞妧', notes: '杞欢宸ョ▼甯?, isFavorite: true, gender: 1, birthday: '1995-11-11'
    };

    const testContacts = [
      new Contact(p1),
      new Contact(p2),
      new Contact(p3)
    ];

    try {
      for (const contact of testContacts) {
        await this.addContact(contact);
      }
      console.info('[ContactViewModel] 娴嬭瘯鑱旂郴浜烘坊鍔犲畬鎴?);
    } catch (error) {
      console.error('[ContactViewModel] 娣诲姞娴嬭瘯鑱旂郴浜哄け璐?', JSON.stringify(error));
    }
  }
}



