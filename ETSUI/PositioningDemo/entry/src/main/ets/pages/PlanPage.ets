/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-20
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import router from '@ohos.router';
import { PlanService } from '../service/PlanService';
import { HistoryService } from '../service/HistoryService';
import { Plan, PlanStatus } from '../model/PlanModel';
import { common } from '@kit.AbilityKit';
import promptAction from '@ohos.promptAction';

@Entry
@Component
struct PlanPage {
  @State plans: Plan[] = [];
  @State totalPlans: number = 0;
  @State activePlans: number = 0;
  @State completedPlans: number = 0;
  @State averageCompletionRate: number = 0;
  @State isLoaded: boolean = false;

  private historyService: HistoryService = new HistoryService();
  private planService: PlanService = new PlanService(this.historyService);

  async aboutToAppear() {
    const context = getContext(this) as common.UIAbilityContext;
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
    this.plans = await this.planService.getAllPlans();
    const statistics = await this.planService.getPlanStatistics();
    this.totalPlans = statistics.totalPlans;
    this.activePlans = statistics.activePlans;
    this.completedPlans = statistics.completedPlans;
    this.averageCompletionRate = statistics.averageCompletionRate;

    // 按状态和创建时间排序：进行中的在前，然后按创建时间倒序
    this.plans.sort((a, b) => {
      if (a.status === PlanStatus.ACTIVE && b.status !== PlanStatus.ACTIVE) return -1;
      if (a.status !== PlanStatus.ACTIVE && b.status === PlanStatus.ACTIVE) return 1;
      return b.createdTime - a.createdTime;
    });
  }

  build() {
    Column() {
      this.TopNavBar()
      
      this.StatisticsHeader()
      
      if (this.plans.length === 0) {
        Column() {
          Text('📋')
            .fontSize(64)
            .margin({ bottom: 16 })
          Text('暂无训练计划')
            .fontSize(18)
            .fontColor('#999999')
            .margin({ bottom: 8 })
          Text('点击右上角创建新计划')
            .fontSize(14)
            .fontColor('#CCCCCC')
        }
        .width('100%')
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
      } else {
        List({ space: 12 }) {
          ForEach(this.plans, (plan: Plan, index: number) => {
            ListItem() {
              this.PlanItem(plan, index)
            }
            .margin({ left: 16, right: 16 })
            .onClick(() => {
              router.pushUrl({
                url: 'pages/PlanDetailPage',
                params: { planId: plan.id }
              });
            })
          }, (plan: Plan, index: number) => `${plan.id}_${index}`)
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
      Text('📅 训练计划')
        .fontSize(24)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')

      Blank()

      Text('创建')
        .fontSize(16)
        .fontColor('#4ECDC4')
        .onClick(() => {
          router.pushUrl({
            url: 'pages/PlanDetailPage',
            params: { planId: '' }
          });
        })
    }
    .width('100%')
    .padding({ left: 16, right: 16, top: 16, bottom: 16 })
    .backgroundColor('#FFFFFF')
  }

  @Builder
  StatisticsHeader() {
    Column() {
      Row() {
        Column() {
          Text(`${this.totalPlans}`)
            .fontSize(32)
            .fontWeight(FontWeight.Bold)
            .fontColor('#4ECDC4')
            .margin({ bottom: 4 })
          Text('总计划')
            .fontSize(14)
            .fontColor('#999999')
        }
        .alignItems(HorizontalAlign.Center)
        .layoutWeight(1)

        Column() {
          Text(`${this.activePlans}`)
            .fontSize(32)
            .fontWeight(FontWeight.Bold)
            .fontColor('#FF6B6B')
            .margin({ bottom: 4 })
          Text('进行中')
            .fontSize(14)
            .fontColor('#999999')
        }
        .alignItems(HorizontalAlign.Center)
        .layoutWeight(1)

        Column() {
          Text(`${this.completedPlans}`)
            .fontSize(32)
            .fontWeight(FontWeight.Bold)
            .fontColor('#4CAF50')
            .margin({ bottom: 4 })
          Text('已完成')
            .fontSize(14)
            .fontColor('#999999')
        }
        .alignItems(HorizontalAlign.Center)
        .layoutWeight(1)

        Column() {
          Text(`${this.averageCompletionRate}%`)
            .fontSize(32)
            .fontWeight(FontWeight.Bold)
            .fontColor('#FFA502')
            .margin({ bottom: 4 })
          Text('平均完成度')
            .fontSize(14)
            .fontColor('#999999')
        }
        .alignItems(HorizontalAlign.Center)
        .layoutWeight(1)
      }
      .width('100%')
      .padding({ top: 20, bottom: 20 })
    }
    .width('100%')
    .padding({ left: 16, right: 16, top: 16, bottom: 16 })
    .backgroundColor('#FFFFFF')
    .margin({ bottom: 12 })
  }

  @Builder
  PlanItem(plan: Plan, index: number) {
    Column() {
      Row() {
        Column() {
          Text(plan.name)
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333333')
            .margin({ bottom: 6 })
          
          Text(plan.description || plan.goal)
            .fontSize(14)
            .fontColor('#666666')
            .maxLines(2)
            .textOverflow({ overflow: TextOverflow.Ellipsis })
            .margin({ bottom: 8 })

          Row() {
            Text(this.getStatusText(plan.status))
              .fontSize(12)
              .fontColor(this.getStatusColor(plan.status))
              .padding({ left: 8, right: 8, top: 4, bottom: 4 })
              .backgroundColor(this.getStatusBgColor(plan.status))
              .borderRadius(12)
            
            Text(`${plan.completedDays}/${plan.totalDays} 天`)
              .fontSize(12)
              .fontColor('#999999')
              .margin({ left: 12 })
          }
          .width('100%')
          .margin({ bottom: 8 })

          // 进度条
          Stack() {
            Progress({
              value: plan.completionRate,
              total: 100,
              type: ProgressType.Linear
            })
            .width('100%')
            .height(6)
            .color('#4ECDC4')
            .backgroundColor('#E0E0E0')
          }
          .width('100%')
          .height(6)
        }
        .alignItems(HorizontalAlign.Start)
        .layoutWeight(1)
      }
      .width('100%')
    }
    .width('100%')
    .padding(16)
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
    .shadow({ radius: 8, color: '#1A000000', offsetX: 0, offsetY: 2 })
  }

  getStatusText(status: PlanStatus): string {
    switch (status) {
      case PlanStatus.ACTIVE:
        return '进行中';
      case PlanStatus.COMPLETED:
        return '已完成';
      case PlanStatus.PAUSED:
        return '已暂停';
      case PlanStatus.CANCELLED:
        return '已取消';
      default:
        return '未知';
    }
  }

  getStatusColor(status: PlanStatus): string {
    switch (status) {
      case PlanStatus.ACTIVE:
        return '#4ECDC4';
      case PlanStatus.COMPLETED:
        return '#4CAF50';
      case PlanStatus.PAUSED:
        return '#FFA502';
      case PlanStatus.CANCELLED:
        return '#999999';
      default:
        return '#666666';
    }
  }

  getStatusBgColor(status: PlanStatus): string {
    switch (status) {
      case PlanStatus.ACTIVE:
        return '#4ECDC420';
      case PlanStatus.COMPLETED:
        return '#4CAF5020';
      case PlanStatus.PAUSED:
        return '#FFA50220';
      case PlanStatus.CANCELLED:
        return '#99999920';
      default:
        return '#66666620';
    }
  }
}
