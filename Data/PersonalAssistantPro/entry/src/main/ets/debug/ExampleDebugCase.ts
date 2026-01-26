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
/**
 * src/main/ets/debug/ExampleDebugCase.ts
 * ExampleDebugCase
 * ----------------
 * 这是一个“示范 / 模板”调试用例，本身不做任何真实业务，只输出几行日志。
 *
 * 👉 后续如果要添加新的调试 Case，一般步骤是：
 * 1. 新建一个文件，例如：
 * entry/src/main/ets/debug/MyNewFeatureCase.ts
 * 2. 让这个类继承 DebugCase，并实现：
 * - readonly name: string    用于标识这个调试用例
 * - async run(): Promise<void>   在这里写你的调试逻辑
 * 3. 在 DebugRunner.ts 中：
 * - 在 DebugTarget 枚举里加一个枚举值（如 MY_NEW_FEATURE）
 * - 在 createCase(...) 的 switch 里返回 new MyNewFeatureCase()
 * - 把 CURRENT_DEBUG_TARGET 改成你新加的那个枚举值（或者在 UI 里动态选择）
 *
 * 之后，EntryAbility 里只需要调用 runCurrentDebugCase()，不用再改。
 */

import { DebugCase } from './DebugCase';
// [适配说明] 这里暂时注释掉，等你创建了 local.secret.ts 后可以解开
// import { TEST_PASSWORD, TEST_USERNAME } from './local.secret';

export class ExampleDebugCase extends DebugCase {
  readonly name = 'Example / Template debug case';

  async run(): Promise<void> {
    // 这里只是演示：简单输出几行日志
    const now = new Date();

    this.logInfo('=== ExampleDebugCase START ===');
    this.logInfo('Case name:', this.name);
    this.logInfo('Current timestamp (ms):', now.getTime());
    this.logInfo('Current time (ISO):', now.toISOString());
    this.logInfo('Some debug context:', {
      hint: '这里可以放一些你想看到的环境信息、配置、状态等',
      env: 'dev',
      random: Math.random(),
    });
    this.logInfo('=== ExampleDebugCase END ===');
  }
}