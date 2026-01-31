/*
 * Copyright (c) 2023 Your Company Name
 * All rights reserved.
 */
import router from '@ohos.router';
import { StatisticsService } from '../service/StatisticsService';
import { HistoryService } from '../service/HistoryService';
import { WeeklyStatistics, DailyRecord } from '../model/UserModel';
import { SportData } from '../model/SportModel';
import { common } from '@kit.AbilityKit';

@Entry
@Component
struct StatisticsPage {
  // 将周统计的关键属性展开为独立的 @State
  @State weekLabel: string = '';
  @State totalRecords: number = 0;
  @State totalDistance: number = 0;
  @State totalDuration: number = 0;
  @State totalCalories: number = 0;
  @State dailyRecords: DailyRecord[] = [];

  @State recentTrend: DailyRecord[] = [];
  @State topDistanceRecords: SportData[] = [];

  private historyService: HistoryService = new HistoryService();
  private statisticsService: StatisticsService = new StatisticsService(this.historyService);

  async aboutToAppear() {
    const context = getContext(this) as common.UIAbilityContext;
    await this.historyService.init(context);
    await this.loadStatistics();
  }

  async loadStatistics() {
    const allRecords = await this.historyService.getSportRecords();

    // 计算本周数据
    const now = new Date();
    const dayOfWeek = now.getDay();
    const mondayOffset = dayOfWeek === 0 ? -6 : 1 - dayOfWeek;
    const monday = new Date(now);
    monday.setDate(now.getDate() + mondayOffset);
    monday.setHours(0, 0, 0, 0);
    const sunday = new Date(monday);
    sunday.setDate(monday.getDate() + 6);
    sunday.setHours(23, 59, 59, 999);

    const year = monday.getFullYear();
    const weekNumber = Math.ceil((monday.getTime() - new Date(year, 0, 1).getTime()) / (7 * 24 * 60 * 60 * 1000));
    this.weekLabel = `${year}年第${weekNumber}周`;

    // 筛选本周记录
    const weekRecords = allRecords.filter(record => {
      return record.startTime >= monday.getTime() && record.startTime <= sunday.getTime();
    });

    this.totalRecords = weekRecords.length;
    this.totalDistance = weekRecords.reduce((sum, r) => sum + r.distance, 0);
    this.totalDuration = weekRecords.reduce((sum, r) => sum + r.duration, 0);
    this.totalCalories = weekRecords.reduce((sum, r) => sum + r.calories, 0);

    // 初始化7天记录
    this.dailyRecords = [];
    for (let i = 0; i < 7; i++) {
      const day = new Date(monday);
      day.setDate(monday.getDate() + i);
      const dailyRecord = new DailyRecord();
      dailyRecord.date = day;
      const labels = ['周一', '周二', '周三', '周四', '周五', '周六', '周日'];
      dailyRecord.dayLabel = labels[i];
      dailyRecord.records = 0;
      dailyRecord.distance = 0;
      dailyRecord.duration = 0;
      dailyRecord.calories = 0;
      this.dailyRecords.push(dailyRecord);
    }

    // 分配记录到每天
    weekRecords.forEach(record => {
      const recordDate = new Date(record.startTime);
      const dayIndex = Math.floor((recordDate.getTime() - monday.getTime()) / (1000 * 60 * 60 * 24));
      if (dayIndex >= 0 && dayIndex < 7) {
        this.dailyRecords[dayIndex].records++;
        this.dailyRecords[dayIndex].distance += record.distance;
        this.dailyRecords[dayIndex].duration += record.duration;
        this.dailyRecords[dayIndex].calories += record.calories;
      }
    });

    this.recentTrend = await this.statisticsService.calculateRecentTrend(7);
    this.topDistanceRecords = await this.statisticsService.getTopDistanceRecords(3);
  }

  build() {
    Column() {
      this.TopNavBar()
      Scroll() {
        Column() {
          this.WeeklyOverviewCard()
          this.DailyDetailsCard()
          this.RecentTrendCard()
          this.TopRecordsCard()
        }
        .padding(16)
      }
      .layoutWeight(1)
      .backgroundColor('#F5F5F5')
    }
    .width('100%')
    .height('100%')
  }

  @Builder
  TopNavBar() {
    Row() {
      Text('< 返回')
        .fontSize(16)
        .fontColor('#4ECDC4')
        .onClick(() => {
          router.back();
        })
      Blank()
      Text('本周统计')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
      Blank()
      Text('     ')
    }
    .width('100%')
    .padding(16)
    .backgroundColor('#FFFFFF')
  }

  @Builder
  WeeklyOverviewCard() {
    Column() {
      Text(this.weekLabel)
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .width('100%')
        .margin({ bottom: 16 })

      Row() {
        // 运动次数 - 直接引用@State变量
        Column() {
          Text(this.totalRecords + ' 次')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
            .fontColor('#FF6B6B')
            .margin({ bottom: 6 })
          Text('运动次数')
            .fontSize(12)
            .fontColor('#999999')
        }
        .width('48%')
        .padding(16)
        .backgroundColor('#F8F9FA')
        .borderRadius(12)

        // 总距离 - 直接引用@State变量
        Column() {
          Text(this.totalDistance >= 1000 ?
            `${(this.totalDistance / 1000).toFixed(2)} km` :
            `${this.totalDistance.toFixed(0)} m`)
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
            .fontColor('#4ECDC4')
            .margin({ bottom: 6 })
          Text('总距离')
            .fontSize(12)
            .fontColor('#999999')
        }
        .width('48%')
        .padding(16)
        .backgroundColor('#F8F9FA')
        .borderRadius(12)
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceBetween)
      .margin({ bottom: 12 })

      Row() {
        // 总时长 - 直接引用@State变量
        Column() {
          Text((() => {
            const hours = Math.floor(this.totalDuration / 3600000);
            const minutes = Math.floor((this.totalDuration % 3600000) / 60000);
            const seconds = Math.floor((this.totalDuration % 60000) / 1000);
            if (hours > 0) {
              return `${hours}小时${minutes}分钟`;
            } else if (minutes > 0) {
              return `${minutes}分钟${seconds}秒`;
            } else {
              return `${seconds}秒`;
            }
          })())
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
            .fontColor('#FFA502')
            .margin({ bottom: 6 })
          Text('总时长')
            .fontSize(12)
            .fontColor('#999999')
        }
        .width('48%')
        .padding(16)
        .backgroundColor('#F8F9FA')
        .borderRadius(12)

        // 总卡路里 - 直接引用@State变量
        Column() {
          Text(this.totalCalories.toFixed(0) + ' kcal')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
            .fontColor('#9B59B6')
            .margin({ bottom: 6 })
          Text('总卡路里')
            .fontSize(12)
            .fontColor('#999999')
        }
        .width('48%')
        .padding(16)
        .backgroundColor('#F8F9FA')
        .borderRadius(12)
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceBetween)
    }
    .width('100%')
    .padding(20)
    .backgroundColor('#FFFFFF')
    .borderRadius(16)
    .margin({ bottom: 16 })
    .shadow({ radius: 8, color: '#10000000', offsetX: 0, offsetY: 2 })
  }

  @Builder
  StatCard(label: string, value: string, color: string) {
    Column() {
      Text(value)
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .fontColor(color)
        .margin({ bottom: 6 })
      Text(label)
        .fontSize(12)
        .fontColor('#999999')
    }
    .width('48%')
    .padding(16)
    .backgroundColor('#F8F9FA')
    .borderRadius(12)
  }

  @Builder
  DailyDetailsCard() {
    Column() {
      Text('每日详情')
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .width('100%')
        .margin({ bottom: 16 })

      ForEach(this.dailyRecords, (daily: DailyRecord) => {
        this.DailyItem(daily)
      })
    }
    .width('100%')
    .padding(20)
    .backgroundColor('#FFFFFF')
    .borderRadius(16)
    .margin({ bottom: 16 })
    .shadow({ radius: 8, color: '#10000000', offsetX: 0, offsetY: 2 })
  }

  @Builder
  DailyItem(daily: DailyRecord) {
    Row() {
      Column() {
        Text(daily.dayLabel)
          .fontSize(14)
          .fontColor('#666666')
          .margin({ bottom: 4 })
        Text(this.formatDateShort(daily.date))
          .fontSize(12)
          .fontColor('#999999')
      }
      .width('20%')
      .alignItems(HorizontalAlign.Start)

      Column() {
        if (daily.records > 0) {
          Row() {
            Text(`${daily.records}次`)
              .fontSize(12)
              .fontColor('#666666')
              .margin({ right: 12 })
            Text(this.formatDistance(daily.distance))
              .fontSize(12)
              .fontColor('#4ECDC4')
              .fontWeight(FontWeight.Bold)
          }
        } else {
          Text('未运动')
            .fontSize(12)
            .fontColor('#CCCCCC')
        }
      }
      .layoutWeight(1)
      .alignItems(HorizontalAlign.Start)

      Column() {
        if (daily.records > 0) {
          Row()
            .width(this.calculateDailyProgress(daily.distance) + '%')
            .height(6)
            .backgroundColor('#4ECDC4')
            .borderRadius(3)
        }
      }
      .width('30%')
      .alignItems(HorizontalAlign.End)
    }
    .width('100%')
    .padding({ top: 10, bottom: 10 })
    .borderRadius(8)
    .backgroundColor(daily.records > 0 ? '#F8F9FA' : 'transparent')
    .margin({ bottom: 6 })
  }

  @Builder
  RecentTrendCard() {
    Column() {
      Text('最近7天趋势')
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .width('100%')
        .margin({ bottom: 16 })

      ForEach(this.recentTrend, (daily: DailyRecord) => {
        Row() {
          Text(this.formatDateShort(daily.date))
            .fontSize(14)
            .fontColor('#666666')
            .width('20%')

          Blank()

          if (daily.distance > 0) {
            Text(this.formatDistance(daily.distance))
              .fontSize(14)
              .fontWeight(FontWeight.Bold)
              .fontColor('#4ECDC4')
          } else {
            Text('休息')
              .fontSize(14)
              .fontColor('#CCCCCC')
          }
        }
        .width('100%')
        .padding({ top: 10, bottom: 10 })
        .backgroundColor('#F8F9FA')
        .borderRadius(8)
        .margin({ bottom: 6 })
      })
    }
    .width('100%')
    .padding(20)
    .backgroundColor('#FFFFFF')
    .borderRadius(16)
    .margin({ bottom: 16 })
    .shadow({ radius: 8, color: '#10000000', offsetX: 0, offsetY: 2 })
  }

  @Builder
  TopRecordsCard() {
    Column() {
      Text('距离之最 TOP3')
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .width('100%')
        .margin({ bottom: 16 })

      if (this.topDistanceRecords.length === 0) {
        Column() {
          Text('暂无记录')
            .fontSize(14)
            .fontColor('#999999')
        }
        .width('100%')
        .height(100)
        .justifyContent(FlexAlign.Center)
      } else {
        ForEach(this.topDistanceRecords, (record: SportData, index: number) => {
          this.TopRecordItem(record, index + 1)
        })
      }
    }
    .width('100%')
    .padding(20)
    .backgroundColor('#FFFFFF')
    .borderRadius(16)
    .margin({ bottom: 16 })
    .shadow({ radius: 8, color: '#10000000', offsetX: 0, offsetY: 2 })
  }

  @Builder
  TopRecordItem(record: SportData, rank: number) {
    Row() {
      Text(rank.toString())
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .fontColor('#FFFFFF')
        .width(32)
        .height(32)
        .textAlign(TextAlign.Center)
        .backgroundColor(this.getRankColor(rank))
        .borderRadius(16)
        .margin({ right: 12 })

      Column() {
        Text(this.getSportTypeName(record.type))
          .fontSize(14)
          .fontColor('#666666')
          .margin({ bottom: 4 })
        Text(this.formatDate(record.startTime))
          .fontSize(11)
          .fontColor('#999999')
      }
      .alignItems(HorizontalAlign.Start)
      .layoutWeight(1)

      Text(this.formatDistance(record.distance))
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .fontColor('#4ECDC4')
    }
    .width('100%')
    .padding(12)
    .backgroundColor('#F8F9FA')
    .borderRadius(12)
    .margin({ bottom: 8 })
  }

  calculateDailyProgress(distance: number): number {
    const maxDistance = Math.max(...this.dailyRecords.map(d => d.distance));
    if (maxDistance === 0) return 0;
    return Math.round((distance / maxDistance) * 100);
  }

  getRankColor(rank: number): string {
    if (rank === 1) return '#FFD700';
    if (rank === 2) return '#C0C0C0';
    if (rank === 3) return '#CD7F32';
    return '#95A5A6';
  }

  getSportTypeName(type: string): string {
    switch (type) {
      case 'running':
        return '跑步';
      case 'walking':
        return '行走';
      case 'cycling':
        return '骑行';
      default:
        return '运动';
    }
  }

  formatDistance(distance: number): string {
    if (distance >= 1000) {
      return `${(distance / 1000).toFixed(2)} km`;
    }
    return `${distance.toFixed(0)} m`;
  }

  formatDuration(duration: number): string {
    const hours = Math.floor(duration / 3600000);
    const minutes = Math.floor((duration % 3600000) / 60000);
    if (hours > 0) {
      return `${hours}小时${minutes}分钟`;
    }
    return `${minutes}分钟`;
  }

  formatDateShort(date: Date): string {
    const month = date.getMonth() + 1;
    const day = date.getDate();
    return `${month}/${day}`;
  }

  formatDate(timestamp: number): string {
    const date = new Date(timestamp);
    const month = date.getMonth() + 1;
    const day = date.getDate();
    return `${month}月${day}日`;
  }
}
