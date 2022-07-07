/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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
export default struct PressureEffectButton {
  @State isEffected: boolean = false
  @State isPressed: boolean = false
  private width: string = '100px'
  private height: string = '100px'
  private bgColor: string = '#00000000'
  private bgRadius: number = 0
  private effectWidth: string
  private effectHeight: string
  private commonSrc: string | PixelMap | Resource
  private effectSrc: string | PixelMap | Resource
  private effectCallback: (isEffected: boolean) => void

  aboutToAppear() {
    if (!this.effectWidth) {
      this.effectWidth = this.width
    }
    if (!this.effectHeight) {
      this.effectHeight = this.height
    }
  }

  build() {
    Stack({ alignContent: Alignment.Center }) {
      Image((this.isEffected && this.effectSrc != null) ? this.effectSrc : this.commonSrc)
        .objectFit(ImageFit.Fill)
        .width(this.effectWidth)
        .height(this.effectHeight)
        .borderRadius((this.effectHeight == this.height) ? this.bgRadius : 0)
    }
    .backgroundColor(this.bgColor)
    .borderRadius(this.bgRadius)
    .height(this.height)
    .width(this.width)
    .scale({ x: this.isPressed ? 0.8 : 1.0, y: this.isPressed ? 0.8 : 1.0 })
    .opacity(this.isPressed ? 0.6 : 1.0)
    .animation({
      duration: this.isPressed || !this.effectSrc ? 300 : 0,
      curve: Curve.Linear
    })
    .onTouch((event: TouchEvent) => {
      if (event.type === TouchType.Down) {
        this.isPressed = true
      } else if (event.type === TouchType.Up) {
        this.isPressed = false
        this.isEffected = !this.isEffected
        this.effectCallback(this.isEffected)
      }
    })
  }
}