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
// entry/src/main/ets/debug/BackupRestoreDebugCase.ts
import { DebugCase } from './DebugCase';
import { BackupService } from '../services/BackupService';
import { ContactModel, Contact } from '../model/ContactModel';
import { EventModel, Event } from '../model/EventModel';

/**
 * 备份与恢复功能测试用例 (F3)
 * 流程：插入测试数据 -> 执行备份 -> 删除测试数据 -> 执行恢复 -> 验证数据是否存在
 */
export class BackupRestoreDebugCase extends DebugCase {
  readonly name = 'Backup & Restore Integration Test';

  async run(): Promise<void> {
    this.logInfo('=== BackupRestoreDebugCase START ===');

    // 1. 准备测试数据 (标记为 [BACKUP_TEST])
    const uniqueTag = '[BACKUP_TEST]';
    const testContact = new Contact(`${uniqueTag} User`, '13800009999', 'Test', 'backup@test.com');
    const testEvent = new Event(`${uniqueTag} Meeting`, Date.now(), Date.now() + 3600000, 'Test Backup');

    // 获取 Context (DebugCase 运行在 Ability 环境中，可以通过 globalThis 或 AppStorage 获取，或者假定 Service 内部处理)
    // 注意：BackupService 需要 Context。我们在 EntryAbility 调用时要把 Context 传进去，
    // 或者这里我们假设 BackupService.getInstance().backup(context) 需要 context。
    // 为了简化，我们假设 EntryAbility 里已经初始化了某些全局 Context，或者直接通过 getContext() 获取 (如果是 UI 组件)。
    // **修正**：由于 DebugCase 是纯类，无法直接使用 getContext()。
    // 我们需要在调用 run 之前或者通过某种方式传入 Context。
    // 观察你的 EntryAbility 代码，DebugRunner 运行在 onCreate，有 this.context。
    // 我们可以让 DebugRunner 传递 context，或者暂时使用 ApplicationContext。

    // *为了代码能跑，我们假设 DebugRunner 会被传入 context，或者我们在 EntryAbility 里把 context 挂到了 AppStorage*
    // 这里暂时使用 AppStorage 获取 context (需要在 EntryAbility onCreate 里存一下)
    // 或者更简单的：直接测试逻辑，假设 context 已就绪。
    // *根据你之前的 BackupService 代码，它需要传入 context。*

    // 为了不修改 DebugRunner 架构，我们这里使用 globalThis 里的 context (需要在 EntryAbility 挂载)
    // 或者，我们可以只测试 "数据生成" 和 "解析" 的逻辑，跳过文件 IO?
    // 不，文件 IO 是关键。

    // === 解决方案 ===
    // 请在 EntryAbility.ets 的 onCreate 中添加: AppStorage.setOrCreate('abilityContext', this.context);
    const context = AppStorage.get('abilityContext') as Context;

    if (!context) {
      this.logInfo('❌ Context not found in AppStorage. Please set "abilityContext" in EntryAbility.');
      return;
    }

    try {
      // Step 1: 插入数据
      this.logInfo('1. Inserting test data...');
      const contactId = await ContactModel.insert(testContact);
      const eventId = await EventModel.insert(testEvent);
      this.logInfo(`   > Inserted Contact ID: ${contactId}, Event ID: ${eventId}`);

      // Step 2: 执行备份
      this.logInfo('2. Executing Backup...');
      const backupPath = await BackupService.getInstance().backup(context);
      this.logInfo(`   > Backup file created at: ${backupPath}`);

      // Step 3: 模拟数据丢失 (删除刚才插入的数据)
      this.logInfo('3. Deleting test data (Simulating data loss)...');
      await ContactModel.deleteById(contactId);
      await EventModel.deleteById(eventId);

      // 验证删除
      const searchDeleted = await ContactModel.search(uniqueTag);
      if (searchDeleted.length === 0) {
        this.logInfo('   > Data deleted successfully.');
      } else {
        this.logInfo('   ❌ Data deletion failed, stopping test.');
        return;
      }

      // Step 4: 执行恢复
      this.logInfo('4. Executing Restore...');
      const restoreCount = await BackupService.getInstance().restore(context);
      this.logInfo(`   > Restored items count: ${restoreCount}`);

      // Step 5: 验证数据是否回来
      this.logInfo('5. Verifying restored data...');
      const restoredContacts = await ContactModel.search(uniqueTag);
      const restoredEvents = await EventModel.search(uniqueTag);

      if (restoredContacts.length > 0 && restoredEvents.length > 0) {
        this.logInfo(`   ✅ Verification SUCCESS! Found ${restoredContacts.length} contacts and ${restoredEvents.length} events with tag "${uniqueTag}".`);
        this.logInfo(`   > Restored Contact: ${restoredContacts[0].name}`);
      } else {
        this.logInfo('   ❌ Verification FAILED. Data not found.');
      }

      // Step 6: 清理 (可选)
      // await ContactModel.deleteById(restoredContacts[0].id);

    } catch (err) {
      this.logInfo(`❌ Test Exception: ${JSON.stringify(err)}`);
    }

    this.logInfo('=== BackupRestoreDebugCase END ===');
  }
}