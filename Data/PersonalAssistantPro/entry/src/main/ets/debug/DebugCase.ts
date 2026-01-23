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
// File: entry/src/main/ets/debug/DebugCase.ts
import { Logger } from '../common/utils/Logger';

export abstract class DebugCase {
  // 每个调试用例必须定义自己的名字
  abstract readonly name: string;

  // 核心运行逻辑
  abstract run(): Promise<void>;

  // 实例化 Logger，模块名统一为 "DebugCase"
  private logger = new Logger('DebugCase');

  protected logDebug(...args: unknown[]): void {
    this.logger.debug(`[${this.name}]`, ...args);
  }

  protected logInfo(...args: unknown[]): void {
    this.logger.info(`[${this.name}]`, ...args);
  }

  protected logWarn(...args: unknown[]): void {
    this.logger.warn(`[${this.name}]`, ...args);
  }

  protected logError(...args: unknown[]): void {
    this.logger.error(`[${this.name}]`, ...args);
  }

  protected logFatal(...args: unknown[]): void {
    this.logger.fatal(`[${this.name}]`, ...args);
  }
}