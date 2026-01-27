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

import { Prompt, router } from '@kit.ArkUI'
import { AppStorageV2 } from '@kit.ArkUI'  // 引入 AppStorage

@Entry
@Component
struct edit {
  @State signature: string = AppStorage.get<string>('userSignature') || ''  // 从存储读取签名，默认空字符串

  build() {
    Column() {
      Row() {
        Button()
          .height(30).width(30).margin({left:10})
          .backgroundImage($r('app.media.return1'))
          .backgroundColor(Color.White).backgroundImageSize(ImageSize.FILL)
          .onClick(() => {
            this.saveSignature() // 返回前保存签名
            router.back()
          })

        Text('编辑个人信息')
          .fontSize(18)
          .textAlign(TextAlign.Center) // 文本水平居中
          .width('80%') // 必须设置宽度才能生效
      }.height(40).width('100%').margin({top:5})

      Button()
        .width(150).height(150).borderRadius(90)
        .backgroundColor(Color.Gray).margin({top:20})
        .onClick(() => {
          AlertDialog.show({
            title: '未授权',
            message: '需允许摄像头权限和访问相册权限才能发布图片，请开启后重试',
            autoCancel: false,
            alignment: DialogAlignment.Center,
            offset: { dx: 0, dy: 0 },
            gridCount: 5,
            primaryButton: {
              value: '去开启',
              fontColor: Color.Blue,
              action: () => {
                console.info('Button-clicking callback')
              },
            },
            secondaryButton: {
              value: '取消',
              fontColor: Color.Blue,
              action: () => {
                console.info('Button-cancel callback')
              },
            },
          })
        })
        .backgroundImage($r('app.media.cat')).backgroundImageSize(ImageSize.FILL)

      Text('cc').fontWeight(FontWeight.Bold).height(50).fontSize(25)

      TextInput({ placeholder: '点击添加签名', text: this.signature })  // 绑定签名数据
        .backgroundColor(Color.White)
        .textAlign(TextAlign.Center)
        .width('40%')
        .onChange((value: string) => {
          this.signature = value  // 实时更新签名
        })

      Text('如果修改头像时遇到点对勾没有反应的情况\n可以尝试退出本页重试')
        .margin({top:140})
        .textAlign(TextAlign.Center)

    }.height('100%').width('100%')
  }

  // 保存签名的方法
  saveSignature() {
    AppStorage.setOrCreate('userSignature', this.signature)  // 存储签名
    console.log('签名已保存:', this.signature)
  }
}