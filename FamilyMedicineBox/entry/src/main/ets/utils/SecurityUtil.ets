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

        arr[i] = text.charCodeAt(i);
      }
      return that.encodeToStringSync(arr);
    } catch (e) {
      console.error('[SecurityUtil] 鍔犲瘑澶辫触', e);
      return text;
    }
  }

  // 瑙ｅ瘑鏂规硶
  static decrypt(text: string): string {
    if (!text) return '';
    try {
      let that = new util.Base64Helper();
      let arr = that.decodeSync(text);
      let str = '';
      for (let i = 0; i < arr.length; i++) {
        str += String.fromCharCode(arr[i]);
      }
      return str;
    } catch (e) {
      // 濡傛灉瑙ｅ瘑澶辫触锛堝彲鑳芥槸鑰佹暟鎹級锛岃繑鍥炲師鏂?      return text;
    }
  }
}


