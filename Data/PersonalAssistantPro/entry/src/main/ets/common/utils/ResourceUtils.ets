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
// entry/src/main/ets/common/utils/ResourceUtils.ets
import { common } from '@kit.AbilityKit';
import { Logger } from './Logger';
import { BusinessError } from '@kit.BasicServicesKit';

/**
 * 资源工具类
 * 简化 Context.resourceManager 的调用，提供统一的异常处理
 */
export class ResourceUtils {
  private static logger = new Logger('ResourceUtils');
  private static context: common.UIAbilityContext;

  /**
   * 初始化工具类
   */
  public static init(context: common.UIAbilityContext) {
    //  Fix: this.context -> ResourceUtils.context
    ResourceUtils.context = context;
    ResourceUtils.logger.info('ResourceUtils initialized');
  }

  /**
   * 获取字符串资源
   * @param resId 资源 ID ($r('app.string.xxx'))
   * @returns Promise<string>
   */
  public static async getString(resId: Resource): Promise<string> {
    //  Fix: this.context -> ResourceUtils.context
    if (!ResourceUtils.context) {
      ResourceUtils.logger.error('Context not initialized');
      return '';
    }
    try {
      const manager = ResourceUtils.context.resourceManager;
      const value = await manager.getStringValue(resId);
      return value;
    } catch (e) {
      const err = e as BusinessError;
      ResourceUtils.logger.error(`Get string failed: ${err.message}`);
      return '';
    }
  }

  /**
   * 获取颜色资源 (返回 number)
   * @param resId
   */
  public static async getColor(resId: Resource): Promise<number> {
    if (!ResourceUtils.context) return 0;
    try {
      const manager = ResourceUtils.context.resourceManager;
      return await manager.getColor(resId);
    } catch (e) {
      ResourceUtils.logger.error('Get color failed');
      return 0;
    }
  }

  /**
   * 获取 RawFile 原始内容 (Uint8Array)
   * @param fileName rawfile 下的文件名
   */
  public static async getRawFileContent(fileName: string): Promise<Uint8Array> {
    if (!ResourceUtils.context) {
      return new Uint8Array();
    }
    try {
      const content = await ResourceUtils.context.resourceManager.getRawFileContent(fileName);
      ResourceUtils.logger.info(`Read raw file success: ${fileName}, size: ${content.length}`);
      return content;
    } catch (e) {
      const err = e as BusinessError;
      ResourceUtils.logger.error(`Read raw file failed: ${fileName}, error: ${err.message}`);
      return new Uint8Array();
    }
  }

  /**
   * 获取 RawFile 内容并转为字符串
   */
  public static async getRawFileString(fileName: string): Promise<string> {
    try {
      //  Fix: this.getRawFileContent -> ResourceUtils.getRawFileContent
      const data = await ResourceUtils.getRawFileContent(fileName);
      if (data.length === 0) return '';

      // 暂未实现 TextDecoder，返回空
      ResourceUtils.logger.warn('TextDecoder not implemented in this util yet.');
      return '';
    } catch (e) {
      return '';
    }
  }
}