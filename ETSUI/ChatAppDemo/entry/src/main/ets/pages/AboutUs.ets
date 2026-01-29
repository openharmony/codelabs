/**
 * 关于本APP页面
 * 负责人：待分配
 * 状态：已实现
 */
import router from '@ohos.router'

@Entry
@Component
struct AboutWechat {
  build() {
    Column() {
      // 顶部导航栏
      Row() {
        // 返回按钮
        Button() {
          Text('返回')
            .fontSize(16)
            .fontColor('#007AFF')
        }
        .backgroundColor('#FFFFFF')
        .margin({ left: 16 })
        .onClick(() => {
          router.back()
        })

        // 页面标题
        Text('关于聊天APP')
          .fontSize(18)
          .fontWeight(700)
          .flexGrow(1)
          .textAlign(TextAlign.Center)

        // 占位元素，保持导航栏平衡
        Text('')
          .width(80)
      }
      .width('100%')
      .height(56)
      .backgroundColor('#FFFFFF')
      .alignItems(VerticalAlign.Center)
      .shadow({ radius: 2, color: '#00000020', offsetY: 2 })

      // 内容区域
      Column() {
        // APP图标
        Image($r('app.media.user7'))
          .width(80)
          .height(80)
          .margin({ bottom: 16 })

        // APP名称
        Text('聊天APP')
          .fontSize(20)
          .fontWeight(700)
          .margin({ bottom: 8 })

        // 版本信息
        Text('版本 8.0.28')
          .fontSize(14)
          .fontColor('#666666')
          .margin({ bottom: 24 })

        // 版权信息
        Text('© 2025 Tencent. All rights reserved.')
          .fontSize(12)
          .fontColor('#999999')
      }
      .width('100%')
      .layoutWeight(1)
      .padding(24)
      .alignItems(HorizontalAlign.Center)
      .justifyContent(FlexAlign.Center)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5')
  }
}