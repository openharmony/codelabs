/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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
import { router } from '@kit.ArkUI';

interface IndexRouteParams {
  tab?: number
}

@Entry
@Component
struct Index {
  @State currentTab: number = 0; // 当前选中的标签页

  aboutToAppear() {
    let params: IndexRouteParams | null = null

    try {
      params = router.getParams() as IndexRouteParams
    } catch (e) {
      // 冷启动 / 非路由进入，会走到这里，直接忽略
      return
    }

    if (params && typeof params.tab === 'number') {
      this.currentTab = params.tab
    }
  }

  build() {
    Column() {
      // 内容区域，占满剩余空间
      Column() {
        if (this.currentTab === 0) {
          Dashboard()
        } else if (this.currentTab === 1) {
          HealthMain()
        } else if (this.currentTab === 2) {
          TravelMain()
        } else if (this.currentTab === 3) {
          GoalsMain()
        } else if (this.currentTab === 4) {
          MoodMain()
        } else if (this.currentTab === 5) {
          LearningMain()
        } else if (this.currentTab === 6) {
          DiaryMain()
        }
      }
      .layoutWeight(1)

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
    .margin({ bottom: 16 })
    .backgroundColor(Color.White)
    .shadow({
      radius: 8,
      color: '#10000000',
      offsetX: 0,
      offsetY: -2
    })
  }
}
