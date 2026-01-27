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

import { LengthMetrics, router } from '@kit.ArkUI'

@Styles function myfun() {
  .height('95%').width('48%')
}

@Entry
@Component
struct SummerSolsticeHealthy {
  myscr: Scroller = new Scroller()
  build() {
    Column() {
      // 返回按钮和标题
      Row() {
        Button({ type: ButtonType.Circle }).backgroundImage($r('app.media.return1')).size({ width: 20 })
          .backgroundColor(Color.White).backgroundImageSize(ImageSize.FILL).onClick(() => {
          router.back()
        })
        Text("立夏").fontSize(25).fontWeight(FontWeight.Bold).margin({left:120})
      }.height('5%').width('100%').margin({ top: 2})

      // 滚动内容区域
      Scroll(this.myscr) {
        Column() {
          // 大图区域
          Text()
            .width('100%')
            .height('30%')
            .backgroundColor(Color.Green)
            .backgroundImage($r('app.media.lixia'))
            .backgroundImageSize(ImageSize.FILL)

          // 开篇文案
          Column() {
            Text("立夏养生 | 多吃这四类食物，养心又消暑")
              .fontSize(20)
              .fontWeight(FontWeight.Bold)
              .width('95%')
              .padding({ left: 5 })
              .margin({ top: 8, bottom: 4 })
              .textAlign(TextAlign.Start)

            Text("立夏，是二十四节气中的第七个节气，标志着夏季的开始。此时气温逐渐升高，天气转热，中医认为，立夏时节养生要注重" +
              "**养心消暑**，健脾祛湿。以下四类食物，是立夏时节的养生佳品，助您顺应节气变化，保持身体健康。")
              .fontSize(16)
              .lineSpacing(LengthMetrics.vp(12))
              .width('95%')
              .margin({ bottom: 10 })
          }.width('100%')

          // 四类食物介绍
          // 1. 清热解暑类
          Column() {
            Text('1. 清热解暑类 - 降火祛暑')
              .fontColor(Color.Red)
              .fontSize(18)
              .fontWeight(FontWeight.Bold)
              .width('95%')
              .margin({ top: 10, bottom: 4 })

            Text("夏季天气炎热，容易上火，食用清热解暑类食物能有效缓解暑热带来的不适。这类食物多性寒凉，有清热解毒、生津止渴的功效，适合立夏时节食用。")
              .lineSpacing(LengthMetrics.vp(12))
              .width('95%')
              .margin({ bottom: 8 })

            Text("推荐食物：绿豆")
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .lineSpacing(LengthMetrics.vp(12))
              .width('95%')
              .margin({ bottom: 4 })

            Text("绿豆性凉味甘，有清热解毒、消暑除烦的作用。夏季喝绿豆汤，既能补充水分，又能清热解暑，是立夏时节的理想饮品。")
              .lineSpacing(LengthMetrics.vp(12))
              .width('95%')
              .margin({ bottom: 8 })

            Text()
              .width('100%')
              .height('40%')
              .backgroundColor(Color.Gray)
              .backgroundImage($r('app.media.lvdou'))
              .backgroundImageSize(ImageSize.FILL)
              .margin({ bottom: 10 })
          }.width('100%')

          // 2. 养心安神类
          Column() {
            Text('2. 养心安神类 - 调养心神')
              .fontColor(Color.Red)
              .fontSize(18)
              .fontWeight(FontWeight.Bold)
              .width('95%')
              .margin({ top: 10, bottom: 4 })

            Text("夏季属火，对应心脏，天气炎热容易使人烦躁不安，因此立夏时节要特别注意养心安神。这类食物多含有丰富的维生素和矿物质，能够调节神经系统，缓解焦虑情绪。")
              .lineSpacing(LengthMetrics.vp(12))
              .width('95%')
              .margin({ bottom: 8 })

            Text("推荐食物：莲子")
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .lineSpacing(LengthMetrics.vp(12))
              .width('95%')
              .margin({ bottom: 4 })

            Text("莲子性平味甘涩，有养心安神、健脾止泻的功效。夏季食用莲子，能够缓解心悸、失眠等症状，对心脾两虚引起的食欲不振也有很好的调理作用。")
              .lineSpacing(LengthMetrics.vp(12))
              .width('95%')
              .margin({ bottom: 8 })

            Text()
              .width('100%')
              .height('40%')
              .backgroundColor(Color.Gray)
              .backgroundImage($r('app.media.lianzi'))
              .backgroundImageSize(ImageSize.FILL)
              .margin({ bottom: 10 })
          }.width('100%')

          // 3. 健脾祛湿类
          Column() {
            Text('3. 健脾祛湿类 - 调理脾胃')
              .fontColor(Color.Red)
              .fontSize(18)
              .fontWeight(FontWeight.Bold)
              .width('95%')
              .margin({ top: 10, bottom: 4 })

            Text("立夏时节，雨水增多，空气湿度大，人体容易受到湿邪侵袭，导致脾胃功能减弱。食用健脾祛湿类食物，能够增强脾胃功能，促进消化，去除体内湿气。")
              .lineSpacing(LengthMetrics.vp(12))
              .width('95%')
              .margin({ bottom: 8 })

            Text("推荐食物：冬瓜")
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .lineSpacing(LengthMetrics.vp(12))
              .width('95%')
              .margin({ bottom: 4 })

            Text("冬瓜性凉味甘淡，有清热利水、消肿解毒的作用。夏季食用冬瓜，能够促进体内水分代谢，消除水肿，对湿热引起的小便不利有很好的缓解作用。")
              .lineSpacing(LengthMetrics.vp(12))
              .width('95%')
              .margin({ bottom: 8 })

            Text()
              .width('100%')
              .height('40%')
              .backgroundColor(Color.Gray)
              .backgroundImage($r('app.media.dg'))
              .backgroundImageSize(ImageSize.FILL)
              .margin({ bottom: 10 })
          }.width('100%')

          // 4. 滋阴润燥类
          Column() {
            Text('4. 滋阴润燥类 - 补充水分')
              .fontColor(Color.Red)
              .fontSize(18)
              .fontWeight(FontWeight.Bold)
              .width('95%')
              .margin({ top: 10, bottom: 4 })

            Text("夏季天气炎热，人体水分流失快，容易出现口干舌燥、皮肤干燥等症状。食用滋阴润燥类食物，能够补充身体所需水分，保持皮肤水润，预防中暑。")
              .lineSpacing(LengthMetrics.vp(12))
              .width('95%')
              .margin({ bottom: 8 })

            Text("推荐食物：西瓜")
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .lineSpacing(LengthMetrics.vp(12))
              .width('95%')
              .margin({ bottom: 4 })

            Text("西瓜性寒味甘，有清热解暑、生津止渴的功效。夏季吃西瓜，能够快速补充水分和电解质，缓解暑热带来的不适，是夏季消暑的佳品。")
              .lineSpacing(LengthMetrics.vp(12))
              .width('95%')
              .margin({ bottom: 8 })

            Text()
              .width('100%')
              .height('40%')
              .backgroundColor(Color.Gray)
              .backgroundImage($r('app.media.xigua'))
              .backgroundImageSize(ImageSize.FILL)
              .margin({ bottom: 15 })
          }.width('100%')

          // 结束语
          Text("立夏时节，让我们顺应自然规律，通过合理的饮食调养身体。这四类食物既能养心消暑，又能健脾祛湿，是立夏养生的绝佳选择。你最喜欢哪一种呢？欢迎在评论区分享你的立夏养生食谱！")
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .lineSpacing(LengthMetrics.vp(12))
            .width('95%')
            .margin({ bottom: 20 })
        }.width('100%')
      }.scrollable(ScrollDirection.Vertical)
    }
  }
}