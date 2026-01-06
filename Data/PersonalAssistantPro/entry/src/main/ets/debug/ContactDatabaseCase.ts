// File: entry/src/main/ets/debug/ContactDatabaseCase.ts
import { DebugCase } from './DebugCase';
import { Contact, ContactModel } from '../model/ContactModel';

export class ContactDatabaseCase extends DebugCase {
  readonly name = 'Contact Database CRUD Test';

  async run(): Promise<void> {
    this.logInfo('=== 开始联系人数据库测试 ===');

    // 1. 测试插入
    this.logInfo('Step 1: Inserting contacts...');
    const contact1 = new Contact('张三', '13800138000', '同事', 'zhangsan@example.com');
    const contact2 = new Contact('李四', '13900139000', '家人', 'lisi@home.com');

    const id1 = await ContactModel.insert(contact1);
    const id2 = await ContactModel.insert(contact2);

    if (id1 > 0 && id2 > 0) {
      this.logInfo(`✅ Insert success. IDs: ${id1}, ${id2}`);
    } else {
      this.logError('❌ Insert failed');
      return;
    }

    // 2. 测试查询
    this.logInfo('Step 2: Querying all contacts...');
    const list = await ContactModel.queryAll();
    this.logInfo(`Found ${list.length} contacts.`);
    list.forEach(c => {
      this.logInfo(` - [${c.id}] ${c.name} (${c.relation}): ${c.phone}`);
    });

    // 验证数据准确性
    const insertedZhang = list.find(c => c.name === '张三');
    if (insertedZhang && insertedZhang.phone === '13800138000') {
      this.logInfo('✅ Data verification passed (Zhang San found).');
    } else {
      this.logError('❌ Data verification failed!');
    }

    // 3. 测试删除
    this.logInfo(`Step 3: Deleting contact ID: ${id1}...`);
    const deleteSuccess = await ContactModel.deleteById(id1);
    if (deleteSuccess) {
      this.logInfo('✅ Delete success.');
    } else {
      this.logError('❌ Delete failed.');
    }

    // 4. 再次查询验证
    this.logInfo('Step 4: Final query check...');
    const finalList = await ContactModel.queryAll();
    this.logInfo(`Remaining contacts: ${finalList.length}`);
    if (finalList.length === list.length - 1) {
      this.logInfo('✅ Final count check passed.');
    } else {
      this.logError('❌ Final count check failed.');
    }

    this.logInfo('=== 联系人数据库测试结束 ===');
  }
}