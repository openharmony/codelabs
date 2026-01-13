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

@ComponentV2
export struct ConfirmDialog {
  @Param title: string = 'Confirm';
  @Param message: string = '';
  @Event onConfirm?: () => void;
  @Event onCancel?: () => void;

  build() {
    Column() {
      Text(this.title)
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .margin({ bottom: 8 });
      Text(this.message)
        .fontSize(14)
        .fontColor('#666666')
        .margin({ bottom: 16 });
      Row({ space: 12 }) {
        Button('Cancel')
          .onClick(() => this.onCancel && this.onCancel());
        Button('Confirm')
          .onClick(() => this.onConfirm && this.onConfirm());
      }
      .justifyContent(FlexAlign.End)
      .width('100%');
    }
    .padding(20)
    .width('100%')
  }
}
