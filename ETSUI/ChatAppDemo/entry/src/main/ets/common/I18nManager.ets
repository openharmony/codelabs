/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import i18n from '@ohos.i18n';
import { BusinessError } from '@ohos.base';

export class I18nManager {
  /**
   * 切换语言逻辑
   * @param languageCode 'zh-Hans' 或 'en-US'
   */
  static switchLanguage(languageCode: string) {
    try {
      // 设置系统语言（这会影响 $r 的指向）
      i18n.System.setAppPreferredLanguage(languageCode);

      // 将当前语言持久化，以便下次启动时读取
      AppStorage.setOrCreate('currentLanguage', languageCode);

      console.info(`成功切换语言至: ${languageCode}`);
    } catch (error) {
      let err: BusinessError = error as BusinessError;
      console.error(`切换语言失败: ${err.code} ${err.message}`);
    }
  }

  /**
   * 初始化语言（在 EntryAbility 的 onCreate 中调用）
   */
  static initLanguage() {
    let lang = AppStorage.get<string>('currentLanguage') || 'zh-Hans';
    i18n.System.setAppPreferredLanguage(lang);
  }
}