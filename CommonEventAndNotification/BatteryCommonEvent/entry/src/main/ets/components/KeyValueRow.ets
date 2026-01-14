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

@Component
export struct KeyValueRow {
  k: string = '';
  v: string = '';
  hint: string = '';

  build() {
    Row() {
      Column({ space: 2 }) {
        Text(this.k).fontSize(14).fontWeight(FontWeight.Medium);
        if (this.hint) {
          Text(this.hint).fontSize(12).opacity(0.65);
        }
      }.layoutWeight(1);

      Text(this.v).fontSize(14).fontWeight(FontWeight.Medium);
    }
    .padding({ top: 10, bottom: 10 })
    .border({ width: { bottom: 1 }, color: 0x14000000 });
  }
}
