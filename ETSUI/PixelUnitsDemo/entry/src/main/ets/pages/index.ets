/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
import router from '@ohos.router'
@Entry
@Component
struct Index {
  @State message: string = 'Hello World！'
  TAG= 'PixelUnitsDemo'

  build() {

    Row() {
      Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceEvenly }) {
        TextButton({ content: '像素介绍' ,onclick: () => {
          router.push({url:  "pages/pixelUnitsPage",})
        }})
        TextButton({ content: '像素转换', onclick: () => {
          router.push({url: "pages/pixelConversionPage"})
        } })
      }
      .width('100%')
    }
    .height('100%')
  }
}
@Component
struct TextButton {
  @Prop content: string
  onclick: (onclick?: ClickEvent) => void

  build() {
    Button(this.content, { type: ButtonType.Normal, stateEffect: false })
      .borderRadius(9).fontSize(40).padding(50).onClick(this.onclick)
  }
}