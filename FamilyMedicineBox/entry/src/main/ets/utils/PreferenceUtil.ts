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
// 下面是你的业务代码（比如 import 语句、类定义等）



} catch (err) {
      // 【修复】严格模式下，将错误转换为字符串打印
      console.error('Preferences load failed', JSON.stringify(err));
    }
  }

  async putString(key: string, value: string) {
    if (!this.pref) return;
    try {
      await this.pref.put(key, value);
      await this.pref.flush();
    } catch (err) {
      console.error('Preferences put failed', JSON.stringify(err));
    }
  }

  async getString(key: string, defValue: string): Promise<string> {
    if (!this.pref) return defValue;
    try {
      return (await this.pref.get(key, defValue)) as string;
    } catch (err) {
      console.error('Preferences get failed', JSON.stringify(err));
      return defValue;
    }
  }
}

export default new PreferenceUtil();
