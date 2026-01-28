import { Main } from './MainPage'
import { WaterPage } from './WaterPage'
import { DietPage } from './DietPage'
import { MeditationChoicePage } from './Meditation/MeditationChoicePage'
import { KnowledgePage } from './healthConsult';
import { ExercisePage } from './ExercisePage'

@Entry
@Component
struct MainPage {
  @State currentIndex: number = 0
  // 1. 引入全局缩放变量
  @StorageProp('appFontScale') fontScale: number = 1.0
  // 2. 增加控制器，用于代码控制页面切换
  private controller: TabsController = new TabsController()

  build() {
    // 使用 Column 布局：上面是内容(Tabs)，下面是导航栏(Row)
    Column() {
      // === 上半部分：内容区域 ===
      Tabs({ index: this.currentIndex, controller: this.controller }) {
        TabContent() {
          Main()
        }

        TabContent() {
          WaterPage()
        }

        TabContent() {
          DietPage()
        }

        TabContent() {
          ExercisePage()
        }

        TabContent() {
          MeditationChoicePage()
        }

        TabContent() {
          KnowledgePage()
        }
      }
      .vertical(false)
      .barHeight(0)
      .layoutWeight(1)
      .scrollable(false)
      .onChange((index: number) => {
        // 当页面滑动时，同步更新底部的选中状态
        this.currentIndex = index
      })

      // === 下半部分：自定义底部导航栏 ===
      Divider().color('#F0F0F0')

      Row() {
        // 这里的 this.fontScale 是响应式的，一旦变化，整个 Row 都会重绘
        this.tabItem(
          '我的',
          0,
          $r('app.media.startIcon')
        )
        this.tabItem(
          '饮水',
          1,
          $r('app.media.icon')
        )
        this.tabItem(
          '饮食',
          2,
          $r('app.media.icon')
        )
        this.tabItem(
          '运动',
          3,
          $r('app.media.icon')
        )
        this.tabItem(
          '冥想',
          4,
          $r('app.media.icon')
        )
        this.tabItem(
          '咨询',
          5,
          $r('app.media.icon')
        )
      }
      .width('100%')
      .height(50 + (10 * (this.fontScale - 1))) // 导航栏高度也可以随字体适当稍微变高一点点
      .backgroundColor(Color.White)
      .justifyContent(FlexAlign.SpaceAround) // 均匀分布
    }
    .width('100%')
    .height('100%')
  }

  // 自定义 Tab 按钮构建器
  @Builder
  tabItem(title: string, targetIndex: number, icon: Resource) {
    Column() {
      Image(icon)
        .size({
          width: 24,
          height: 24
        }) // 图标大小通常保持固定，或轻微缩放
        .objectFit(ImageFit.Contain)
        .fillColor(this.currentIndex === targetIndex ? '#007DFF' : '#999')
        .margin({ top: 5 })

      Text(title)
        .fontColor(this.currentIndex === targetIndex ? '#007DFF' : '#999')
        .fontSize(10 * this.fontScale)
        .margin({
          top: 2,
          bottom: 2
        })
    }
    .layoutWeight(1) // 等宽分布
    .height('100%')
    .justifyContent(FlexAlign.Center)
    // 点击事件
    .onClick(() => {
      this.currentIndex = targetIndex
      this.controller.changeIndex(targetIndex) // 切换 Tabs 页面
    })
  }
}