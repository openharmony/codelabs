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
import promptAction from '@ohos.promptAction'
import Context from '@ohos.app.ability.common'
import { RdbUtil } from '../utils/RdbUtil'

@Entry
@Component
struct Login {
  @State private tip: string = ''
  @State private name: string = ''

  private readonly defaultAvatarKey: string = 'user'

  aboutToAppear(): void {
    const params = router.getParams() as Record<string, Object> | undefined
    const t = params && params['tip'] ? `${params['tip']}` : ''
    this.tip = t
  }

  private avatarRes(): Resource {
    return $r('app.media.user')
  }

  private async login(): Promise<void> {
    const n = this.name.trim()
    if (n.length <= 0) {
      promptAction.showToast({ message: '请输入昵称' })
      return
    }
    const ok = await RdbUtil.setCurrentUser(getContext(this) as Context, n, this.defaultAvatarKey)
    if (ok) {
      promptAction.showToast({ message: '登录成功' })
      router.back()
      return
    }
    promptAction.showToast({ message: '登录失败，请重试' })
  }

  build() {
    Column() {
      Row() {
        Text('返回')
          .fontSize(14)
          .fontColor('#666')
          .onClick(() => router.back())
        Blank().layoutWeight(1)
        Text('登录')
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .fontColor('#111')
        Blank().layoutWeight(1)
        Text(' ')
          .fontSize(14)
      }
      .padding({ left: 16, right: 16, top: 14, bottom: 10 })

      Column({ space: 12 }) {
        if (this.tip.length > 0) {
          Text(this.tip)
            .fontSize(13)
            .fontColor('#D47D00')
            .padding(12)
            .borderRadius(12)
            .backgroundColor('#FFF6EE')
        }

        TextInput({ placeholder: '昵称', text: this.name })
          .onChange((v: string) => {
            this.name = v
          })
          .height(46)
          .padding({ left: 12, right: 12 })
          .borderRadius(12)
          .backgroundColor('#FFFFFF')

        Row({ space: 10 }) {
          Image(this.avatarRes())
            .width(54)
            .height(54)
            .borderRadius(16)
            .objectFit(ImageFit.Cover)
        }

        Button('登录并返回')
          .type(ButtonType.Capsule)
          .backgroundColor('#111')
          .fontColor('#FFF')
          .onClick(() => {
            void this.login()
          })
          .margin({ top: 6 })
      }
      .padding(16)
      .borderRadius(18)
      .backgroundColor('#FFFFFF')
      .margin({ left: 16, right: 16, top: 10 })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F6F4F1')
  }
}