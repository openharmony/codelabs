import { DebugCase } from './DebugCase';
import { ContactModel, Contact } from '../model/ContactModel';

/**
 * 通讯录性能压力测试
 * 场景：模拟短时间内插入 100 条数据，验证数据库稳定性和写入速度
 */
export class ContactStressTestCase extends DebugCase {
  readonly name = 'Contact Stress & Performance Test';

  async run(): Promise<void> {
    this.logInfo('=== ContactStressTestCase START ===');

    const BATCH_SIZE = 100; // 测试插入数量
    const timestamp = Date.now();

    this.logInfo(`Goal: Insert ${BATCH_SIZE} contacts sequentially.`);

    const startTime = Date.now();
    let successCount = 0;
    let failCount = 0;

    // 1. 批量插入循环
    for (let i = 0; i < BATCH_SIZE; i++) {
      // 构造测试数据
      const name = `StressUser_${i}_${timestamp}`;
      const phone = `138${String(i).padStart(8, '0')}`;
      const email = `stress${i}@test.com`;
      const relation = i % 2 === 0 ? 'Work' : 'Family';

      const contact = new Contact(name, phone, relation, email);

      try {
        const id = await ContactModel.insert(contact);
        if (id > 0) {
          successCount++;
        } else {
          failCount++;
          this.logInfo(`   ❌ Failed to insert index ${i}`);
        }
      } catch (e) {
        failCount++;
        this.logInfo(`   ❌ Exception at index ${i}: ${JSON.stringify(e)}`);
      }

      // 每 20 条输出一次日志，证明在运行
      if (i > 0 && i % 20 === 0) {
        this.logInfo(`   > Progress: ${i}/${BATCH_SIZE} ...`);
      }
    }

    const endTime = Date.now();
    const totalTime = endTime - startTime;
    const avgTime = totalTime / BATCH_SIZE;

    // 2. 输出性能报告
    this.logInfo('----------------------------------------');
    this.logInfo('📊 Performance Report:');
    this.logInfo(`   Total Time: ${totalTime} ms`);
    this.logInfo(`   Avg Time/Op: ${avgTime.toFixed(2)} ms`);
    this.logInfo(`   Success: ${successCount}`);
    this.logInfo(`   Failed: ${failCount}`);
    this.logInfo('----------------------------------------');

    // 3. 验证数据是否存在 (随机抽查)
    const checkIndex = Math.floor(Math.random() * BATCH_SIZE);
    const checkName = `StressUser_${checkIndex}_${timestamp}`;
    const results = await ContactModel.search(checkName);

    if (results.length > 0) {
      this.logInfo(`✅ Verification Passed: Found ${checkName}`);
    } else {
      this.logInfo(`❌ Verification Failed: Could not find ${checkName}`);
    }

    // 4. 清理数据 (可选，为了保持环境干净，演示删除逻辑)
    // 这里我们演示批量删除搜索到的数据
    this.logInfo('4. Cleaning up stress test data...');
    // 注意：实际项目中可能需要一个 deleteByLike 或循环删除，这里简单演示只删抽查到的那个
    if (results.length > 0) {
      await ContactModel.deleteById(results[0].id);
      this.logInfo('   > Cleaned up sample record.');
    }

    this.logInfo('=== ContactStressTestCase END ===');
  }
}