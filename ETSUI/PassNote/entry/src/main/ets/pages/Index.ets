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


// pages/Index.ets
import router from '@ohos.router'
interface AuthMethod {
  icon: Resource
  title: string
  desc: string
}

@Entry
@Component
struct Index {
  @State message: string = '加密课表系统'
  @State showAuthDetail: boolean = false

  private authMethods: AuthMethod[] = [
    {
      icon: $r('app.media.ic_fingerprint'),
      title: '指纹识别',
      desc: '使用系统生物识别能力，快速完成身份验证'
    },
    {
      icon: $r('app.media.ic_number'),
      title: '数字密码',
      desc: '支持自定义数字 PIN 码进行安全访问'
    },
    {
      icon: $r('app.media.ic_pattern'),
      title: '图形密码',
      desc: '通过绘制图形完成身份校验'
    }
  ]

  build() {
    Column() {

      /* ================= 顶部导航标题 ================= */
      Row() {
        Column() {
          Text('加密课表系统')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)

          Text('安全访问您的课程表')
            .fontSize(12)
            .fontColor('#666')
        }
      }
      .padding(16)
      .backgroundColor('#FFFFFF')
      .border({ width: { bottom: 1 }, color: '#EEEEEE' })

      Scroll() {
        Column() {

          /* ================= 顶部欢迎区（Flex） ================= */
          Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center }) {
            Image($r('app.media.ic_shield'))
              .width(64)
              .height(64)
              .margin({ bottom: 16 })

            Text(this.message)
              .fontSize(26)
              .fontWeight(FontWeight.Bold)
              .fontColor('#1A1A1A')

            Text('多重认证保护您的隐私')
              .fontSize(14)
              .fontColor('#666')
              .margin({ top: 8 })
          }
          .padding(29)
          .backgroundColor('#F8FAFF')
          .borderRadius(16)
          .margin({ top: 20, bottom: 24 })

          /* ================= 功能卡片（Grid 风格） ================= */
          Column() {
            Text('支持的认证方式')
              .fontSize(18)
              .fontWeight(FontWeight.Medium)
              .margin({ top: 20,bottom: 16  })


            Row() {

              Column() {
                Image($r('app.media.ic_number'))
                  .width(32)
                  .height(32)
                  .margin({ bottom: 8 })

                Text('密码保护')
                  .fontSize(15)
                  .fontWeight(FontWeight.Medium)

                Text('数字 & 图形密码')
                  .fontSize(12)
                  .fontColor('#666')
              }
              .padding(16)
              .backgroundColor('#FFFFFF')
              .borderRadius(12)
              .shadow({ radius: 8, color: '#00000020' })
              .width('48%')




              Column() {
                Image($r('app.media.ic_up'))
                  .width(32)
                  .height(32)
                  .margin({ bottom: 8 })

                Text('生物识别')
                  .fontSize(15)
                  .fontWeight(FontWeight.Medium)

                Text('指纹解锁')
                  .fontSize(12)
                  .fontColor('#666')
              }
              .padding(16)
              .backgroundColor('#FFFFFF')
              .borderRadius(12)
              .shadow({ radius: 8, color: '#00000020' })
              .width('48%')
            }
          }
          .margin({ bottom: 28 })
        }
        .padding({ left: 16, right: 16 })
      }

      /* ================= 悬浮主操作按钮 ================= */
      Button('立即体验')
        .width(180)
        .height(48)
        .fontSize(16)
        .backgroundColor('#007AFF')
        .fontColor('#FFFFFF')
        .borderRadius(24)
        .margin({ top: 24, bottom: 20 })
        .onClick(() => {
          router.pushUrl({ url: 'pages/PasswordEntry' })
        })
        .shadow({ radius: 12, color: '#00000040' })

      /* ================= 底部说明 ================= */
        Column() {
          Text('OpenHarmony 身份认证示例')
            .fontSize(12)
            .fontColor('#999')

          Text('支持数字 / 图形密码 / 指纹密码')
            .fontSize(12)
            .fontColor('#999')
            .margin({ top: 4 })
        }
        .alignItems(HorizontalAlign.Center)
        .margin({ bottom: 80 })
    }
    .alignItems(HorizontalAlign.Center)
    .width('100%')
    .height('100%')
    .backgroundColor('#FFFFFF')
  }
}
