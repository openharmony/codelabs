/**
 * LifeHub 主页面
 * 包含底部导航栏和内容区域
 */
import { Dashboard } from './Dashboard';
import { HealthMain } from './health/HealthMain';
import { TravelMain } from './travel/TravelMain';
import { GoalsMain } from './goals/GoalsMain';
import { MoodMain } from './mood/MoodMain';
import { LearningMain } from './learning/LearningMain';
import { DiaryMain } from './diary/DiaryMain';

@Entry
@Component
struct Index {
  @State currentTab: number = 0; // 当前选中的标签页

  build() {
    Column() {
      // 内容区域
      if (this.currentTab === 0) {
        // 仪表盘
        Dashboard()
      } else if (this.currentTab === 1) {
        // 健康模块
        HealthMain()
      } else if (this.currentTab === 2) {
        // 旅行模块
        TravelMain()
      } else if (this.currentTab === 3) {
        // 目标模块
        GoalsMain()
      } else if (this.currentTab === 4) {
        // 心情模块
        MoodMain()
      } else if (this.currentTab === 5) {
        // 学习模块
        LearningMain()
      } else if (this.currentTab === 6) {
        // 日记模块
        DiaryMain()
      }

      // 底部导航栏
      this.buildBottomNav()
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  @Builder
  buildBottomNav() {
    Row() {
      // 仪表盘
      Column() {
        Text('🏠')
          .fontSize(24)
          .opacity(this.currentTab === 0 ? 1 : 0.5)
        Text('首页')
          .fontSize(12)
          .fontColor(this.currentTab === 0 ? '#007DFF' : '#999')
          .margin({ top: 4 })
      }
      .layoutWeight(1)
      .onClick(() => {
        this.currentTab = 0;
      })

      // 健康
      Column() {
        Text('💚')
          .fontSize(24)
          .opacity(this.currentTab === 1 ? 1 : 0.5)
        Text('健康')
          .fontSize(12)
          .fontColor(this.currentTab === 1 ? '#007DFF' : '#999')
          .margin({ top: 4 })
      }
      .layoutWeight(1)
      .onClick(() => {
        this.currentTab = 1;
      })

      // 旅行
      Column() {
        Text('✈️')
          .fontSize(24)
          .opacity(this.currentTab === 2 ? 1 : 0.5)
        Text('旅行')
          .fontSize(12)
          .fontColor(this.currentTab === 2 ? '#007DFF' : '#999')
          .margin({ top: 4 })
      }
      .layoutWeight(1)
      .onClick(() => {
        this.currentTab = 2;
      })

      // 目标
      Column() {
        Text('🎯')
          .fontSize(24)
          .opacity(this.currentTab === 3 ? 1 : 0.5)
        Text('目标')
          .fontSize(12)
          .fontColor(this.currentTab === 3 ? '#007DFF' : '#999')
          .margin({ top: 4 })
      }
      .layoutWeight(1)
      .onClick(() => {
        this.currentTab = 3;
      })

      // 心情
      Column() {
        Text('😊')
          .fontSize(24)
          .opacity(this.currentTab === 4 ? 1 : 0.5)
        Text('心情')
          .fontSize(12)
          .fontColor(this.currentTab === 4 ? '#007DFF' : '#999')
          .margin({ top: 4 })
      }
      .layoutWeight(1)
      .onClick(() => {
        this.currentTab = 4;
      })

      // 学习
      Column() {
        Text('📚')
          .fontSize(24)
          .opacity(this.currentTab === 5 ? 1 : 0.5)
        Text('学习')
          .fontSize(12)
          .fontColor(this.currentTab === 5 ? '#007DFF' : '#999')
          .margin({ top: 4 })
      }
      .layoutWeight(1)
      .onClick(() => {
        this.currentTab = 5;
      })

      // 日记
      Column() {
        Text('📝')
          .fontSize(24)
          .opacity(this.currentTab === 6 ? 1 : 0.5)
        Text('日记')
          .fontSize(12)
          .fontColor(this.currentTab === 6 ? '#007DFF' : '#999')
          .margin({ top: 4 })
      }
      .layoutWeight(1)
      .onClick(() => {
        this.currentTab = 6;
      })
    }
    .width('100%')
    .height(60)
    .padding({ top: 8, bottom: 8 })
    .backgroundColor(Color.White)
    .shadow({ radius: 8, color: '#10000000', offsetX: 0, offsetY: -2 })
  }
}
