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
import router from '@ohos.router';
import { AppFontSize } from '../AppConfig/app_font_config'

@Entry
@Component
struct PixelUnitsPage {
  @State message: string = 'Hello World'
  private blueItemColor: string= "#f1faff"
  private blueTitleColor: string= "#2788d9"
  private Tag: string= "Ark_pixel_page"

  build() {
    Stack({ alignContent: Alignment.Bottom }) {
      Scroll() {
        Column() {
          Text('像素介绍')
            .fontSize(AppFontSize.TITLE)
            .fontWeight(FontWeight.Bold)
            .textAlign(TextAlign.Center)
            .align(Alignment.Top)
            .width("60%")
            .margin({ top: 40, bottom: 0 })
          List({ space: 20, initialIndex: 0 }) {
            ListItem() {
              Column() {
                Text('px')
                  .fontSize(AppFontSize.SUBTITLE)
                  .fontColor(this.blueTitleColor)
                  .align(Alignment.Start)
                  .width("100%")
                  .fontWeight(FontWeight.Bold)
                Text('屏幕物理像素单位。')
                  .width('100%')
                  .lineHeight(35)
                  .fontSize(AppFontSize.TEXT)
                  .textAlign(TextAlign.Start)
                  .borderRadius(10)
                Column() {
                  Text("200px")
                    .width(200 + 'px')
                    .height(25)
                    .backgroundColor(this.blueTitleColor)
                    .fontSize(AppFontSize.TEXT_SMALL)
                    .textAlign(TextAlign.Start)
                    .fontColor(Color.White)
                    .borderRadius(4)
                    .padding({ left: 5 })
                }
                .align(Alignment.Start)
                .width('100%')
                .alignItems(HorizontalAlign.Start)
                .margin({ top: 20 })


              }.align(Alignment.Start)


            }.editable(true)
            .padding(20)
            .backgroundColor(this.blueItemColor)
            .borderRadius(10)

            ListItem() {
              Column() {
                Text('vp')
                  .fontSize(AppFontSize.SUBTITLE)
                  .fontColor(this.blueTitleColor)
                  .align(Alignment.Start)
                  .width("100%")
                  .fontWeight(FontWeight.Bold)
                Text('屏幕密度相关像素，根据屏幕像素密度转换为屏幕物理像素。')
                  .width('100%')
                  .lineHeight(35)
                  .fontSize(AppFontSize.TEXT)
                  .textAlign(TextAlign.Start)
                  .borderRadius(10)
                Text('像素密度为160dpi的设备上1vp=1px,1vp对应的物理屏幕像素=（屏幕像素密度/160）px')
                  .width('100%')
                  .lineHeight(35)
                  .fontSize(AppFontSize.TEXT_SMALL)
                  .textAlign(TextAlign.Start)
                  .borderRadius(10)
                  .fontColor("#5f5f5f")
                Column() {
                  Text("200vp")
                    .width(200 + 'vp')
                    .height(25)
                    .backgroundColor(this.blueTitleColor)
                    .fontSize(AppFontSize.TEXT_SMALL)
                    .textAlign(TextAlign.Start)
                    .fontColor(Color.White)
                    .borderRadius(4)
                    .padding({ left: 5 })
                }
                .align(Alignment.Start)
                .width('100%')
                .alignItems(HorizontalAlign.Start)
                .margin({ top: 20 })

              }.align(Alignment.Start)

            }.editable(true)
            .padding(20)
            .backgroundColor(this.blueItemColor)
            .borderRadius(10)

            ListItem() {
              Column() {
                Text('fp')
                  .fontSize(AppFontSize.SUBTITLE)
                  .fontColor(this.blueTitleColor)
                  .align(Alignment.Start)
                  .width("100%")
                  .fontWeight(FontWeight.Bold)
                Text('字体像素，与vp类似，随系统字体大小设置变化。')
                  .width('100%')
                  .lineHeight(35)
                  .fontSize(AppFontSize.TEXT)
                  .textAlign(TextAlign.Start)
                  .borderRadius(10)
                Text('默认情况下与vp相同，即1vp=1fp,如果用户手动调整了系统字体，scale为缩放比例，设置后的字体大小（单位fp） = 设置前的字体大小 * scale')
                  .width('100%')
                  .lineHeight(35)
                  .fontSize(AppFontSize.TEXT_SMALL)
                  .textAlign(TextAlign.Start)
                  .borderRadius(10)
                  .fontColor("#5f5f5f")
                Column() {
                  Text("这是一段为14fp的文字")
                    .height(25)
                    .backgroundColor(this.blueTitleColor)
                    .fontSize(14 + 'fp')
                    .textAlign(TextAlign.Start)
                    .fontColor(Color.White)
                    .borderRadius(4)
                    .padding({ left: 5 })
                    .borderRadius(4)
                    .padding({ left: 5, right: 10 })
                }
                .align(Alignment.Start)
                .width('100%')
                .alignItems(HorizontalAlign.Start)
                .margin({ top: 20 })

                Column() {
                  Text("这是一段为24fp的文字")
                    .fontSize(24 + 'fp')
                    .backgroundColor(this.blueTitleColor)
                    .textAlign(TextAlign.Start)
                    .fontColor(Color.White)
                    .borderRadius(4)
                    .padding({ left: 5, right: 10, top: 5, bottom: 5 })
                }
                .align(Alignment.Start)
                .width('100%')
                .alignItems(HorizontalAlign.Start)
                .margin({ top: 20 })

              }.align(Alignment.Start)

            }.editable(true)
            .padding(20)
            .backgroundColor(this.blueItemColor)
            .borderRadius(10)

            ListItem() {
              Column() {
                Text('lpx')
                  .fontSize(AppFontSize.SUBTITLE)
                  .fontColor(this.blueTitleColor)
                  .align(Alignment.Start)
                  .width("100%")
                  .fontWeight(FontWeight.Bold)
                Text('视窗逻辑像素单位，lpx单位为实际屏幕宽度与逻辑宽度（通过designWidth配置）的比值。')
                  .width('100%')
                  .lineHeight(35)
                  .fontSize(AppFontSize.TEXT)
                  .textAlign(TextAlign.Start)
                  .borderRadius(10)

                Column() {
                  Text("200lpx")
                    .width(200 + 'lpx')
                    .height(25)
                    .backgroundColor(this.blueTitleColor)
                    .fontSize(AppFontSize.TEXT_SMALL)
                    .textAlign(TextAlign.Start)
                    .fontColor(Color.White)
                    .borderRadius(4)
                    .padding({ left: 5 })
                }
                .align(Alignment.Start)
                .width('100%')
                .alignItems(HorizontalAlign.Start)
                .margin({ top: 20 })

                Image($r('app.media.designWidth720'))
                  .border({ width: 1 })
                  .borderStyle(BorderStyle.Dashed)
                  .margin({ top: 10 })

                Image($r('app.media.designWidth1440'))
                  .border({ width: 1 })
                  .borderStyle(BorderStyle.Dashed)
                  .margin({ top: 10 })

              }.align(Alignment.Start)

            }.editable(true)
            .padding(20)
            .backgroundColor(this.blueItemColor)
            .borderRadius(10)
          }
          .padding(15).margin({ top: 20, bottom: 160 })


        }.height("100%")
      }

      Column() {
        Text('< 返回主页')
          .fontSize(AppFontSize.BUTTON)
          .fontWeight(FontWeight.Bold)
          .fontColor(Color.White)
      }
      .width('100%')
      .padding({ right: 10, left: 10, top: 20, bottom: 20 })
      .backgroundColor(this.blueTitleColor)
      .align(Alignment.Center)
      .onClick(() => {
        console.log(this.Tag + "click the button to back home")
        router.back()
      })
    }
  }
}