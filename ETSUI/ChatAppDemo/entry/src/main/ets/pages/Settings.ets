import router from '@ohos.router';

@Entry
@Component
struct Settings {
  build() {
    Column() {
      // --- 自定义导航栏 ---
      Row() {
        Image($r('app.media.back')) // 请确保你有返回图标，或者用 Text('<') 代替
          .width(24)
          .height(24)
          .onClick(() => {
            router.back()
          })

        Text('设置')
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .margin({ left: 16 })

        Blank() // 自动撑开中间空间
      }
      .width('100%')
      .height(56)
      .padding({ left: 16, right: 16 })
      .backgroundColor(Color.White)

      // 设置项列表
      List() {
        // 切换语言
        ListItem() {
          this.SettingItem('多语言切换', () => {
            router.pushUrl({ url: 'pages/LanguageSettings' }) // 替换为你实际的路径
          })
        }

        // 修改密码
        ListItem() {
          this.SettingItem('修改密码', () => {
            router.pushUrl({ url: 'pages/ChangePassword' }) // 替换为你实际的路径
          })
        }

        // 关于我们
        ListItem() {
          this.SettingItem('关于我们', () => {
            // 暂时留空或弹个窗
          })
        }
      }
      .width('100%')
      .backgroundColor(Color.White)
      .margin({ top: 12 })
      .divider({ strokeWidth: 0.5, color: '#f0f0f0', startMargin: 16, endMargin: 16 }) // 分割线
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5') // 设置页面常用的浅灰色背景
  }

  // 封装设置项组件
  @Builder SettingItem(title: string, action: () => void) {
    Row() {
      Text(title)
        .fontSize(16)
        .fontColor('#181818')

      Blank()

      Image($r('app.media.arrow_right')) // 右侧箭头
        .width(16)
        .height(16)
        .opacity(0.4)
    }
    .width('100%')
    .height(64)
    .padding({ left: 16, right: 16 })
    .onClick(() => {
      action()
    })
  }
}