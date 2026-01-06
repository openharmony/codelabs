// entry/src/main/ets/debug/EncryptionPrivacyCase.ts
import { DebugCase } from './DebugCase';
import { ContactModel, Contact } from '../model/ContactModel';
import { RdbHelper } from '../common/database/RdbHelper';
import { TABLE_CONTACT } from '../common/database/Tables';
import { AesCryptoUtils } from '../common/utils/AesCryptoUtils';
import rdb from '@ohos.data.relationalStore';

export class EncryptionPrivacyCase extends DebugCase {
  readonly name = 'F6 Privacy Encryption Check';

  async run(): Promise<void> {
    this.logInfo('=== 🔒 开始隐私加密专项测试 (Static Mode) ===');

    // ✅ 修复：直接调用静态方法，去掉 getInstance()
    await AesCryptoUtils.initKey();

    const rawPhone = '138-SECRET-888';
    // 使用 ContactModel.ts 中定义的 Contact 类
    const testContact = new Contact('加密测试员', rawPhone, '测试', 'security@test.com');

    // ---------------------------------------------------------
    // Step 1: 静态调用 insert
    // ---------------------------------------------------------
    this.logInfo('Step 1: Inserting data via ContactModel...');
    const id = await ContactModel.insert(testContact);

    if (id <= 0) {
      this.logError('❌ Setup failed: Could not insert contact.');
      return;
    }
    this.logInfo(`✅ Inserted successfully. ID: ${id}`);

    // ---------------------------------------------------------
    // Step 2: 直接查库 (验证加密)
    // ---------------------------------------------------------
    this.logInfo('Step 2: Inspecting Raw DB Data...');
    try {
      const predicates = new rdb.RdbPredicates(TABLE_CONTACT);
      predicates.equalTo('id', id);
      const resultSet = await RdbHelper.getInstance().query(predicates);

      if (resultSet.goToNextRow()) {
        const storedPhone = resultSet.getString(resultSet.getColumnIndex('phone'));
        this.logInfo(`   [Expected Plaintext]: ${rawPhone}`);
        this.logInfo(`   [Actual DB Value]   : ${storedPhone}`);

        // 验证逻辑：不等于明文 且 包含冒号(IV格式)
        if (storedPhone !== rawPhone && storedPhone.includes(':')) {
          this.logInfo('✅ 验证通过：数据库中存储的是加密密文 (Ciphertext)。');
        } else {
          this.logError('❌ 验证失败：数据库中数据未加密或格式错误！');
        }
      } else {
        this.logError('❌ Error: Could not find the inserted record in DB.');
      }
      resultSet.close();
    } catch (e) {
      this.logError('❌ DB Inspection Error:', e);
    }

    // ---------------------------------------------------------
    // Step 3: 静态调用 queryAll (验证解密)
    // ---------------------------------------------------------
    this.logInfo('Step 3: Reading back via ContactModel...');
    const allContacts = await ContactModel.queryAll();
    const retrievedContact = allContacts.find(c => c.id === id);

    if (retrievedContact && retrievedContact.phone === rawPhone) {
      this.logInfo(`✅ 解密验证成功: 读取结果为 ${retrievedContact.phone}`);
    } else {
      this.logError(`❌ 解密失败: 期望 ${rawPhone}, 实际读取到 ${retrievedContact?.phone}`);
    }

    // 4. 清理数据
    this.logInfo('Step 4: Cleaning up...');
    await ContactModel.deleteById(id);

    this.logInfo('=== 🔒 加密测试结束 ===');
  }
}