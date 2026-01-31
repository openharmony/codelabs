/*
 * Copyright (c) 2025 YourCompany. All rights reserved.
 * 文件名: example.cpp
 * 作者:邢欣
 * 版本: 1.0
 * 日期: 2025-11-01
 */
import hilog from '@ohos.hilog';

export class Logger {
  /**
   * 指示日志颜色
   */
  private domain: number;

  /**
   *  日志前缀
   */
  private prefix: string;

  /**
   * 日志格式字符串
   */
  private format: string = '%{public}s, %{public}s';

  /**
   * Constructor.
   *
   * @param prefix Identifies the log tag.
   * @param domain Indicates the service domain, which is a hexadecimal integer ranging from 0x0 to 0xFFFFF.
   */
  constructor(prefix: string = 'DistributeDraw', domain: number = 0xFF00) {
    this.prefix = prefix;
    this.domain = domain;
  }

  debug(...args: string[]): void {
    hilog.debug(this.domain, this.prefix, this.format, args);
  }

  info(...args: string[]): void {
    hilog.info(this.domain, this.prefix, this.format, args);
  }

  warn(...args: string[]): void {
    hilog.warn(this.domain, this.prefix, this.format, args);
  }

  error(...args: string[]): void {
    hilog.error(this.domain, this.prefix, this.format, args);
  }
}

export default new Logger();