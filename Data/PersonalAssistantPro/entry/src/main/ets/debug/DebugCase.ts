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