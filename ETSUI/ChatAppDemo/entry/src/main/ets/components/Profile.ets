/**
 * 个人中心组件
 * 负责人：待分配
 * 状态：已实现
 */
import router from '@ohos.router'
import { UserModel } from '../viewmodel/UserModel'

// 个人信息接口
// interface UserInfo {
//   avatar: Resource
//   nickName: string
//   region: string
// }

// 功能菜单项接口
interface MenuItem {
  icon: Resource
  title: string
  badge?: number
}

@Component
export struct Profile {

  // @StorageProp 是单向绑定（只读），@StorageLink 是双向绑定
  @StorageProp('currentUser') user: UserModel | null = null;

  // 模拟个人信息数据
  // private userInfo: UserInfo = {
  //   avatar: $r('app.media.user1'), // 使用现有资源
  //   nickName: '微信用户',
  //   region: '中国 北京'
  // }

  // 功能菜单数据
  private menuItems: MenuItem[][] = [
    [
      { icon: $r('app.media.user7'), title: '关于我们' },
      { icon: $r('app.media.settings'), title: '设置' }
    ]
  ]



  // 退出登录函数
  private handleLogout(): void {
    // 这里可以添加退出登录的逻辑，例如清除本地缓存等
    // 然后跳转到登录页面
    router.replaceUrl({
      url: 'pages/Login'
    })
  }

  build(): void {
    Column() {
      // 顶部个人信息区域
      Column() {
        if(this.user){
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
                      .padding({ left: 6, right: 6, top: 2, bottom: 2 })
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
                if (item.title === '关于我们') {
                  router.pushUrl({
                    url: 'pages/AboutWechat'
                  })
                }else if (item.title === '设置') {
                  router.pushUrl({
                    url: 'pages/Settings'
                  })
                }
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
        Text('退出登录')
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