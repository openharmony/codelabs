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
import { DebugCase } from './DebugCase';
import { AesCryptoUtils } from '../common/utils/AesCryptoUtils';

/**
 * 安全模块压力测试
 * 目的：验证 AES 加密解密在各种极端输入下的准确性与性能
 */
export class SecurityStressTestCase extends DebugCase {
  readonly name = 'Security & Encryption Stress Test';

  // 准备海量测试数据 (特殊字符、多语言、边界情况)
  private testPayloads: string[] = [
    'Hello World',
    '1234567890',
    '', // 空字符串
    ' ', // 空格
    'Test_With_Underscore',
    'Test-With-Dash',
    '中文测试',
    '这是一个非常非常非常非常长的中文句子用于测试加密算法的长度处理能力',
    'Mixed Content 混合内容 123',
    '特殊符号 !@#$%^&*()_+-=[]{}|;:,.<>?/',
    'Emoji测试 😀😁😂🤣😃😄😅😆',
    'More Emoji 🚀🚁🚂🚃🚄🚅',
    'Line Break \n New Line',
    'Tab \t Character',
    'Single Quote \' ',
    'Double Quote " ',
    'Backslash \\ ',
    'SQL Injection Simulation: OR 1=1; DROP TABLE users;',
    'XSS Simulation: <script>alert(1)</script>',
    'JSON String: {"name": "test", "val": 123}',
    'Long Number: 9999999999999999999999999',
    'Float: 3.14159265358979323846',
    'Undefined String',
    'Null String',
    'Lorem ipsum dolor sit amet, consectetur adipiscing elit.',
    'Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.',
    'Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris.',
    'Duis aute irure dolor in reprehenderit in voluptate velit esse.',
    'Excepteur sint occaecat cupidatat non proident, sunt in culpa.',
    '1', // 极短
    'a',
    '0',
    'false',
    'true',
    'undefined',
    'null',
    'NaN'
  ];

  async run(): Promise<void> {
    this.logInfo('=== SecurityStressTestCase START ===');
    this.logInfo(`Total payloads to test: ${this.testPayloads.length}`);

    let successCount = 0;
    let failCount = 0;
    const startTime = Date.now();

    // 循环测试每一条数据
    for (let i = 0; i < this.testPayloads.length; i++) {
      const original = this.testPayloads[i];

      this.logInfo(`[${i + 1}] Testing payload: "${this.truncate(original)}"`);

      try {
        // 1. 加密
        const encrypted = await AesCryptoUtils.encrypt(original);

        if (!encrypted || encrypted === original) {
          // 加密失败或未加密
          this.logInfo(`   ❌ Encryption Failed or returned raw text.`);
          failCount++;
          continue;
        }

        // 2. 解密
        const decrypted = await AesCryptoUtils.decrypt(encrypted);

        // 3. 比对
        if (decrypted === original) {
          // this.logInfo(`   ✅ Verification Passed.`); // 为了控制日志长度，成功时不打印太详细
          successCount++;
        } else {
          this.logInfo(`   ❌ Verification Failed!`);
          this.logInfo(`      Original:  ${original}`);
          this.logInfo(`      Decrypted: ${decrypted}`);
          failCount++;
        }

      } catch (err) {
        this.logInfo(`   ❌ Exception: ${JSON.stringify(err)}`);
        failCount++;
      }
    }

    const endTime = Date.now();
    const duration = endTime - startTime;

    this.logInfo('----------------------------------------');
    this.logInfo('🔐 Encryption Stress Report:');
    this.logInfo(`   Total Time: ${duration} ms`);
    this.logInfo(`   Average Time: ${(duration / this.testPayloads.length).toFixed(2)} ms/op`);
    this.logInfo(`   Success: ${successCount}`);
    this.logInfo(`   Failed:  ${failCount}`);
    this.logInfo('----------------------------------------');

    if (failCount === 0) {
      this.logInfo('✅ All security tests PASSED.');
    } else {
      this.logInfo('⚠️ Some security tests FAILED.');
    }

    this.logInfo('=== SecurityStressTestCase END ===');
  }

  // 辅助方法：截断过长的日志
  private truncate(str: string, maxLength: number = 20): string {
    if (str.length > maxLength) {
      return str.substring(0, maxLength) + '...';
    }
    return str;
  }
}