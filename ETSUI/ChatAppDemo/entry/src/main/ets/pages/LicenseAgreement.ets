/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import { router } from '@kit.ArkUI';

interface AgreementSection {
  titleZh: ResourceStr;
  titleEn: ResourceStr;
  contentZh: ResourceStr;
  contentEn: ResourceStr;
}

@Entry
@Component
struct LicenseAgreementPage {
  // 模拟协议数据（可以放进你之前的 2000 行数据文件中导入）
  private agreementData: AgreementSection[] = [
    {
      titleZh: "1. 许可范围",
      titleEn: "1. Grant of License",
      contentZh: "本公司授予您一项非排他性的、不可转让的、有限的许可，允许您根据本协议条款安装及使用本软件。",
      contentEn: "The Company hereby grants you a non-exclusive, non-transferable, limited license to install and use the Software in accordance with the terms of this Agreement."
    },
    {
      titleZh: "2. 用户行为限制",
      titleEn: "2. User Restrictions",
      contentZh: "用户不得对本软件进行反向工程、反向编译或反向汇编。不得删除软件内的任何版权声明。",
      contentEn: "Users shall not reverse engineer, decompile, or disassemble the Software. No copyright notices within the Software may be removed."
    },
    {
      titleZh: "3. 隐私与数据保护",
      titleEn: "3. Privacy and Data Protection",
      contentZh: "我们重视您的隐私。我们将按照相关的隐私政策收集和使用您的个人数据，包括但不限于您选择的行政区域信息。",
      contentEn: "We value your privacy. We will collect and use your personal data in accordance with our Privacy Policy, including but not limited to the administrative region information you select."
    },
    {
      titleZh: "4. 免责声明",
      titleEn: "4. Disclaimer of Warranty",
      contentZh: "本软件按“现状”提供。公司不保证软件运行过程中不出现中断或错误。",
      contentEn: "The Software is provided 'AS IS'. The Company does not warrant that the operation of the software will be uninterrupted or error-free."
    }
  ];

  build() {
    Column() {
      // 自定义顶部导航栏
      Row() {
        Image($r('app.media.back')) // 确保你的资源库有返回图标，或改用 Text("<")
          .width(24)
          .height(24)
          .margin({ left: 16 })
          .onClick(() => {
            router.back(); // 返回上一页
          })

        Text("协议与条款 / Agreement")
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .margin({ left: 12 })
          .layoutWeight(1)

        Blank()
      }
      .width('100%')
      .height(56)
      .backgroundColor('#FFFFFF')
      .border({ width: { bottom: 0.5 }, color: '#E5E5E5' })

      // 协议内容滚动区
      List() {
        // 头部标题
        ListItem() {
          Column() {
            Text("软件许可及服务协议")
              .fontSize(22)
              .fontWeight(FontWeight.Bolder)
              .margin({ top: 30, bottom: 8 })
            Text("Software License and Service Agreement")
              .fontSize(14)
              .fontColor('#666666')
              .margin({ bottom: 30 })
          }.width('100%').alignItems(HorizontalAlign.Center)
        }

        // 循环渲染条款
        ForEach(this.agreementData, (item: AgreementSection) => {
          ListItem() {
            Column() {
              // 中文标题
              Text(item.titleZh)
                .fontSize(16)
                .fontWeight(FontWeight.Bold)
                .width('100%')
                .margin({ top: 20, bottom: 4 })

              // 英文标题
              Text(item.titleEn)
                .fontSize(14)
                .fontWeight(FontWeight.Medium)
                .fontColor('#444444')
                .width('100%')
                .margin({ bottom: 10 })

              // 中文正文
              Text(item.contentZh)
                .fontSize(15)
                .fontColor('#333333')
                .lineHeight(22)
                .width('100%')
                .margin({ bottom: 8 })

              // 英文正文
              Text(item.contentEn)
                .fontSize(13)
                .fontColor('#666666')
                .lineHeight(18)
                .width('100%')
                .margin({ bottom: 10 })

              Divider().color('#F0F0F0').strokeWidth(1).margin({ top: 10 })
            }
            .padding({ left: 20, right: 20 })
          }
        })

        // 底部留白
        ListItem() {
          Column().height(50)
        }
      }
      .width('100%')
      .layoutWeight(1)
      .edgeEffect(EdgeEffect.Spring) // 鸿蒙特色的回弹效果
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F7F7F7')
  }
}