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
// File: entry/src/main/ets/common/utils/Logger.ts
import hilog from '@ohos.hilog';

// 定义日志模式类型
type LogFilterMode = 'BLOCK_LIST' | 'ALLOW_LIST';

export class Logger {
  // ===================== 核心配置区域 =====================

  private static readonly DOMAIN: number = 0x0101;
  private static readonly ROOT_TAG: string = 'PA_Pro'; // 项目 Tag

  /** 全局总开关：false 则彻底关闭所有日志 */
  private static IS_DEBUG: boolean = true;

  /**
   * 过滤模式：
   * - 'BLOCK_LIST' (黑名单): 默认打印所有，**屏蔽** LIST 中的模块
   * - 'ALLOW_LIST' (白名单): 默认不打印，**只看** LIST 中的模块
   *
   * 建议：初期开发使用 BLOCK_LIST 且列表为空（全看）；
   * 后期日志太多时，改为 ALLOW_LIST 只看正在调试的模块。
   */
  private static readonly FILTER_MODE: LogFilterMode = 'BLOCK_LIST';

  /**
   * 模块列表
   * 将你想 屏蔽(黑名单时) 或 关注(白名单时) 的模块名填入这里
   */
  private static readonly TARGET_MODULES: string[] = [
    'ContactModel'
    // 'DebugCase'
    // 例如：觉得数据库日志太烦，就把 'RdbHelper' 加进来(黑名单模式下)
    // 'RdbHelper',
  ];

  // =======================================================

  // 📝 项目模块注册表 (根据 PROJECT_CONTEXT.md 更新)
  public static readonly ALL_MODULES = {
    // [App & Ability]
    EntryAbility: 'EntryAbility',

    // [Database & Common]
    RdbHelper: 'RdbHelper',
    Tables: 'Tables',
    HttpUtils: 'HttpUtils',
    PermissionUtils: 'PermissionUtils',
    CryptoUtils: 'CryptoUtils',

    // [Model - 数据层]
    ContactModel: 'ContactModel',
    EventModel: 'EventModel',
    SettingModel: 'SettingModel',

    // [ViewModel - 业务逻辑层]
    ContactViewModel: 'ContactViewModel',
    EventViewModel: 'EventViewModel',
    CalendarViewModel: 'CalendarViewModel',

    // [Services - 后台服务]
    NotificationService: 'NotificationService',
    BiometricService: 'BiometricService',
    JumpService: 'JumpService',

    // [Pages - 页面]
    SplashPage: 'SplashPage',
    MainPage: 'MainPage',
    ContactListPage: 'ContactListPage',
    EventDetailPage: 'EventDetailPage',
    EventEditPage: 'EventEditPage',

    // [Components - 组件]
    CalendarView: 'CalendarView',
    CustomDialog: 'CustomDialog',
  };

  // ================= 逻辑实现区域 (无需修改) =================

  private moduleName: string;

  constructor(moduleName: string) {
    this.moduleName = moduleName;
  }

  private shouldLog(): boolean {
    // 1. 总开关
    if (!Logger.IS_DEBUG) return false;

    // 2. 判断是否在列表中
    const isInList = Logger.TARGET_MODULES.includes(this.moduleName);

    if (Logger.FILTER_MODE === 'BLOCK_LIST') {
      // 黑名单模式：在列表里 -> 不打印；不在列表里 -> 打印
      return !isInList;
    } else {
      // 白名单模式：在列表里 -> 打印；不在列表里 -> 不打印
      return isInList;
    }
  }

  debug(...args: any[]) {
    if (!this.shouldLog()) return;
    hilog.debug(Logger.DOMAIN, this.getFullTag(), this.formatLog(args));
  }

  info(...args: any[]) {
    if (!this.shouldLog()) return;
    hilog.info(Logger.DOMAIN, this.getFullTag(), this.formatLog(args));
  }

  warn(...args: any[]) {
    if (!this.shouldLog()) return;
    hilog.warn(Logger.DOMAIN, this.getFullTag(), this.formatLog(args));
  }

  error(...args: any[]) {
    // Error 级别建议始终打印，除非全局开关关闭
    if (!Logger.IS_DEBUG) return;
    hilog.error(Logger.DOMAIN, this.getFullTag(), this.formatLog(args));
  }

  fatal(...args: any[]) {
    if (!Logger.IS_DEBUG) return;
    hilog.fatal(Logger.DOMAIN, this.getFullTag(), this.formatLog(args));
  }

  private getFullTag(): string {
    // 限制 Tag 长度，防止被截断
    return `${Logger.ROOT_TAG}_${this.moduleName}`;
  }

  private formatLog(args: any[]): string {
    return args.map(arg => {
      if (arg === null) return 'null';
      if (arg === undefined) return 'undefined';
      if (arg instanceof Error) return `Error: ${arg.message}\nStack: ${arg.stack}`;
      if (typeof arg === 'object') {
        try {
          // 格式化 JSON，使其在控制台更易读
          return JSON.stringify(arg);
        } catch (e) {
          return String(arg);
        }
      }
      return String(arg);
    }).join(' ');
  }
}