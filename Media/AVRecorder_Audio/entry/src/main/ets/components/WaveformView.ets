/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
export struct WaveformView {
  @Prop samples: number[] = [];

  build() {
    Row({ space: 4 }) {
      ForEach(this.samples, (value: number, index?: number) => {
        Column() {
          Blank()
            .width(4)
            .height(6 + value * 36)
            .backgroundColor(index !== undefined && index % 2 === 0 ? '#1f6f6d' : '#f2c1a0')
            .borderRadius(3);
        }
      }, (value: number, index?: number) => `${index}`)
    }
    .padding({ left: 12, right: 12, top: 8, bottom: 8 })
    .backgroundColor('#f1ebe1')
    .borderRadius(16)
    .height(70)
    .alignItems(VerticalAlign.Bottom)
  }
}
