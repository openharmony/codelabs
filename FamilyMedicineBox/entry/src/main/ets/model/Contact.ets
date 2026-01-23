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

  email: string = '';     // 瀵瑰簲浣嶇疆
  image: string = '';     // 瀵瑰簲鍥剧墖
  userId: number = 0;     // 瀵瑰簲鐢ㄦ埛ID

  // 鏋勯€犲嚱鏁版敮鎸佷袱涓弬鏁帮紝閬垮厤 Expected 1 arguments but got 2 鎶ラ敊
  constructor(name: string, phone: string) {
    this.name = name;
    this.phone = phone;
  }
}


