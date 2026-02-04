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
// src/main/ets/services/JumpService.ets
import { Logger } from '../common/utils/Logger';
import { common, Want } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';

/**
 * 统一跳转服务 (Navigation Service)
 * 管理应用内的页面跳转、DeepLink 解析以及外部应用拉起
 * 单例模式
 */
export class JumpService {
  private static instance: JumpService;
  private logger = new Logger('JumpService');
  private context: common.UIAbilityContext | null = null;

  private constructor() {
  }

  public static getInstance(): JumpService {
    if (!JumpService.instance) {
      JumpService.instance = new JumpService();
    }
    return JumpService.instance;
  }

  /**
   * 初始化上下文
   * 必须在 EntryAbility 的 onCreate 或 onWindowStageCreate 中调用
   */
  public init(context: common.UIAbilityContext) {
    this.context = context;
    this.logger.info('JumpService initialized with context.');
  }

  /**
   * 跳转到 Web 浏览器
   * @param url 目标 URL
   */
  public jumpToWeb(url: string) {
    if (!url || !url.startsWith('http')) {
      this.logger.error('Invalid URL for web jump');
      return;
    }
    this.logger.info(`Attempting to open web: ${url}`);

    if (this.context) {
      const want: Want = {
        action: 'ohos.want.action.viewData',
        entities: ['entity.system.browsable'],
        uri: url
      };

      this.context.startAbility(want).then(() => {
        this.logger.info('Start web ability success');
      }).catch((err: BusinessError) => {
        this.logger.error(`Start web ability failed: ${err.message}`);
      });
    } else {
      this.logger.error('Context is null, cannot jump');
    }
  }

  /**
   * 跳转到系统设置页 (权限设置)
   */
  public jumpToSystemSettings() {
    this.logger.info('Jumping to system settings...');
    if (this.context) {
      const want: Want = {
        bundleName: 'com.huawei.hmos.settings',
        abilityName: 'com.huawei.hmos.settings.MainAbility'
      };
      // 注意：不同系统的设置页 AbilityName 可能不同，这里仅作示例
      this.context.startAbility(want).catch((err: BusinessError) => {
        this.logger.error(`Jump settings failed: ${err.message}`);
      });
    }
  }

  /**
   * 处理 DeepLink 协议
   * 格式: pa://jump?target=detail&id=123
   */
  public handleDeepLink(link: string): void {
    this.logger.info(`Handling DeepLink: ${link}`);

    if (!link || !link.startsWith('pa://')) {
      this.logger.warn('Invalid scheme, ignored.');
      return;
    }

    // 简单的参数解析模拟
    // 实际项目中可以引入 url 模块进行解析
    if (link.includes('target=detail')) {
      this.logger.info('DeepLink Strategy: Navigate to Detail Page');
      // 这里可以配合 AppStorage 或 EventBus 通知 UI 层跳转
    } else if (link.includes('target=mine')) {
      this.logger.info('DeepLink Strategy: Navigate to Mine Page');
    } else {
      this.logger.warn('Unknown DeepLink target');
    }
  }
}