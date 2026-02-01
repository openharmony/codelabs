/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-20
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
@Component
export struct SportControlButton {
  @Prop isRunning: boolean;
  @Prop isPaused: boolean;
  onStart: () => void = () => {};
  onPause: () => void = () => {};
  onResume: () => void = () => {};
  onStop: () => void = () => {};

  build() {
    Row() {
      if (!this.isRunning) {
        this.Button('开始运动', '#4ECDC4', this.onStart)
      } else if (this.isPaused) {
        this.Button('继续运动', '#7AC36C', this.onResume)
        this.Button('结束运动', '#FF6B6B', this.onStop)
      } else {
        this.Button('暂停运动', '#F5BE14', this.onPause)
        this.Button('结束运动', '#FF6B6B', this.onStop)
      }
    }
    .width('100%')
    .justifyContent(FlexAlign.SpaceEvenly)
  }

  @Builder
  Button(text: string, color: string, onClick: () => void) {
    Text(text)
      .fontSize(16)
      .fontWeight(FontWeight.Bold)
      .fontColor('#FFFFFF')
      .width(120)
      .height(48)
      .textAlign(TextAlign.Center)
      .backgroundColor(color)
      .borderRadius(24)
      .onClick(onClick)
  }
}
