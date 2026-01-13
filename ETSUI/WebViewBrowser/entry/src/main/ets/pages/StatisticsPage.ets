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

import { dataManager, BookmarkItem, HistoryItem } from '../manager/DataManager';

// Builder函数，用于Navigation路由
@Builder
export function StatisticsPageBuilder() {
  StatisticsPage();
}

// 顶级访问站点接口
interface TopSite {
  url: string;
  title: string;
  count: number;
}

// 统计数据接口
interface StatisticsData {
  totalHistory: number;
  totalBookmarks: number;
  todayVisits: number;
  weekVisits: number;
  monthVisits: number;
  topVisitedSites: TopSite[];
  recentBookmarks: BookmarkItem[];
}

@Component
export struct StatisticsPage {
  // 获取全局NavPathStack
  pathStack: NavPathStack = AppStorage.get('pathStack') as NavPathStack;
  // 统计数据
  @State statistics: StatisticsData = {
    totalHistory: 0,
    totalBookmarks: 0,
    todayVisits: 0,
    weekVisits: 0,
    monthVisits: 0,
    topVisitedSites: [],
    recentBookmarks: []
  };
  // 深色模式
  @StorageLink('darkMode') darkMode: boolean = false;

  aboutToAppear() {
    this.loadStatistics();
  }

  // 加载统计数据
  loadStatistics() {
    const historyList = dataManager.getHistoryList();
    const bookmarkList = dataManager.getBookmarkList();

    // 基础统计
    this.statistics.totalHistory = historyList.length;
    this.statistics.totalBookmarks = bookmarkList.length;

    // 时间统计
    const now = Date.now();
    const today = new Date();
    today.setHours(0, 0, 0, 0);
    const todayTime = today.getTime();
    const weekAgo = todayTime - 7 * 24 * 60 * 60 * 1000;
    const monthAgo = todayTime - 30 * 24 * 60 * 60 * 1000;

    this.statistics.todayVisits = historyList.filter(item => item.visitTime >= todayTime).length;
    this.statistics.weekVisits = historyList.filter(item => item.visitTime >= weekAgo).length;
    this.statistics.monthVisits = historyList.filter(item => item.visitTime >= monthAgo).length;

    // 最常访问的网站（按访问次数排序，取前5个）
    const sortedHistory = [...historyList].sort((a, b) => b.visitCount - a.visitCount);
    this.statistics.topVisitedSites = sortedHistory.slice(0, 5).map((item): TopSite => {
      return {
        url: item.url,
        title: item.title,
        count: item.visitCount
      };
    });

    // 最近的书签（取前5个）
    this.statistics.recentBookmarks = bookmarkList.slice(0, 5);
  }

  build() {
    NavDestination() {
      Column() {
        // 自定义标题栏
        Row() {
          // 返回按钮
          Image($r('app.media.back'))
            .width(24)
            .height(24)
            .fillColor(this.darkMode ? '#FFFFFF' : '#000000')
            .margin({ left: 16 })
            .onClick(() => {
              this.pathStack.pop();
            })

          Text('统计信息')
            .fontSize(18)
            .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
            .fontWeight(FontWeight.Medium)
            .margin({ left: 16 })

          Blank()
        }
        .width('100%')
        .height(56)
        .backgroundColor(this.darkMode ? '#1C1C1E' : '#FFFFFF')
        .alignItems(VerticalAlign.Center)

        // 内容区域
        Scroll() {
          Column({ space: 16 }) {
            // 总览卡片
            this.OverviewCard()

            // 访问统计卡片
            this.VisitStatsCard()

            // 热门网站卡片
            this.TopSitesCard()

            // 最近书签卡片
            this.RecentBookmarksCard()
          }
          .width('100%')
          .padding(16)
        }
        .layoutWeight(1)
        .backgroundColor(this.darkMode ? '#000000' : '#F2F2F7')
      }
      .width('100%')
      .height('100%')
    }
    .hideTitleBar(true)
  }

  // 总览卡片
  @Builder
  OverviewCard() {
    Column({ space: 12 }) {
      Text('数据概览')
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
        .width('100%')

      Row({ space: 12 }) {
        // 历史记录数
        Column({ space: 8 }) {
          Text(`${this.statistics.totalHistory}`)
            .fontSize(28)
            .fontWeight(FontWeight.Bold)
            .fontColor('#007AFF')

          Text('历史记录')
            .fontSize(12)
            .fontColor('#8E8E93')
        }
        .layoutWeight(1)
        .height(80)
        .backgroundColor(this.darkMode ? '#2C2C2E' : '#FFFFFF')
        .borderRadius(12)
        .justifyContent(FlexAlign.Center)
        .shadow({ radius: 8, color: this.darkMode ? '#00000040' : '#00000010', offsetY: 2 })

        // 书签数
        Column({ space: 8 }) {
          Text(`${this.statistics.totalBookmarks}`)
            .fontSize(28)
            .fontWeight(FontWeight.Bold)
            .fontColor('#FF9500')

          Text('收藏书签')
            .fontSize(12)
            .fontColor('#8E8E93')
        }
        .layoutWeight(1)
        .height(80)
        .backgroundColor(this.darkMode ? '#2C2C2E' : '#FFFFFF')
        .borderRadius(12)
        .justifyContent(FlexAlign.Center)
        .shadow({ radius: 8, color: this.darkMode ? '#00000040' : '#00000010', offsetY: 2 })
      }
      .width('100%')
    }
    .width('100%')
    .padding(16)
    .backgroundColor(this.darkMode ? '#1C1C1E' : '#FFFFFF')
    .borderRadius(12)
    .shadow({ radius: 8, color: this.darkMode ? '#00000040' : '#00000010', offsetY: 2 })
  }

  // 访问统计卡片
  @Builder
  VisitStatsCard() {
    Column({ space: 12 }) {
      Text('访问统计')
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
        .width('100%')

      // 今天
      Row() {
        Text('今天')
          .fontSize(14)
          .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
          .layoutWeight(1)

        Text(`${this.statistics.todayVisits} 次`)
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .fontColor('#34C759')
      }
      .width('100%')
      .height(44)
      .padding({ left: 12, right: 12 })
      .backgroundColor(this.darkMode ? '#2C2C2E' : '#F2F2F7')
      .borderRadius(8)

      // 最近7天
      Row() {
        Text('最近7天')
          .fontSize(14)
          .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
          .layoutWeight(1)

        Text(`${this.statistics.weekVisits} 次`)
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .fontColor('#007AFF')
      }
      .width('100%')
      .height(44)
      .padding({ left: 12, right: 12 })
      .backgroundColor(this.darkMode ? '#2C2C2E' : '#F2F2F7')
      .borderRadius(8)

      // 最近30天
      Row() {
        Text('最近30天')
          .fontSize(14)
          .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
          .layoutWeight(1)

        Text(`${this.statistics.monthVisits} 次`)
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .fontColor('#FF9500')
      }
      .width('100%')
      .height(44)
      .padding({ left: 12, right: 12 })
      .backgroundColor(this.darkMode ? '#2C2C2E' : '#F2F2F7')
      .borderRadius(8)
    }
    .width('100%')
    .padding(16)
    .backgroundColor(this.darkMode ? '#1C1C1E' : '#FFFFFF')
    .borderRadius(12)
    .shadow({ radius: 8, color: this.darkMode ? '#00000040' : '#00000010', offsetY: 2 })
  }

  // 热门网站卡片
  @Builder
  TopSitesCard() {
    Column({ space: 12 }) {
      Text('最常访问')
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
        .width('100%')

      if (this.statistics.topVisitedSites.length === 0) {
        Column() {
          Text('暂无数据')
            .fontSize(14)
            .fontColor('#8E8E93')
        }
        .width('100%')
        .height(100)
        .justifyContent(FlexAlign.Center)
      } else {
        Column() {
          ForEach(this.statistics.topVisitedSites, (site: TopSite, index: number) => {
            Row({ space: 12 }) {
              // 排名
              Text(`${index + 1}`)
                .width(24)
                .height(24)
                .fontSize(14)
                .fontWeight(FontWeight.Bold)
                .fontColor('#FFFFFF')
                .textAlign(TextAlign.Center)
                .borderRadius(12)
                .backgroundColor(this.getRankColor(index))

              // 网站信息
              Column({ space: 4 }) {
                Text(site.title || '无标题')
                  .fontSize(14)
                  .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
                  .maxLines(1)
                  .textOverflow({ overflow: TextOverflow.Ellipsis })

                Text(site.url)
                  .fontSize(12)
                  .fontColor('#8E8E93')
                  .maxLines(1)
                  .textOverflow({ overflow: TextOverflow.Ellipsis })
              }
              .alignItems(HorizontalAlign.Start)
              .layoutWeight(1)

              // 访问次数
              Text(`${site.count}次`)
                .fontSize(14)
                .fontColor('#8E8E93')
            }
            .width('100%')
            .padding(12)
            .backgroundColor(this.darkMode ? '#2C2C2E' : '#F2F2F7')
            .borderRadius(8)
            .margin({ bottom: 8 })
          })
        }
        .width('100%')
      }
    }
    .width('100%')
    .padding(16)
    .backgroundColor(this.darkMode ? '#1C1C1E' : '#FFFFFF')
    .borderRadius(12)
    .shadow({ radius: 8, color: this.darkMode ? '#00000040' : '#00000010', offsetY: 2 })
  }

  // 最近书签卡片
  @Builder
  RecentBookmarksCard() {
    Column({ space: 12 }) {
      Text('最近书签')
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
        .width('100%')

      if (this.statistics.recentBookmarks.length === 0) {
        Column() {
          Text('暂无书签')
            .fontSize(14)
            .fontColor('#8E8E93')
        }
        .width('100%')
        .height(100)
        .justifyContent(FlexAlign.Center)
      } else {
        Column() {
          ForEach(this.statistics.recentBookmarks, (bookmark: BookmarkItem) => {
            Row({ space: 12 }) {
              // 图标
              Column() {
                Image($r('app.media.bookmark_filled'))
                  .width(16)
                  .height(16)
                  .fillColor('#FFFFFF')
              }
              .width(36)
              .height(36)
              .borderRadius(8)
              .backgroundColor('#FF9500')
              .justifyContent(FlexAlign.Center)

              // 书签信息
              Column({ space: 4 }) {
                Text(bookmark.title || '无标题')
                  .fontSize(14)
                  .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
                  .maxLines(1)
                  .textOverflow({ overflow: TextOverflow.Ellipsis })

                Text(this.formatDate(bookmark.createTime))
                  .fontSize(12)
                  .fontColor('#8E8E93')
              }
              .alignItems(HorizontalAlign.Start)
              .layoutWeight(1)
            }
            .width('100%')
            .padding(12)
            .backgroundColor(this.darkMode ? '#2C2C2E' : '#F2F2F7')
            .borderRadius(8)
            .margin({ bottom: 8 })
          })
        }
        .width('100%')
      }
    }
    .width('100%')
    .padding(16)
    .backgroundColor(this.darkMode ? '#1C1C1E' : '#FFFFFF')
    .borderRadius(12)
    .shadow({ radius: 8, color: this.darkMode ? '#00000040' : '#00000010', offsetY: 2 })
  }

  // 获取排名颜色
  getRankColor(index: number): string {
    switch (index) {
      case 0:
        return '#FFD700'; // 金色
      case 1:
        return '#C0C0C0'; // 银色
      case 2:
        return '#CD7F32'; // 铜色
      default:
        return '#8E8E93'; // 灰色
    }
  }

  // 格式化日期
  formatDate(timestamp: number): string {
    const date = new Date(timestamp);
    return `${date.getMonth() + 1}月${date.getDate()}日`;
  }
}
