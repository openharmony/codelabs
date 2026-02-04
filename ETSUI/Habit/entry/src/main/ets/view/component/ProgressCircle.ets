/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
@Component
export struct ProgressCircle {
  @Prop percent: number; // 0 - 100
  color: string = '#007DFF';

  build() {
    Stack() {
      // 1. 底层光晕 (装饰)
      Circle({ width: 140, height: 140 })
        .fill(this.color)
        .opacity(0.1) // 淡淡的背景色
        .scale({ x: this.percent > 0 ? 1.1 : 1, y: this.percent > 0 ? 1.1 : 1 })
        .animation({ duration: 800, curve: Curve.EaseOut })

      // 2. 进度条背景环 (灰色)
      Circle({ width: 140, height: 140 })
        .strokeWidth(12)
        .stroke('#F0F0F0')
        .fill('none') // 空心
        .strokeLineCap(LineCapStyle.Round)

      // 3. 进度条 (彩色)
      // 使用 Path 或者 Progress 组件
      Progress({ value: this.percent, total: 100, type: ProgressType.Ring })
        .width(140)
        .color(this.color)
        .style({ strokeWidth: 12, enableSmoothEffect: true }) // 开启平滑动画

      // 4. 中间文字
      Column() {
        Text('今日达成')
          .fontSize(12)
          .fontColor('#999')
          .margin({ bottom: 4 })

        Text(Math.round(this.percent).toString() + '%')
          .fontSize(32)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333')
      }
    }
  }
}