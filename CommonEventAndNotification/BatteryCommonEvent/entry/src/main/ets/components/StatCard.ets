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
export struct StatCard {
  @Prop title: string = '';
  @Prop value: string = '--';
  subtitle: string = '';
  onTap?: () => void;

  build() {
    Column() {
      Text(this.title)
        .fontSize(12)
        .opacity(0.75);

      Text(this.value)
        .fontSize(22)
        .fontWeight(FontWeight.Medium)
        .margin({ top: 4 });

      if (this.subtitle) {
        Text(this.subtitle)
          .fontSize(11)
          .opacity(0.65)
          .margin({ top: 2 });
      }
    }
    .padding(12)
    .backgroundColor(0x0D000000)
    .borderRadius(12)
    .onClick(() => {
      if (this.onTap) {this.onTap()};
    });
  }
}
