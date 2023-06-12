/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import util from '@ohos.util';

class DataTransformUtil {
  private base64Helper: util.Base64Helper = null;

  constructor() {
    this.base64Helper = new util.Base64Helper();
  }

  /**
   * Base64 to uint8Array.
   *
   * @param content Base64 data.
   * @returns Uint8Array data.
   */
  base64ToUint8Array(content: string): Uint8Array {
    let uint8ArrayData: Uint8Array = this.base64Helper.decodeSync(content);
    return uint8ArrayData;
  }

  /**
   * Convert uint8Array to base64.
   *
   * @param content Uint8Array data.
   * @returns Base64 data.
   */
  uint8ArrayToBase64(content: Uint8Array): string {
    let base64Data: string = this.base64Helper.encodeToStringSync(content);
    return base64Data;
  }

  /**
   * Convert a character string to a uint8Array byte stream.
   *
   * @param content to be converted.
   * @returns Uint8Array data.
   */
  stringToUint8Array(content: string): Uint8Array {
    let arr = [];
    for (let i = 0, j = content.length; i < j; ++i) {
      arr.push(content.charCodeAt(i));
    }
    return new Uint8Array(arr);
  }

  /**
   * Converting uint8Array bytes to stream strings.
   *
   * @param content Uint8Array data.
   * @returns Converted string.
   */
  uint8ArrayToString(content: Uint8Array): string {
    let arrayString = '';
    for (let i = 0; i < content.length; i++) {
      arrayString += String.fromCharCode(content[i]);
    }
    return arrayString;
  }
}

export default new DataTransformUtil();