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
import { AppFontSize } from '../AppConfig/app_font_config'

@Entry
@Component
struct PixelConversionPage {
  @State message: string = 'Hello World'
  private blueItemColor: string= "#f1faff"
  private blueColor: string= "#2788d9"
  private redColor: string= "#DE1a33"
  private yellowColor: number= 0xF9CF93
  private vp_px_width: number= 260
  private fp_px_width: number= 180
  private lpx_px_width: number= 200
  private Tag: string= 'Pixel_conversion_page'

  build() {
    Stack({ alignContent: Alignment.Bottom }) {
      Scroll() {
        Column() {

          Text('像素转换')
            .fontSize(AppFontSize.TITLE)
            .fontWeight(FontWeight.Bold)
            .textAlign(TextAlign.Center)
            .align(Alignment.Top)
            .width("60%")
            .margin({ top: 40, bottom: 20 })


          List({ space: 20, initialIndex: 0 }) {
            ListItem() {
              Column() {
                //vp转换为px
                Text('vp > px:')
                  .fontSize(AppFontSize.SUBTITLE)
                  .fontColor(this.blueColor)
                  .align(Alignment.Start)
                  .width("100%")
                  .fontWeight(FontWeight.Bold)
                  .margin({ bottom: 5, left: 10 })
                Column() {
                  Text('vp2px(' + this.vp_px_width + ')')
                    .height(25)
                    .fontColor(this.blueColor)
                    .fontSize(AppFontSize.TEXT_SMALL)
                    .textAlign(TextAlign.Center)

                }.margin(10)


                Column() {
                  Text()
                    .width(vp2px(this.vp_px_width))
                    .height(25)
                    .backgroundColor(this.redColor)
                    .fontSize(AppFontSize.TEXT_SMALL)
                    .textAlign(TextAlign.Start)
                    .fontColor(Color.White)
                }.margin(10)

                //px转换为vp
                Text('px > vp:')
                  .fontSize(AppFontSize.SUBTITLE)
                  .fontColor(this.blueColor)
                  .align(Alignment.Start)
                  .width("100%")
                  .fontWeight(FontWeight.Bold)
                  .margin({ bottom: 5, left: 10 })
                Column() {
                  Text('px2vp(' + this.vp_px_width + ')')
                    .height(25)
                    .fontColor(this.blueColor)
                    .fontSize(AppFontSize.TEXT_SMALL)
                    .textAlign(TextAlign.Center)
                }.margin(10)

                Column() {
                  Text()
                    .width(px2vp(this.vp_px_width) + 'vp')
                    .height(25)
                    .backgroundColor(this.redColor)
                    .fontSize(AppFontSize.TEXT_SMALL)
                    .textAlign(TextAlign.Start)
                    .fontColor(Color.White)
                }.margin(10)
              }.alignItems(HorizontalAlign.Start)

            }.padding({ top: 30, bottom: 30, left: 10 })
            .backgroundColor(this.blueItemColor)
            .borderRadius(10)

            ListItem() {
              //fp转换为px
              Column() {
                Text('fp > px:')
                  .fontSize(AppFontSize.SUBTITLE)
                  .fontColor(this.blueColor)
                  .align(Alignment.Start)
                  .width("100%")
                  .fontWeight(FontWeight.Bold)
                  .margin({ bottom: 5, left: 10 })
                Column() {
                  Text('fp2px(' + this.fp_px_width + ')')
                    .height(25)
                    .fontColor(this.blueColor)
                    .fontSize(AppFontSize.TEXT_SMALL)
                    .textAlign(TextAlign.Center)

                }.margin(10)

                Column() {
                  Text()
                    .width(fp2px(this.fp_px_width))
                    .height(25)
                    .backgroundColor(this.yellowColor)
                    .fontSize(AppFontSize.TEXT_SMALL)
                    .textAlign(TextAlign.Start)
                    .fontColor(Color.White)
                }.margin(10)
                //px转换为fp
                Text('px > fp:')
                  .fontSize(AppFontSize.SUBTITLE)
                  .fontColor(this.blueColor)
                  .align(Alignment.Start)
                  .width("100%")
                  .margin({ bottom: 5, left: 10 })
                  .fontWeight(FontWeight.Bold)
                Column() {
                  Text('px2fp(' + this.fp_px_width + ')')
                    .height(25)
                    .fontColor(this.blueColor)
                    .fontSize(AppFontSize.TEXT_SMALL)
                    .textAlign(TextAlign.Center)
                }.margin(10)

                Column() {
                  Text()
                    .width(px2fp(this.fp_px_width))
                    .height(25)
                    .backgroundColor(this.yellowColor)
                    .fontSize(AppFontSize.TEXT_SMALL)
                    .textAlign(TextAlign.Start)
                    .fontColor(Color.White)
                }.margin(10)
              }.alignItems(HorizontalAlign.Start)

            }.padding({ top: 30, bottom: 30, left: 10 })
            .backgroundColor(this.blueItemColor)
            .borderRadius(10)

            ListItem() {
              Column() {
                //lpx转换为px
                Text('lpx > px:')
                  .fontSize(AppFontSize.SUBTITLE)
                  .fontColor(this.blueColor)
                  .align(Alignment.Start)
                  .width("100%")
                  .fontWeight(FontWeight.Bold)
                  .margin({ bottom: 5, left: 10 })
                Column() {
                  Text('lpx2px(' + this.lpx_px_width + ')')
                    .height(25)
                    .fontColor(this.blueColor)
                    .fontSize(AppFontSize.TEXT_SMALL)
                    .textAlign(TextAlign.Center)
                }.margin(10)

                Column() {
                  Text()
                    .width(lpx2px(this.lpx_px_width))
                    .height(25)
                    .backgroundColor(this.blueColor)
                    .fontSize(AppFontSize.TEXT_SMALL)
                    .textAlign(TextAlign.Start)
                    .fontColor(Color.White)
                }.margin(10)
                //px转换为lpx
                Text('px > lpx:')
                  .fontSize(AppFontSize.SUBTITLE)
                  .fontColor(this.blueColor)
                  .align(Alignment.Start)
                  .width("100%")
                  .fontWeight(FontWeight.Bold)
                  .padding({ left: 10, right: 10 })
                Column() {
                  Text('px2lpx(' + this.lpx_px_width + ')')
                    .height(25)
                    .fontColor(this.blueColor)
                    .fontSize(AppFontSize.TEXT_SMALL)
                }.margin(10)

                Column() {
                  Text()
                    .width(px2lpx(this.lpx_px_width))
                    .height(25)
                    .backgroundColor(this.blueColor)
                    .fontSize(AppFontSize.TEXT_SMALL)
                    .textAlign(TextAlign.Start)
                    .fontColor(Color.White)
                }.margin(10)

                Text('lpx与px之间的转换，需要根据实际情况设置designWidth')
                  .width("100%")
                  .fontSize(AppFontSize.TEXT_SMALL)
                  .textAlign(TextAlign.Start)
                  .fontColor(Color.Red)
                  .margin({ bottom: 5, left: 10 })
              }.alignItems(HorizontalAlign.Start)
            }.padding({ top: 30, bottom: 30, left: 10 })
            .backgroundColor(this.blueItemColor)
            .borderRadius(10)
          }
          .borderRadius(10)
          .padding({ top: 20, bottom: 120, left: 15, right: 15 })


        }.height("100%")
      }.margin({ bottom: 80 })

      Column() {
        Text('< 返回主页')
          .fontSize(AppFontSize.BUTTON_LARGE)
          .fontWeight(FontWeight.Bold)
          .fontColor(Color.White)
          .padding({ top: 8, bottom: 8 })
      }
      .width('100%')
      .height(60)
      .backgroundColor(this.blueColor)
      .align(Alignment.Center)
      .onClick(() => {
        console.log(this.Tag + "click the button to back home")
        router.back()
      })
    }
  }
}
