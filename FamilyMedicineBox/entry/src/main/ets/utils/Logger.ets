/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSEt-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

  private static readonly PREFIX: string = '[AppLog] ';

  
  static warn(message: string, ...args: any[]) {
    hilog.warn(this.DOMAIN, this.TAG, this.PREFIX + message, args);
  }

  
  static error(message: string, ...args: any[]) {
    hilog.error(this.DOMAIN, this.TAG, this.PREFIX + message, args);
  }

  
  static printJson(obj: any) {
    try {
      const jsonStr = JSON.stringify(obj, null, 2);
      hilog.info(this.DOMAIN, this.TAG, this.PREFIX + 'JSON Content:\n' + jsonStr);
    } catch (e) {
      this.error('JSON parse failed', e);
    }
  }

  
  static printDivider() {
    hilog.info(this.DOMAIN, this.TAG, '--------------------------------------------------');
  }

  
  static startMethod(methodName: string) {
    hilog.info(this.DOMAIN, this.TAG, `>>> Method [${methodName}] Start <<<`);
  }

  
  static endMethod(methodName: string) {
    hilog.info(this.DOMAIN, this.TAG, `<<< Method [${methodName}] End <<<`);
  }
}

export default new Logger();





