// Copyright 2023 YourCompany. All rights reserved.
import router from '@ohos.router';
import { UserService } from '../service/UserService';
import { StatisticsService } from '../service/StatisticsService';
import { HistoryService } from '../service/HistoryService';
import { PlanService } from '../service/PlanService';
import { UserInfo, UserStatistics } from '../model/UserModel';
import { common } from '@kit.AbilityKit';

@Entry
@Component
struct ProfilePage {
  @State userNickname: string = '运动爱好者';
  @State userSignature: string = '坚持运动，享受健康';
  @State userGender: string = '未设置';
  @State userAge: number = 0;
  @State userHeight: number = 0;
  @State userWeight: number = 0;

  @State totalRecords: number = 0;
  @State totalDistance: number = 0;
  @State totalDuration: number = 0;
  @State totalCalories: number = 0;
  @State runningCount: number = 0;
  @State runningDistance: number = 0;
  @State walkingCount: number = 0;
  @State walkingDistance: number = 0;
  @State cyclingCount: number = 0;
  @State cyclingDistance: number = 0;

  @State activityDays: number = 0;
  @State weeklyFrequency: number = 0;
  @State isLoaded: boolean = false;

  @State totalPlans: number = 0;
  @State activePlans: number = 0;
  @State completedPlans: number = 0;
  @State averageCompletionRate: number = 0;

  private userService: UserService = new UserService();
  private historyService: HistoryService = new HistoryService();
  private statisticsService: StatisticsService = new StatisticsService(this.historyService);
  private planService: PlanService = new PlanService(this.historyService);

  async aboutToAppear() {
    const context = getContext(this) as common.UIAbilityContext;
    await this.userService.init(context);
    await this.historyService.init(context);
    await this.planService.init(context);
    this.isLoaded = true;
    await this.loadData();
  }

  onPageShow() {
    if (this.isLoaded) {
      this.loadData();
    }
  }

  async loadData() {
    const userInfo = await this.userService.getUserInfo();

    this.userNickname = userInfo.nickname;
    this.userSignature = userInfo.signature;
    this.userGender = userInfo.gender;
    this.userAge = userInfo.age;
    this.userHeight = userInfo.height;
    this.userWeight = userInfo.weight;

    await this.loadStatistics();
  }

  async loadStatistics() {
    const allRecords = await this.historyService.getSportRecords();

    // 手动计算统计数据，直接赋值给 @State 变量
    this.totalRecords = allRecords.length;
    this.totalDistance = allRecords.reduce((sum, r) => sum + r.distance, 0);
    this.totalDuration = allRecords.reduce((sum, r) => sum + r.duration, 0);
    this.totalCalories = allRecords.reduce((sum, r) => sum + r.calories, 0);

    // 按类型统计
    this.runningCount = 0;
    this.runningDistance = 0;
    this.walkingCount = 0;
    this.walkingDistance = 0;
    this.cyclingCount = 0;
    this.cyclingDistance = 0;

    allRecords.forEach(record => {
      if (record.type === 'running') {
        this.runningCount++;
        this.runningDistance += record.distance;
      } else if (record.type === 'walking') {
        this.walkingCount++;
        this.walkingDistance += record.distance;
      } else if (record.type === 'cycling') {
        this.cyclingCount++;
        this.cyclingDistance += record.distance;
      }
    });

    this.activityDays = await this.statisticsService.calculateActivityDays(30);
    this.weeklyFrequency = await this.statisticsService.calculateWeeklyFrequency();

    // 加载训练计划统计
    const planStatistics = await this.planService.getPlanStatistics();
    this.totalPlans = planStatistics.totalPlans;
    this.activePlans = planStatistics.activePlans;
    this.completedPlans = planStatistics.completedPlans;
    this.averageCompletionRate = planStatistics.averageCompletionRate;
  }

  build() {
    Column() {
      this.TopNavBar()
      Scroll() {
        Column() {
          this.UserInfoCard()
          this.OverviewCard()
          this.SportTypeCard()
          this.PlanCard()
          this.SettingsCard()
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
      Text('个人中心')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
      Blank()
      Text('编辑')
        .fontSize(16)
        .fontColor('#4ECDC4')
        .onClick(() => {
          router.pushUrl({ url: 'pages/PersonalInfoPage' });
        })
    }
    .width('100%')
    .padding(16)
    .backgroundColor('#FFFFFF')
  }

  @Builder
  UserInfoCard() {
    Column() {
      Row() {
        Image($r('app.media.startIcon'))
          .width(70)
          .height(70)
          .borderRadius(35)
          .margin({ right: 16 })
        Column() {
          Text(this.userNickname)
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333333')
            .margin({ bottom: 6 })
          Text(this.userSignature)
            .fontSize(14)
            .fontColor('#999999')
        }
        .alignItems(HorizontalAlign.Start)
        .layoutWeight(1)
      }
      .width('100%')

      Row() {
        // 性别 - 直接引用@State变量
        Column() {
          Text(this.userGender)
            .fontSize(14)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333333')
            .margin({ bottom: 4 })
          Text('性别')
            .fontSize(12)
            .fontColor('#999999')
        }

        // 年龄 - 直接引用@State变量
        Column() {
          Text(this.userAge > 0 ? this.userAge + '岁' : '未设置')
            .fontSize(14)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333333')
            .margin({ bottom: 4 })
          Text('年龄')
            .fontSize(12)
            .fontColor('#999999')
        }

        // 身高 - 直接引用@State变量
        Column() {
          Text(this.userHeight > 0 ? this.userHeight + 'cm' : '未设置')
            .fontSize(14)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333333')
            .margin({ bottom: 4 })
          Text('身高')
            .fontSize(12)
            .fontColor('#999999')
        }

        // 体重 - 直接引用@State变量
        Column() {
          Text(this.userWeight > 0 ? this.userWeight + 'kg' : '未设置')
            .fontSize(14)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333333')
            .margin({ bottom: 4 })
          Text('体重')
            .fontSize(12)
            .fontColor('#999999')
        }
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceBetween)
      .margin({ top: 20 })
    }
    .width('100%')
    .padding(20)
    .backgroundColor('#FFFFFF')
    .borderRadius(16)
    .margin({ bottom: 16 })
    .shadow({ radius: 8, color: '#10000000', offsetX: 0, offsetY: 2 })
  }

  @Builder
  InfoItem(label: string, value: string) {
    Column() {
      Text(value)
        .fontSize(14)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .margin({ bottom: 4 })
      Text(label)
        .fontSize(12)
        .fontColor('#999999')
    }
  }

  @Builder
  OverviewCard() {
    Column() {
      Row() {
        Text('运动总览')
          .fontSize(16)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')
        Blank()
        Text('查看详细统计 >')
          .fontSize(14)
          .fontColor('#4ECDC4')
          .onClick(() => {
            router.pushUrl({ url: 'pages/StatisticsPage' });
          })
      }
      .width('100%')
      .margin({ bottom: 16 })

      Row() {
        // 总距离 - 直接引用@State变量
        Column() {
          Row() {
            Text('📏')
              .fontSize(20)
              .margin({ right: 6 })
            Text(this.totalDistance >= 1000 ?
              `${(this.totalDistance / 1000).toFixed(2)} km` :
              `${this.totalDistance.toFixed(0)} m`)
              .fontSize(16)
              .fontWeight(FontWeight.Bold)
              .fontColor('#FF6B6B')
          }
          .margin({ bottom: 4 })
          Text('总距离')
            .fontSize(12)
            .fontColor('#999999')
        }
        .width('48%')
        .padding(14)
        .backgroundColor('#F8F9FA')
        .borderRadius(12)
        .alignItems(HorizontalAlign.Start)

        // 总时长 - 直接引用@State变量
        Column() {
          Row() {
            Text('⏱️')
              .fontSize(20)
              .margin({ right: 6 })
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
              .fontSize(16)
              .fontWeight(FontWeight.Bold)
              .fontColor('#4ECDC4')
          }
          .margin({ bottom: 4 })
          Text('总时长')
            .fontSize(12)
            .fontColor('#999999')
        }
        .width('48%')
        .padding(14)
        .backgroundColor('#F8F9FA')
        .borderRadius(12)
        .alignItems(HorizontalAlign.Start)
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceBetween)
      .margin({ bottom: 12 })

      Row() {
        // 总卡路里 - 直接引用@State变量
        Column() {
          Row() {
            Text('🔥')
              .fontSize(20)
              .margin({ right: 6 })
            Text(this.totalCalories.toFixed(0) + ' kcal')
              .fontSize(16)
              .fontWeight(FontWeight.Bold)
              .fontColor('#FFA502')
          }
          .margin({ bottom: 4 })
          Text('总卡路里')
            .fontSize(12)
            .fontColor('#999999')
        }
        .width('48%')
        .padding(14)
        .backgroundColor('#F8F9FA')
        .borderRadius(12)
        .alignItems(HorizontalAlign.Start)

        // 运动次数 - 直接引用@State变量
        Column() {
          Row() {
            Text('📊')
              .fontSize(20)
              .margin({ right: 6 })
            Text(this.totalRecords + ' 次')
              .fontSize(16)
              .fontWeight(FontWeight.Bold)
              .fontColor('#45B7D1')
          }
          .margin({ bottom: 4 })
          Text('运动次数')
            .fontSize(12)
            .fontColor('#999999')
        }
        .width('48%')
        .padding(14)
        .backgroundColor('#F8F9FA')
        .borderRadius(12)
        .alignItems(HorizontalAlign.Start)
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
  StatItem(label: string, value: string, color: string, icon: string) {
    Column() {
      Row() {
        Text(icon)
          .fontSize(20)
          .margin({ right: 6 })
        Text(value)
          .fontSize(16)
          .fontWeight(FontWeight.Bold)
          .fontColor(color)
      }
      .margin({ bottom: 4 })
      Text(label)
        .fontSize(12)
        .fontColor('#999999')
    }
    .width('48%')
    .padding(14)
    .backgroundColor('#F8F9FA')
    .borderRadius(12)
    .alignItems(HorizontalAlign.Start)
  }

  @Builder
  SportTypeCard() {
    Column() {
      Text('运动类型分布')
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .width('100%')
        .margin({ bottom: 16 })
      // 跑步 - 直接引用@State变量
      Row() {
        Text('🏃')
          .fontSize(24)
          .margin({ right: 12 })
        Column() {
          Text('跑步')
            .fontSize(14)
            .fontColor('#666666')
            .margin({ bottom: 4 })
          Text(this.runningCount + ' 次 · ' +
            (this.runningDistance >= 1000 ?
              `${(this.runningDistance / 1000).toFixed(2)} km` :
              `${this.runningDistance.toFixed(0)} m`))
            .fontSize(12)
            .fontColor('#999999')
        }
        .alignItems(HorizontalAlign.Start)
        Blank()
        Text((this.totalRecords === 0 ? 0 : Math.round((this.runningCount / this.totalRecords) * 100)) + '%')
          .fontSize(12)
          .fontWeight(FontWeight.Bold)
          .fontColor('#FF6B6B')
      }
      .width('100%')
      .padding({ top: 10, bottom: 10 })

      // 行走 - 直接引用@State变量
      Row() {
        Text('🚶')
          .fontSize(24)
          .margin({ right: 12 })
        Column() {
          Text('行走')
            .fontSize(14)
            .fontColor('#666666')
            .margin({ bottom: 4 })
          Text(this.walkingCount + ' 次 · ' +
            (this.walkingDistance >= 1000 ?
              `${(this.walkingDistance / 1000).toFixed(2)} km` :
              `${this.walkingDistance.toFixed(0)} m`))
            .fontSize(12)
            .fontColor('#999999')
        }
        .alignItems(HorizontalAlign.Start)
        Blank()
        Text((this.totalRecords === 0 ? 0 : Math.round((this.walkingCount / this.totalRecords) * 100)) + '%')
          .fontSize(12)
          .fontWeight(FontWeight.Bold)
          .fontColor('#4ECDC4')
      }
      .width('100%')
      .padding({ top: 10, bottom: 10 })

      // 骑行 - 直接引用@State变量
      Row() {
        Text('🚴')
          .fontSize(24)
          .margin({ right: 12 })
        Column() {
          Text('骑行')
            .fontSize(14)
            .fontColor('#666666')
            .margin({ bottom: 4 })
          Text(this.cyclingCount + ' 次 · ' +
            (this.cyclingDistance >= 1000 ?
              `${(this.cyclingDistance / 1000).toFixed(2)} km` :
              `${this.cyclingDistance.toFixed(0)} m`))
            .fontSize(12)
            .fontColor('#999999')
        }
        .alignItems(HorizontalAlign.Start)
        Blank()
        Text((this.totalRecords === 0 ? 0 : Math.round((this.cyclingCount / this.totalRecords) * 100)) + '%')
          .fontSize(12)
          .fontWeight(FontWeight.Bold)
          .fontColor('#FFA502')
      }
      .width('100%')
      .padding({ top: 10, bottom: 10 })

      Row() {
        Column() {
          Text(this.weeklyFrequency.toFixed(1))
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor('#9B59B6')
            .margin({ bottom: 4 })
          Text('每周平均次数')
            .fontSize(12)
            .fontColor('#999999')
        }
        .width('48%')
        .padding(12)
        .backgroundColor('#F8F9FA')
        .borderRadius(12)

        Column() {
          Text(this.activityDays + '')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor('#E74C3C')
            .margin({ bottom: 4 })
          Text('30天活跃天数')
            .fontSize(12)
            .fontColor('#999999')
        }
        .width('48%')
        .padding(12)
        .backgroundColor('#F8F9FA')
        .borderRadius(12)
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceBetween)
      .margin({ top: 12 })
    }
    .width('100%')
    .padding(20)
    .backgroundColor('#FFFFFF')
    .borderRadius(16)
    .margin({ bottom: 16 })
    .shadow({ radius: 8, color: '#10000000', offsetX: 0, offsetY: 2 })
  }

  @Builder
  SportTypeItem(icon: string, name: string, count: number, distance: number, color: string) {
    Row() {
      Text(icon)
        .fontSize(24)
        .margin({ right: 12 })
      Column() {
        Text(name)
          .fontSize(14)
          .fontColor('#666666')
          .margin({ bottom: 4 })
        Text(count + ' 次 · ' + this.formatDistance(distance))
          .fontSize(12)
          .fontColor('#999999')
      }
      .alignItems(HorizontalAlign.Start)
      Blank()
      Text(this.calculatePercentage(count) + '%')
        .fontSize(12)
        .fontWeight(FontWeight.Bold)
        .fontColor(color)
    }
    .width('100%')
    .padding({ top: 10, bottom: 10 })
  }

  @Builder
  PlanCard() {
    Column() {
      Row() {
        Text('训练计划')
          .fontSize(16)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')
        Blank()
        Text('查看全部 >')
          .fontSize(14)
          .fontColor('#4ECDC4')
          .onClick(() => {
            router.pushUrl({ url: 'pages/PlanPage' });
          })
      }
      .width('100%')
      .margin({ bottom: 16 })

      Row() {
        // 总计划数
        Column() {
          Row() {
            Text('📋')
              .fontSize(20)
              .margin({ right: 6 })
            Text(this.totalPlans + ' 个')
              .fontSize(16)
              .fontWeight(FontWeight.Bold)
              .fontColor('#4ECDC4')
          }
          .margin({ bottom: 4 })
          Text('总计划')
            .fontSize(12)
            .fontColor('#999999')
        }
        .width('48%')
        .padding(14)
        .backgroundColor('#F8F9FA')
        .borderRadius(12)
        .alignItems(HorizontalAlign.Start)

        // 进行中计划
        Column() {
          Row() {
            Text('🏃')
              .fontSize(20)
              .margin({ right: 6 })
            Text(this.activePlans + ' 个')
              .fontSize(16)
              .fontWeight(FontWeight.Bold)
              .fontColor('#FF6B6B')
          }
          .margin({ bottom: 4 })
          Text('进行中')
            .fontSize(12)
            .fontColor('#999999')
        }
        .width('48%')
        .padding(14)
        .backgroundColor('#F8F9FA')
        .borderRadius(12)
        .alignItems(HorizontalAlign.Start)
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceBetween)
      .margin({ bottom: 12 })

      Row() {
        // 已完成计划
        Column() {
          Row() {
            Text('✅')
              .fontSize(20)
              .margin({ right: 6 })
            Text(this.completedPlans + ' 个')
              .fontSize(16)
              .fontWeight(FontWeight.Bold)
              .fontColor('#4CAF50')
          }
          .margin({ bottom: 4 })
          Text('已完成')
            .fontSize(12)
            .fontColor('#999999')
        }
        .width('48%')
        .padding(14)
        .backgroundColor('#F8F9FA')
        .borderRadius(12)
        .alignItems(HorizontalAlign.Start)

        // 平均完成度
        Column() {
          Row() {
            Text('📊')
              .fontSize(20)
              .margin({ right: 6 })
            Text(this.averageCompletionRate + '%')
              .fontSize(16)
              .fontWeight(FontWeight.Bold)
              .fontColor('#FFA502')
          }
          .margin({ bottom: 4 })
          Text('平均完成度')
            .fontSize(12)
            .fontColor('#999999')
        }
        .width('48%')
        .padding(14)
        .backgroundColor('#F8F9FA')
        .borderRadius(12)
        .alignItems(HorizontalAlign.Start)
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
  SettingsCard() {
    Column() {
      this.SettingItem('🏠', '返回主页', () => {
        router.pushUrl({ url: 'pages/Index' });
      })
    }
    .width('100%')
    .padding(20)
    .backgroundColor('#FFFFFF')
    .borderRadius(16)
    .margin({ bottom: 24 })
    .shadow({ radius: 8, color: '#10000000', offsetX: 0, offsetY: 2 })
  }

  @Builder
  SettingItem(icon: string, label: string, onClick: () => void) {
    Row() {
      Text(icon)
        .fontSize(22)
        .margin({ right: 12 })
      Text(label)
        .fontSize(16)
        .fontColor('#333333')
      Blank()
      Text('>')
        .fontSize(16)
        .fontColor('#CCCCCC')
    }
    .width('100%')
    .padding({ top: 14, bottom: 14 })
    .onClick(onClick)
  }

  calculatePercentage(count: number): number {
    if (this.totalRecords === 0) return 0;
    return Math.round((count / this.totalRecords) * 100);
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
}
