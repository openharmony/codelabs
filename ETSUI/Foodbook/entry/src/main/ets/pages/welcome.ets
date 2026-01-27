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

import { router } from '@kit.ArkUI'

@Entry
@Component
struct welcome {

  build() {
    Stack() {
      // 背景图片
      Image($r('app.media.contextyg')).width('100%').height('150%')


      Column({ space: 5 }) {
        // 调整图标容器的高度为更小的值，让图标更靠上
        Stack()
          .width('100%')
          .height('10%')  // 这里减少了高度，将图标向上推

        // 图标并设置圆角
        Image($r('app.media.app'))
          .width(130)
          .height(130)
          .alignSelf(ItemAlign.Center)
          .borderRadius(30)  // 设置为圆角，值为宽度的一半实现圆形

        // 调整文字间距
        Column({ space: 5 }) {  // 增加 space 值来增加文字间的距离
          // 显式声明 char 为 string 类型
          ForEach('那些和吃有关的小事'.split(''), (char: string) => {
            Text(char)
              .fontSize(30)
              .fontColor(Color.Red)
              .textAlign(TextAlign.Center)
          })
        }
        .alignSelf(ItemAlign.Center)
      }
    }
  }

  onPageShow(): void {
    setTimeout(() => {
      router.replaceUrl({
        url: 'pages/login'
      })
    }, 2000)
  }

}
