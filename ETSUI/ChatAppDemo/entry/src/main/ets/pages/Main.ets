import { ChatList } from '../components/ChatList'
import { Contact } from '../components/Contact'
import { Post } from '../components/Post'
import { Profile } from '../components/Profile'
@Entry
@Component
struct Main {
  @State curIndex: number = 0

  @Builder buildTabBar(title: string, normalImage: Resource, selectedImage: Resource, index: number) {
    Column() {
      Image(this.curIndex == index ? selectedImage : normalImage)
        .width(28)
      Text(title)
        .fontSize(12)
        .fontColor(this.curIndex == index ? Color.Green : Color.Black)
        .margin({
          top: 2
        })
    }
  }

  build() {
    Tabs() {
      TabContent() {
        ChatList()
      }.tabBar(this.buildTabBar('微信', $r("app.media.main_0_normal"), $r("app.media.main_0_select"), 0))

      TabContent() {
        Contact()
      }.tabBar(this.buildTabBar('通讯录', $r("app.media.main_1_normal"), $r("app.media.main_1_select"), 1))

      TabContent() {
        Post()
      }.tabBar(this.buildTabBar('动态', $r("app.media.main_2_normal"), $r("app.media.main_2_select"), 2))

      TabContent() {
        Profile()
      }.tabBar(this.buildTabBar('我', $r("app.media.main_3_normal"), $r("app.media.main_3_select"), 3))

    }.width('100%')
    .height('100%')
    .barPosition(BarPosition.End)
    .backgroundColor($r('app.color.main_background'))
    .onChange((index: number) => {
      this.curIndex = index
    })
  }
}