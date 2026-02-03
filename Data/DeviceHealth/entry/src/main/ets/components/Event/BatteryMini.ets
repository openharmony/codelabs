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
export default struct BatteryMiniWidget {
  @Prop batteryLevel: number = 0;

  build() {
    Row() {
      Stack({ alignContent: Alignment.Start }) {
        Rect().width(35).height(16).fill(Color.Transparent).stroke('#CCCCCC').strokeWidth(1.5).radius(3)
        Rect()
          .width(this.batteryLevel > 0 ? (this.batteryLevel / 100) * 31 : 0)
          .height(12)
          .fill(this.batteryLevel <= 20 ? '#FF4500' : '#32CD32')
          .margin({ left: 2 })
          .radius(1)
          .animation({ duration: 500, curve: Curve.EaseInOut })
      }
      Text(`${this.batteryLevel}%`).fontSize(13).margin({ left: 6 }).fontColor('#999999')
    }
  }
}