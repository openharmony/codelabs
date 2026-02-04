/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
// File: entry/src/main/ets/debug/SearchDebugCase.ts
import { DebugCase } from './DebugCase';
import { Contact, ContactModel } from '../model/ContactModel';
import { Event, EventModel } from '../model/EventModel';

export class SearchDebugCase extends DebugCase {
  readonly name = 'Search Functionality Test (F1)';

  async run(): Promise<void> {
    this.logInfo('=== SearchDebugCase START ===');

    // 1. 准备测试数据 (为了确保搜索能搜到东西，我们先插入一些带有特殊标记的数据)
    const uniqueKey = 'Skywalker'; // 特殊关键词
    const testContact = new Contact(`${uniqueKey} Test`, '13800000000', 'DebugFriend', 'luke@force.com');
    const testEvent = new Event(`${uniqueKey} Meeting`, Date.now(), Date.now() + 3600000, 'Discuss the Force');

    this.logInfo(`1. Inserting test data with keyword: "${uniqueKey}"...`);

    const contactId = await ContactModel.insert(testContact);
    const eventId = await EventModel.insert(testEvent);
    this.logInfo(`   > Contact Inserted ID: ${contactId}`);
    this.logInfo(`   > Event Inserted ID: ${eventId}`);

    // 2. 测试联系人搜索
    this.logInfo('--------------------------------');
    this.logInfo(`2. Testing Contact Search (Keyword: "${uniqueKey}")`);
    try {
      const contacts = await ContactModel.search(uniqueKey);
      this.logInfo(`   > Search Result Count: ${contacts.length}`);
      contacts.forEach((c) => {
        this.logInfo(`   > Found: [${c.id}] ${c.name} | ${c.relation} | ${c.email}`);
      });

      if (contacts.some(c => c.name.includes(uniqueKey))) {
        this.logInfo('    Contact Search Passed');
      } else {
        this.logInfo('   ❌ Contact Search Failed (Target not found)');
      }
    } catch (err) {
      this.logInfo('   ❌ Contact Search Error:', JSON.stringify(err));
    }

    // 3. 测试日程搜索
    this.logInfo('--------------------------------');
    this.logInfo(`3. Testing Event Search (Keyword: "Discuss")`);
    // 注意：我们搜 Description 里的词 "Discuss"
    try {
      const events = await EventModel.search('Discuss');
      this.logInfo(`   > Search Result Count: ${events.length}`);
      events.forEach((e) => {
        this.logInfo(`   > Found: [${e.id}] ${e.title} | ${e.description}`);
      });

      if (events.length > 0) {
        this.logInfo('    Event Search Passed');
      } else {
        this.logInfo('   ❌ Event Search Failed');
      }
    } catch (err) {
      this.logInfo('   ❌ Event Search Error:', JSON.stringify(err));
    }

    // 4. 清理数据 (可选，避免污染数据库，但为了查看效果也可以不删)
    this.logInfo('--------------------------------');
    this.logInfo('4. Cleaning up test data...');
    await ContactModel.deleteById(contactId);
    await EventModel.deleteById(eventId);
    this.logInfo('   > Cleanup done.');

    this.logInfo('=== SearchDebugCase END ===');
  }
}