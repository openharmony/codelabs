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

import router from '@ohos.router'
import { UserModel } from '../viewmodel/UserModel'

// 功能菜单项接口
interface MenuItem {
  index?: number
  icon: Resource
  title: ResourceStr
  badge?: number
}

@Component
export struct Profile {
  // @StorageProp 是单向绑定（只读），@StorageLink 是双向绑定
  @StorageProp('currentUser') user: UserModel | null = null;
  // 功能菜单数据
  private menuItems: MenuItem[][] = [
    [
      // { icon: $r('app.media.user7'), title: $r('app.string.label_about_us') },
      { index: 0, icon: $r('app.media.edit'), title: $r('app.string.btn_edit') },
      { index: 1, icon: $r('app.media.settings'), title: $r('app.string.label_settings') }
    ]
  ]

  // 退出登录函数
  private handleLogout(): void {
    // 这里可以添加退出登录的逻辑
    // 然后跳转到登录页面
    router.replaceUrl({
      url: 'pages/Login'
    })
  }

  build(): void {
    Column() {
      // 顶部个人信息区域
      Column() {
        if (this.user) {
          // 头像
          Image(this.user.avatarPath)
            .width(80)
            .height(80)
            .borderRadius(40)
            .margin({ bottom: 12 })

          // 昵称
          Text(this.user.nickName)
            .fontSize(18)
            .fontWeight(700)
            .margin({ bottom: 8 })

          // 地区
          Text(this.user.country)
            .fontSize(12)
            .fontColor('#666666')
        }
      }
      .width('100%')
      .height(160)
      .backgroundColor('#FFFFFF')
      .borderRadius(12)
      .margin({ top: 16 })
      .padding(24)
      .alignItems(HorizontalAlign.Center)
      .justifyContent(FlexAlign.Center)

      // 功能菜单区域
      Column() {
        ForEach(this.menuItems, (row: MenuItem[]) => {
          Row() {
            ForEach(row, (item: MenuItem) => {
              Column() {
                Image(item.icon)
                  .width(36)
                  .height(36)

                Row() {
                  Text(item.title)
                    .fontSize(14)
                    .fontColor('#000000')

                  if (item.badge) {
                    Text(item.badge.toString())
                      .fontSize(10)
                      .fontColor('#FFFFFF')
                      .backgroundColor('#FF4D4F')
                      .padding({
                        left: 6,
                        right: 6,
                        top: 2,
                        bottom: 2
                      })
                      .borderRadius(10)
                      .margin({ left: 4 })
                  }
                }
              }
              .width('25%')
              .height(100)
              .alignItems(HorizontalAlign.Center)
              .justifyContent(FlexAlign.Center)
              .padding(12)
              .onClick(() => {
                if (item.index === 0) {
                  router.pushUrl({
                    url: 'pages/EditProfile'
                  })
                } else if (item.index === 1) {
                  router.pushUrl({
                    url: 'pages/Settings'
                  })
                }


                // else if (item.title === $r('app.string.label_about_us')) {
                //   router.pushUrl({
                //     url: 'pages/AboutUs'
                //   })
                // }
              })
            })
          }
          .width('100%')
          .height(100)
          .backgroundColor('#FFFFFF')
          .borderRadius(12)
          .margin({ bottom: 12 })
        })
      }
      .width('100%')

      // 退出登录按钮
      Button() {
        Text($r('app.string.btn_logout'))
          .fontSize(16)
          .fontColor('#FFFFFF')
      }
      .width('100%')
      .height(48)
      .backgroundColor('#FF4D4F')
      .margin({ top: 'auto', bottom: 24 })
      .borderRadius(24)
      .onClick(() => {
        this.handleLogout()
      })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5')
    .padding(16)
  }
}