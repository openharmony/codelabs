import router from '@ohos.router';

@Entry
@Component
struct AboutPage {
  build() {
    Column() {
      // Header
      Row() {
        Button() {
          Image($r('sys.media.ohos_ic_public_arrow_left'))
            .width(24)
            .height(24)
            .fillColor('#333333')
        }
        .width(40)
        .height(40)
        .backgroundColor(Color.Transparent)
        .onClick(() => {
          router.back();
        })

        Text('关于我们')
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')
          .layoutWeight(1)
          .textAlign(TextAlign.Center)

        // Placeholder for symmetry
        Row().width(40)
      }
      .width('100%')
      .padding({
        left: 16,
        right: 16,
        top: 12,
        bottom: 12
      })
      .backgroundColor('#FFFFFF')

      Scroll() {
        Column() {
          // App Logo and Info
          Column() {
            Image($r('app.media.startIcon'))
              .width(80)
              .height(80)
              .borderRadius(16)
              .margin({ bottom: 16 })
              .backgroundColor('#007DFF') // Fallback background

            Text('OpenHarmony 课表')
              .fontSize(24)
              .fontWeight(FontWeight.Bold)
              .fontColor('#333333')
              .margin({ bottom: 8 })

            Text('Version 1.0.0')
              .fontSize(14)
              .fontColor('#999999')
          }
          .width('100%')
          .padding({ top: 40, bottom: 40 })
          .alignItems(HorizontalAlign.Center)

          // Info List
          Column() {
            this.InfoItem('功能介绍', '一款基于 OpenHarmony 的简洁课程表应用，支持多周课程管理、桌面卡片、导入导出等功能。')

            Divider().color('#F0F0F0').margin({ left: 16, right: 16 })

            // Developer Team
            Column() {
              Text('开发者团队')
                .fontSize(16)
                .fontWeight(FontWeight.Bold)
                .fontColor('#333333')
                .width('100%')
                .margin({ bottom: 12 })

              this.DeveloperInfo('侯佳希', '3220251330', '3220251330@bit.edu.cn')
              this.DeveloperInfo('胡诗妍', '3220251418', '3220251418@bit.edu.cn')
              this.DeveloperInfo('李思涵', '3220251387', '3220251387@bit.edu.cn')
            }
            .width('100%')
            .padding(16)
            .alignItems(HorizontalAlign.Start)
          }
          .width('100%')
          .backgroundColor('#FFFFFF')
          .borderRadius(16)
          .padding({ top: 8, bottom: 8 })

          // Copyright
          Text('Copyright © 2026 Beijing Institute of Technology.\nAll Rights Reserved.')
            .fontSize(12)
            .fontColor('#CCCCCC')
            .margin({ top: 40, bottom: 20 })
            .textAlign(TextAlign.Center)
            .width('80%')

        }
        .width('100%')
        .padding(16)
      }
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5')
  }

  @Builder
  DeveloperInfo(name: string, id: string, email: string) {
    Column() {
      Row() {
        Text(name)
          .fontSize(15)
          .fontColor('#333333')
          .fontWeight(FontWeight.Medium)

        Blank()

        Text(id)
          .fontSize(14)
          .fontColor('#666666')
      }
      .width('100%')
      .margin({ bottom: 4 })

      Text(email)
        .fontSize(13)
        .fontColor('#999999')
        .width('100%')
    }
    .margin({ bottom: 12 })
  }

  @Builder
  InfoItem(title: string, content: string) {
    Column() {
      Text(title)
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .width('100%')
        .margin({ bottom: 8 })

      Text(content)
        .fontSize(14)
        .fontColor('#666666')
        .lineHeight(20)
    }
    .width('100%')
    .padding(16)
    .alignItems(HorizontalAlign.Start)
  }
}
