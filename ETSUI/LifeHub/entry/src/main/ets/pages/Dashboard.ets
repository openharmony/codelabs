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
 * 仪表盘页面
 * 展示各模块的关键数据和快速入口
 */
import { LifeHubData, DashboardStats } from '../model/LifeHubData';
import { router } from '@kit.ArkUI';

@Component
export struct Dashboard {
  private lifeHubData: LifeHubData = new LifeHubData();
  @State healthRecordsToday: number = 0;
  @State activeGoals: number = 0;
  @State moodToday: string = '😊';
  @State skillsCount: number = 0;
  @State travelPlansActive: number = 0;
  @State isLoading: boolean = true;

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
      // 延迟加载，确保数据库初始化完成
      setTimeout(() => {
        this.loadDashboardData();
      }, 500);
    } else {
      this.loadDashboardData();
    }
  }

  async loadDashboardData() {
    this.isLoading = true;
    try {
      // 等待一下确保数据库初始化完成
      await new Promise<void>((resolve: () => void) => setTimeout(resolve, 300));
      const stats: DashboardStats = await this.lifeHubData.getDashboardStats();
      console.log('Dashboard: 统计数据:', JSON.stringify(stats));
      this.healthRecordsToday = stats.healthRecordsToday;
      this.activeGoals = stats.activeGoals;
      this.moodToday = stats.moodToday?.mood || '😊';
      this.skillsCount = stats.skillsCount;
      this.travelPlansActive = stats.travelPlansActive;
      console.log('Dashboard: 数据已更新', {
        healthRecordsToday: this.healthRecordsToday,
        activeGoals: this.activeGoals,
        moodToday: this.moodToday,
        skillsCount: this.skillsCount,
        travelPlansActive: this.travelPlansActive
      });
    } catch (error) {
      console.error('Dashboard: 加载数据失败:', error);
    } finally {
      this.isLoading = false;
    }
  }

  // build() {
  //   Scroll() {
  //     Column() {
  //       // 标题
  //       Row() {
  //         Text('生活中心')
  //           .fontSize(28)
  //           .fontWeight(FontWeight.Bold)
  //
  //         Blank()
  //       }
  //       .width('100%')
  //       .padding({ left: 20, right: 20, top: 20, bottom: 10 })
  //
  //       // 今日概览
  //       Text('今日概览')
  //         .fontSize(18)
  //         .fontWeight(FontWeight.Medium)
  //         .textAlign(TextAlign.Start)
  //         .width('100%')
  //         .padding({ left: 20, right: 20, top: 10, bottom: 10 })
  //
  //       // 数据卡片
  //       Row() {
  //         // 健康
  //         this.buildStatCard('健康', '💚', this.healthRecordsToday > 0 ? this.healthRecordsToday.toString() : '0', '今日记录', '#4CAF50', () => {
  //           // 跳转到健康模块
  //         })
  //
  //         // 目标
  //         this.buildStatCard('目标', '🎯', this.activeGoals > 0 ? this.activeGoals.toString() : '0', '进行中', '#FF9800', () => {
  //           // 跳转到目标模块
  //         })
  //       }
  //       .width('100%')
  //       .padding({ left: 15, right: 15, bottom: 10 })
  //       .justifyContent(FlexAlign.SpaceBetween)
  //
  //       Row() {
  //         // 心情
  //         this.buildStatCard('心情', this.moodToday, '', '今日心情', '#E91E63', () => {
  //           // 跳转到心情模块
  //         })
  //
  //         // 学习
  //         this.buildStatCard('学习', '📚', this.skillsCount > 0 ? this.skillsCount.toString() : '0', '技能数', '#2196F3', () => {
  //           // 跳转到学习模块
  //         })
  //       }
  //       .width('100%')
  //       .padding({ left: 15, right: 15, bottom: 10 })
  //       .justifyContent(FlexAlign.SpaceBetween)
  //
  //       // 快速入口
  //       Text('快速入口')
  //         .fontSize(18)
  //         .fontWeight(FontWeight.Medium)
  //         .textAlign(TextAlign.Start)
  //         .width('100%')
  //         .padding({ left: 20, right: 20, top: 20, bottom: 10 })
  //
  //       // 功能入口网格
  //       Grid() {
  //         GridItem() {
  //           this.buildQuickEntry('记录健康', '💚', '#4CAF50', () => {
  //             router.pushUrl({ url: 'pages/health/HealthRecord' });
  //           })
  //         }
  //
  //         GridItem() {
  //           this.buildQuickEntry('创建目标', '🎯', '#FF9800', () => {
  //             router.pushUrl({ url: 'pages/goals/GoalDetail' });
  //           })
  //         }
  //
  //         GridItem() {
  //           this.buildQuickEntry('记录心情', '😊', '#E91E63', () => {
  //             router.pushUrl({ url: 'pages/mood/MoodRecord' });
  //           })
  //         }
  //
  //         GridItem() {
  //           this.buildQuickEntry('规划旅行', '✈️', '#2196F3', () => {
  //             router.pushUrl({ url: 'pages/travel/TravelPlan' });
  //           })
  //         }
  //
  //         GridItem() {
  //           this.buildQuickEntry('添加技能', '📚', '#9C27B0', () => {
  //             router.pushUrl({ url: 'pages/learning/SkillDetail' });
  //           })
  //         }
  //
  //         GridItem() {
  //           this.buildQuickEntry('写日记', '📝', '#00BCD4', () => {
  //             router.pushUrl({ url: 'pages/diary/DiaryEdit' });
  //           })
  //         }
  //       }
  //       .columnsTemplate('1fr 1fr 1fr')
  //       .rowsTemplate('1fr 1fr')
  //       .columnsGap(10)
  //       .rowsGap(10)
  //       .width('100%')
  //       .height(200)
  //       .padding({ left: 15, right: 15, bottom: 20 })
  //
  //       // 更多功能
  //       Text('更多功能')
  //         .fontSize(18)
  //         .fontWeight(FontWeight.Medium)
  //         .textAlign(TextAlign.Start)
  //         .width('100%')
  //         .padding({ left: 20, right: 20, top: 20, bottom: 10 })
  //
  //       Grid() {
  //         GridItem() {
  //           this.buildQuickEntry('数据统计', '📊', '#607D8B', () => {
  //             router.pushUrl({ url: 'pages/statistics/StatisticsMain' });
  //           })
  //         }
  //
  //         GridItem() {
  //           this.buildQuickEntry('统一搜索', '🔍', '#795548', () => {
  //             router.pushUrl({ url: 'pages/search/SearchMain' });
  //           })
  //         }
  //
  //         GridItem() {
  //           this.buildQuickEntry('成就系统', '🏆', '#FFD700', () => {
  //             router.pushUrl({ url: 'pages/achievements/AchievementsMain' });
  //           })
  //         }
  //
  //         GridItem() {
  //           this.buildQuickEntry('生活报告', '📄', '#9E9E9E', () => {
  //             router.pushUrl({ url: 'pages/report/ReportMain' });
  //           })
  //         }
  //
  //         GridItem() {
  //           this.buildQuickEntry('设置', '⚙️', '#424242', () => {
  //             router.pushUrl({ url: 'pages/settings/SettingsMain' });
  //           })
  //         }
  //       }
  //       .columnsTemplate('1fr 1fr 1fr')
  //       .rowsTemplate('1fr 1fr')
  //       .columnsGap(10)
  //       .rowsGap(10)
  //       .width('100%')
  //       .height(200)
  //       .padding({ left: 15, right: 15, bottom: 20 })
  //
  //       // 工具功能
  //       Text('工具功能')
  //         .fontSize(18)
  //         .fontWeight(FontWeight.Medium)
  //         .textAlign(TextAlign.Start)
  //         .width('100%')
  //         .padding({ left: 20, right: 20, top: 20, bottom: 10 })
  //
  //       Grid() {
  //         GridItem() {
  //           this.buildQuickEntry('提醒管理', '⏰', '#FF9800', () => {
  //             router.pushUrl({ url: 'pages/reminder/ReminderMain' });
  //           })
  //         }
  //
  //         GridItem() {
  //           this.buildQuickEntry('数据备份', '📦', '#607D8B', () => {
  //             router.pushUrl({ url: 'pages/backup/BackupMain' });
  //           })
  //         }
  //
  //         GridItem() {
  //           this.buildQuickEntry('标签管理', '🏷️', '#9C27B0', () => {
  //             router.pushUrl({ url: 'pages/tags/TagsMain' });
  //           })
  //         }
  //
  //         GridItem() {
  //           this.buildQuickEntry('数据筛选', '🔍', '#795548', () => {
  //             router.pushUrl({ url: 'pages/filter/FilterMain' });
  //           })
  //         }
  //
  //         GridItem() {
  //           this.buildQuickEntry('帮助中心', '❓', '#00BCD4', () => {
  //             router.pushUrl({ url: 'pages/help/HelpMain' });
  //           })
  //         }
  //       }
  //       .columnsTemplate('1fr 1fr 1fr')
  //       .rowsTemplate('1fr 1fr')
  //       .columnsGap(10)
  //       .rowsGap(10)
  //       .width('100%')
  //       .height(200)
  //       .padding({ left: 15, right: 15, bottom: 20 })
  //     }
  //   }
  //   .width('100%')
  //   .layoutWeight(1)
  // }

  build() {
    Scroll() {
      Column() {
        // 标题
        Row() {
          Text('生活中心')
            .fontSize(28)
            .fontWeight(FontWeight.Bold)

          Blank()
        }
        .width('100%')
        .padding({
          left: 20,
          right: 20,
          top: 20,
          bottom: 10
        })

        // ===== 删除：今日概览整个区域 =====


        // 快速入口（保留）
        Text('快速入口')
          .fontSize(18)
          .fontWeight(FontWeight.Medium)
          .textAlign(TextAlign.Start)
          .width('100%')
          .padding({
            left: 20,
            right: 20,
            top: 20,
            bottom: 10
          })

        Grid() {
          GridItem() {
            this.buildQuickEntry('记录健康', '💚', '#4CAF50', () => {
              router.pushUrl({ url: 'pages/health/HealthRecord' });
            })
          }

          GridItem() {
            this.buildQuickEntry('创建目标', '🎯', '#FF9800', () => {
              router.pushUrl({ url: 'pages/goals/GoalDetail' });
            })
          }

          GridItem() {
            this.buildQuickEntry('记录心情', '😊', '#E91E63', () => {
              router.pushUrl({ url: 'pages/mood/MoodRecord' });
            })
          }

          GridItem() {
            this.buildQuickEntry('规划旅行', '✈️', '#2196F3', () => {
              router.pushUrl({ url: 'pages/travel/TravelPlan' });
            })
          }

          GridItem() {
            this.buildQuickEntry('添加技能', '📚', '#9C27B0', () => {
              router.pushUrl({ url: 'pages/learning/SkillDetail' });
            })
          }

          GridItem() {
            this.buildQuickEntry('写日记', '📝', '#00BCD4', () => {
              router.pushUrl({ url: 'pages/diary/DiaryEdit' });
            })
          }
        }
        .columnsTemplate('1fr 1fr 1fr')
        .rowsTemplate('1fr 1fr')
        .columnsGap(10)
        .rowsGap(10)
        .width('100%')
        .height(200)
        .padding({ left: 15, right: 15, bottom: 20 })

        // 更多功能（删除 统一搜索、设置）
        Text('更多功能')
          .fontSize(18)
          .fontWeight(FontWeight.Medium)
          .textAlign(TextAlign.Start)
          .width('100%')
          .padding({
            left: 20,
            right: 20,
            top: 20,
            bottom: 10
          })

        Grid() {
          GridItem() {
            this.buildQuickEntry('数据统计', '📊', '#607D8B', () => {
              router.pushUrl({ url: 'pages/statistics/StatisticsMain' });
            })
          }

          GridItem() {
            this.buildQuickEntry('成就系统', '🏆', '#FFD700', () => {
              router.pushUrl({ url: 'pages/achievements/AchievementsMain' });
            })
          }

          GridItem() {
            this.buildQuickEntry('生活报告', '📄', '#9E9E9E', () => {
              router.pushUrl({ url: 'pages/report/ReportMain' });
            })
          }
        }
        .columnsTemplate('1fr 1fr 1fr')
        .rowsTemplate('1fr')
        .columnsGap(10)
        .rowsGap(10)
        .width('100%')
        .height(100)
        .padding({ left: 15, right: 15, bottom: 20 })

        // 工具功能（删除 数据备份、标签管理）
        Text('工具功能')
          .fontSize(18)
          .fontWeight(FontWeight.Medium)
          .textAlign(TextAlign.Start)
          .width('100%')
          .padding({
            left: 20,
            right: 20,
            top: 20,
            bottom: 10
          })

        Grid() {
          GridItem() {
            this.buildQuickEntry('提醒管理', '⏰', '#FF9800', () => {
              router.pushUrl({ url: 'pages/reminder/ReminderMain' });
            })
          }

          GridItem() {
            this.buildQuickEntry('数据筛选', '🔍', '#795548', () => {
              router.pushUrl({ url: 'pages/filter/FilterMain' });
            })
          }

          GridItem() {
            this.buildQuickEntry('帮助中心', '❓', '#00BCD4', () => {
              router.pushUrl({ url: 'pages/help/HelpMain' });
            })
          }
        }
        .columnsTemplate('1fr 1fr 1fr')
        .rowsTemplate('1fr')
        .columnsGap(10)
        .rowsGap(10)
        .width('100%')
        .height(100)
        .padding({ left: 15, right: 15, bottom: 20 })
      }
    }
    .width('100%')
    .layoutWeight(1)
  }

  @Builder
  buildStatCard(title: string, icon: string, value: string, subtitle: string, color: string, onClick: () => void) {
    Column() {
      Text(icon)
        .fontSize(32)
        .margin({ bottom: 8 })

      Text(value || '0')
        .fontSize(24)
        .fontWeight(FontWeight.Bold)
        .fontColor(color)
        .margin({ bottom: 4 })

      Text(title)
        .fontSize(14)
        .fontColor('#666')
        .margin({ bottom: 2 })

      Text(subtitle)
        .fontSize(12)
        .fontColor('#999')
    }
    .width('48%')
    .height(140)
    .padding(15)
    .backgroundColor(Color.White)
    .borderRadius(12)
    .shadow({
      radius: 4,
      color: '#10000000',
      offsetX: 0,
      offsetY: 2
    })
    .onClick(onClick)
  }

  @Builder
  buildQuickEntry(title: string, icon: string, color: string, onClick: () => void) {
    Column() {
      Text(icon)
        .fontSize(36)
        .margin({ bottom: 8 })

      Text(title)
        .fontSize(14)
        .fontColor('#333')
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
    .backgroundColor(Color.White)
    .borderRadius(12)
    .shadow({
      radius: 2,
      color: '#10000000',
      offsetX: 0,
      offsetY: 1
    })
    .onClick(onClick)
  }
}
