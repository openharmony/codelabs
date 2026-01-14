/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
export class JsonUtil {
  /**
   * ArkTS strict mode dislikes "any/unknown" and indexed signatures.
   * Keep JSON helpers simple: stringify to string and parse to Object.
   */
  static safeStringify(obj: object | object[] | string | number | boolean | null, space: number = 2): string {
    try {
      return JSON.stringify(obj, null, space);
    } catch (_) {
      return '{}';
    }
  }

  static safeParseObject(text: string): Object | null {
    try {
      const v = JSON.parse(text) as Object;
      return v;
    } catch (_) {
      return null;
    }
  }
}
