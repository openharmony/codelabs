import router from '@ohos.router';
import { AchievementService } from '../service/AchievementService';
import { HistoryService } from '../service/HistoryService';
import { StatisticsService } from '../service/StatisticsService';
import { Achievement } from '../model/AchievementModel';
import { common } from '@kit.AbilityKit';

@Entry
@Component
struct AchievementPage {
  @State achievements: Achievement[] = [];
  @State unlockedCount: number = 0;
  @State totalCount: number = 0;
  @State isLoaded: boolean = false;

  private historyService: HistoryService = new HistoryService();
  private statisticsService: StatisticsService = new StatisticsService(this.historyService);
  private achievementService: AchievementService = new AchievementService(this.historyService, this.statisticsService);

  async aboutToAppear() {
    const context = getContext(this) as common.UIAbilityContext;
    await this.historyService.init(context);
    await this.achievementService.init(context);
    this.isLoaded = true;
    await this.loadAchievements();
  }

  onPageShow() {
    if (this.isLoaded) {
      this.loadAchievements();
    }
  }

  async loadAchievements() {
    this.achievements = await this.achievementService.getAllAchievements();
    // 从成就列表中统计已解锁数量，而不是单独调用getUnlockedCount
    // 因为getAllAchievements已经自动检查并解锁了符合条件的成就
    this.unlockedCount = this.achievements.filter(a => a.isUnlocked).length;
    this.totalCount = this.achievementService.getTotalCount();
    
    // 按解锁状态排序：已解锁在前，未解锁在后
    this.achievements.sort((a, b) => {
      if (a.isUnlocked && !b.isUnlocked) return -1;
      if (!a.isUnlocked && b.isUnlocked) return 1;
      return 0;
    });
  }

  build() {
    Column() {
      this.TopNavBar()
      
      this.ProgressHeader()
      
      if (this.achievements.length === 0) {
        Column() {
          Text('暂无成就')
            .fontSize(16)
            .fontColor('#999999')
        }
        .width('100%')
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
      } else {
        List({ space: 12 }) {
          ForEach(this.achievements, (achievement: Achievement, index: number) => {
            ListItem() {
              this.AchievementItem(achievement, index)
            }
            .margin({ left: 16, right: 16 })
          }, (achievement: Achievement, index: number) => `${achievement.id}_${index}`)
        }
        .width('100%')
        .layoutWeight(1)
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F8F9FA')
  }

  @Builder
  TopNavBar() {
    Row() {
      Text('🏆 成就系统')
        .fontSize(24)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')

      Blank()

      Text('返回')
        .fontSize(16)
        .fontColor('#4ECDC4')
        .onClick(() => {
          router.back();
        })
    }
    .width('100%')
    .padding({ left: 16, right: 16, top: 16, bottom: 16 })
    .backgroundColor('#FFFFFF')
  }

  @Builder
  ProgressHeader() {
    Column() {
      Row() {
        Column() {
          Text(`${this.unlockedCount}`)
            .fontSize(32)
            .fontWeight(FontWeight.Bold)
            .fontColor('#4ECDC4')
            .margin({ bottom: 4 })
          Text('已解锁')
            .fontSize(14)
            .fontColor('#999999')
        }
        .alignItems(HorizontalAlign.Center)
        .layoutWeight(1)

        Column() {
          Text(`${this.totalCount}`)
            .fontSize(32)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333333')
            .margin({ bottom: 4 })
          Text('总成就')
            .fontSize(14)
            .fontColor('#999999')
        }
        .alignItems(HorizontalAlign.Center)
        .layoutWeight(1)

        Column() {
          Text(`${Math.round((this.unlockedCount / this.totalCount) * 100)}%`)
            .fontSize(32)
            .fontWeight(FontWeight.Bold)
            .fontColor('#FFA502')
            .margin({ bottom: 4 })
          Text('完成度')
            .fontSize(14)
            .fontColor('#999999')
        }
        .alignItems(HorizontalAlign.Center)
        .layoutWeight(1)
      }
      .width('100%')
      .padding({ top: 20, bottom: 20 })

      // 进度条
      Stack() {
        Progress({
          value: this.unlockedCount,
          total: this.totalCount,
          type: ProgressType.Linear
        })
        .width('100%')
        .height(8)
        .color('#4ECDC4')
        .backgroundColor('#E0E0E0')
      }
      .width('100%')
      .height(8)
      .margin({ top: 12, bottom: 8 })
    }
    .width('100%')
    .padding({ left: 16, right: 16, top: 16, bottom: 16 })
    .backgroundColor('#FFFFFF')
    .margin({ bottom: 12 })
  }

  @Builder
  AchievementItem(achievement: Achievement, index: number) {
    Column() {
      Row() {
        // 成就图标
        Text(achievement.icon)
          .fontSize(48)
          .width(60)
          .height(60)
          .textAlign(TextAlign.Center)
          .opacity(achievement.isUnlocked ? 1 : 0.3)
          .margin({ right: 16 })

        Column() {
          // 成就名称和描述
          Row() {
            Text(achievement.name)
              .fontSize(18)
              .fontWeight(FontWeight.Bold)
              .fontColor(achievement.isUnlocked ? '#333333' : '#999999')
            
            if (achievement.isUnlocked) {
              Text('✓')
                .fontSize(16)
                .fontColor('#4ECDC4')
                .margin({ left: 8 })
            }
          }
          .width('100%')
          .margin({ bottom: 6 })

          Text(achievement.description)
            .fontSize(14)
            .fontColor(achievement.isUnlocked ? '#666666' : '#999999')
            .margin({ bottom: 8 })

          // 进度信息
          if (!achievement.isUnlocked) {
            Column() {
              Row() {
                Text('进度:')
                  .fontSize(12)
                  .fontColor('#999999')
                Text(this.formatProgress(achievement))
                  .fontSize(12)
                  .fontWeight(FontWeight.Bold)
                  .fontColor('#4ECDC4')
                  .margin({ left: 4 })
              }
              .width('100%')
              .margin({ bottom: 6 })

              // 进度条
              Stack() {
                Progress({
                  value: achievement.progress,
                  total: 100,
                  type: ProgressType.Linear
                })
                .width('100%')
                .height(4)
                .color('#4ECDC4')
                .backgroundColor('#E0E0E0')
              }
              .width('100%')
              .height(4)
            }
            .width('100%')
            .alignItems(HorizontalAlign.Start)
          } else {
            Row() {
              Text('解锁时间:')
                .fontSize(12)
                .fontColor('#999999')
              Text(this.formatUnlockedTime(achievement.unlockedTime))
                .fontSize(12)
                .fontColor('#4ECDC4')
                .margin({ left: 4 })
            }
            .width('100%')
          }
        }
        .alignItems(HorizontalAlign.Start)
        .layoutWeight(1)
      }
      .width('100%')
      .alignItems(VerticalAlign.Top)
    }
    .width('100%')
    .padding(16)
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
    .shadow({ radius: 8, color: '#1A000000', offsetX: 0, offsetY: 2 })
    .opacity(achievement.isUnlocked ? 1 : 0.8)
  }

  formatProgress(achievement: Achievement): string {
    const current = achievement.currentValue;
    const target = achievement.targetValue;

    switch (achievement.type) {
      case 'first_sport':
      case 'total_records_10':
      case 'total_records_50':
      case 'total_records_100':
      case 'consecutive_days_3':
      case 'consecutive_days_7':
      case 'consecutive_days_30':
        return `${current}/${target}`;

      case 'total_distance_1km':
      case 'total_distance_5km':
      case 'total_distance_10km':
      case 'total_distance_50km':
      case 'total_distance_100km':
      case 'single_distance_5km':
      case 'single_distance_10km':
        if (current >= 1000) {
          return `${(current / 1000).toFixed(2)}/${(target / 1000).toFixed(0)} km`;
        }
        return `${current.toFixed(0)}/${target.toFixed(0)} m`;

      case 'single_duration_30min':
      case 'single_duration_60min':
        const currentMin = Math.floor(current / 60000);
        const targetMin = Math.floor(target / 60000);
        return `${currentMin}/${targetMin} 分钟`;

      case 'total_calories_1000':
      case 'total_calories_5000':
        return `${current.toFixed(0)}/${target.toFixed(0)} kcal`;

      default:
        return `${current}/${target}`;
    }
  }

  formatUnlockedTime(timestamp: number): string {
    const date = new Date(timestamp);
    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, '0');
    const day = String(date.getDate()).padStart(2, '0');
    const hours = String(date.getHours()).padStart(2, '0');
    const minutes = String(date.getMinutes()).padStart(2, '0');
    return `${year}-${month}-${day} ${hours}:${minutes}`;
  }
}
