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
 *
 */

/**
 * 浮动操作按钮组件
 */
@Component
export struct FloatingButton {
  @Prop iconRes: Resource = $r('sys.media.ohos_ic_public_add')
  @Prop bgColor: string = '#4ECDC4'
  @Prop btnSize: number = 56
  onBtnClick: () => void = () => {
  }
  @State isPressed: boolean = false

  build() {
    Column() {
      Image(this.iconRes)
        .width(24)
        .height(24)
        .fillColor('#FFFFFF')
    }
    .width(this.btnSize)
    .height(this.btnSize)
    .borderRadius(this.btnSize / 2)
    .backgroundColor(this.bgColor)
    .justifyContent(FlexAlign.Center)
    .alignItems(HorizontalAlign.Center)
    .shadow({
      radius: 12,
      color: 'rgba(78, 205, 196, 0.4)',
      offsetX: 0,
      offsetY: 4
    })
    .scale({ x: this.isPressed ? 0.9 : 1, y: this.isPressed ? 0.9 : 1 })
    .animation({
      duration: 150,
      curve: Curve.EaseOut
    })
    .onTouch((event) => {
      if (event.type === TouchType.Down) {
        this.isPressed = true
      } else if (event.type === TouchType.Up || event.type === TouchType.Cancel) {
        this.isPressed = false
      }
    })
    .onClick(() => {
      this.onBtnClick()
    })
  }
}
