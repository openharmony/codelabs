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
import { ExampleDebugCase } from './ExampleDebugCase'; // 示范用例
import { ContactDatabaseCase } from './ContactDatabaseCase'; // 业务用例：联系人数据库
import { EncryptionPrivacyCase } from './EncryptionPrivacyCase';
import { BiometricSettingCase } from './BiometricSettingCase';
import { SearchDebugCase } from './SearchDebugCase';
import { Logger } from '../common/utils/Logger';
import { BackupRestoreDebugCase } from './BackupRestoreDebugCase';
import { ContactStressTestCase } from './ContactStressTestCase';
import { EventAlgoTestCase } from './EventAlgoTestCase';
import { SecurityStressTestCase } from './SecurityStressTestCase';
import { UserScenarioSimulationCase } from './UserScenarioSimulationCase';
import { DateAndConfigStressTestCase } from './DateAndConfigStressTestCase';
enum DebugTarget {
  NONE = 'NONE',
  EXAMPLE = 'EXAMPLE', // 示范用例
  CONTACT_DATABASE = 'CONTACT_DATABASE', // 联系人数据库测试
  ENCRYPTION_PRIVACY = 'ENCRYPTION_PRIVACY',
  BIOMETRIC_SETTING = 'BIOMETRIC_SETTING',
  SEARCH = 'SEARCH',
  BACKUP_RESTORE = 'BACKUP_RESTORE',
  CONTACT_STRESS_TEST = 'CONTACT_STRESS_TEST',
  EVENT_ALGO_TEST = 'EVENT_ALGO_TEST',
  SECURITY_STRESS_TEST = 'SECURITY_STRESS_TEST',
  USER_SCENARIO_SIMULATION = 'USER_SCENARIO_SIMULATION',
  DATE_AND_CONFIG_STRESS_TEST = 'DATE_AND_CONFIG_STRESS_TEST',
  // 在这里添加新的枚举值...
}

/**
 * 当前要跑哪个调试用例
 * - 开发/调试时：改成你想跑的那个
 * - 发布正式包：改成 DebugTarget.NONE（或在 EntryAbility 里关掉调试入口）
 */
const CURRENT_DEBUG_TARGET: DebugTarget = DebugTarget.NONE;

// 你想测别的就改成 DebugTarget.CONTACT_DATABASE 等

function createCase(target: DebugTarget): DebugCase | null {
  switch (target) {
    case DebugTarget.EXAMPLE:
      return new ExampleDebugCase();
    case DebugTarget.CONTACT_DATABASE:
      return new ContactDatabaseCase();
    case DebugTarget.ENCRYPTION_PRIVACY:
      return new EncryptionPrivacyCase();
    case DebugTarget.BIOMETRIC_SETTING:
      return new BiometricSettingCase();
    case DebugTarget.SEARCH:
      return new SearchDebugCase();
    case DebugTarget.BACKUP_RESTORE:
      return new BackupRestoreDebugCase();
    case DebugTarget.CONTACT_STRESS_TEST:
      return new ContactStressTestCase();
    case DebugTarget.EVENT_ALGO_TEST:
      return new EventAlgoTestCase();
    case DebugTarget.SECURITY_STRESS_TEST:
      return new SecurityStressTestCase();
    case DebugTarget.USER_SCENARIO_SIMULATION:
      return new UserScenarioSimulationCase();
    case DebugTarget.DATE_AND_CONFIG_STRESS_TEST:
      return new DateAndConfigStressTestCase();
    case DebugTarget.NONE:
    default:
      return null;
  }
}

const logger = new Logger('DebugRunner');

export async function runCurrentDebugCase(): Promise<void> {
  const testCase = createCase(CURRENT_DEBUG_TARGET);
  if (!testCase) {
    // 如果是 NONE，可以选择不打印日志，或者打印一条 info
    // logger.info('No debug case selected.');
    return;
  }

  // 这里也可以顺便打一行日志，表明跑的是哪个 case
  logger.info(`[DebugRunner] Running debug case: ${testCase.name}`);

  try {
    await testCase.run();
  } catch (error) {
    logger.error(`[DebugRunner] Error running case ${testCase.name}:`, error);
  }
}