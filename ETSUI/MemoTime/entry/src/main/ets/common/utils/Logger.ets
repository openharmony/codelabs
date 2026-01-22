/**
 * 日志工具类
 */
import { hilog } from '@kit.PerformanceAnalysisKit'

const DOMAIN: number = 0x0000
const TAG: string = 'MemoTime'

/**
 * 日志级别
 */
export enum LogLevel {
  DEBUG = 0,
  INFO = 1,
  WARN = 2,
  ERROR = 3
}

/**
 * 日志工具类
 */
export class Logger {
  private static currentLevel: LogLevel = LogLevel.DEBUG

  /**
   * 设置日志级别
   */
  static setLevel(level: LogLevel): void {
    Logger.currentLevel = level
  }

  /**
   * Debug 日志
   */
  static debug(tag: string, message: string): void {
    if (Logger.currentLevel <= LogLevel.DEBUG) {
      hilog.debug(DOMAIN, `${TAG}:${tag}`, message)
    }
  }

  /**
   * Info 日志
   */
  static info(tag: string, message: string): void {
    if (Logger.currentLevel <= LogLevel.INFO) {
      hilog.info(DOMAIN, `${TAG}:${tag}`, message)
    }
  }

  /**
   * Warn 日志
   */
  static warn(tag: string, message: string): void {
    if (Logger.currentLevel <= LogLevel.WARN) {
      hilog.warn(DOMAIN, `${TAG}:${tag}`, message)
    }
  }

  /**
   * Error 日志
   */
  static error(tag: string, message: string): void {
    if (Logger.currentLevel <= LogLevel.ERROR) {
      hilog.error(DOMAIN, `${TAG}:${tag}`, message)
    }
  }
}

export default Logger
