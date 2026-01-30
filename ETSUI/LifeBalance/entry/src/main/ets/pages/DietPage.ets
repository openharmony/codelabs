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
import { DietRecord, FoodItem, MealTime } from '../model/DietModel';
import { FoodSelectorDialog } from './Diet/FoodSelector';
import { WeeklyData, WeekTrendDialog } from './Diet/WeekTrendChart';
import promptAction from '@ohos.promptAction';
import { RdbManager } from '../common/database/RdbManager';
import { DietRecordDao } from '../common/database/DietRecordDao';
import { HealthDao } from '../common/database/HealthDao';
import common from '@ohos.app.ability.common';

@Component
export struct DietPage {
  @State dietRecords: DietRecord[] = [];
  @StorageProp('targetCalories') currentGlobalTarget: number = 2000;
  @State dailyTarget: number = 2000;
  @StorageProp('userAge') userAge: number = 25;
  @StorageProp('isMale') isMale: boolean = true;
  @State selectedDate: Date = new Date();
  @StorageProp('appFontScale') fontScale: number = 1.0;
  @State weeklyStats: WeeklyData[] = [];
  // 1. 【新增】用于控制弹窗默认选中的餐别
  @State currentSelectedMeal: MealTime = MealTime.SNACK;
  trendDialogController: CustomDialogController = new CustomDialogController({
    builder: WeekTrendDialog({
      weeklyData: this.weeklyStats,
      title: '近7天摄入趋势',
      unit: 'kcal'
    }),
    alignment: DialogAlignment.Center,
    customStyle: false,
    autoCancel: true
  });
  addDialogController: CustomDialogController = new CustomDialogController({
    builder: FoodSelectorDialog({
      // 2. 【修改】将状态变量传给弹窗，实现父子同步
      selectedMeal: $currentSelectedMeal,
      onConfirm: (item: FoodItem, count: number, meal: MealTime): Promise<void> => this.addRecord(item, count, meal)
    }),
    alignment: DialogAlignment.Bottom,
    offset: {
      dx: 0,
      dy: -20
    },
    customStyle: false,
    autoCancel: true
  });
  private dietDao: DietRecordDao = new DietRecordDao();
  private healthDao: HealthDao = new HealthDao();

  getTodayStr(): string {
    const now = new Date();
    const y = now.getFullYear();
    const m = (now.getMonth() + 1).toString().padStart(2, '0');
    const d = now.getDate().toString().padStart(2, '0');
    return `${y}-${m}-${d}`;
  }

  // 3. 【新增】原 FoodSelectorDialog 里的猜测逻辑移到这里
  guessMealTime(): MealTime {
    const hour = new Date().getHours();
    if (hour >= 5 && hour < 10) {
      return MealTime.BREAKFAST;
    }
    if (hour >= 10 && hour < 15) {
      return MealTime.LUNCH;
    }
    if (hour >= 15 && hour < 20) {
      return MealTime.DINNER;
    }
    return MealTime.SNACK;
  }

  async aboutToAppear() {
    const context = getContext(this) as common.UIAbilityContext;
    await RdbManager.getInstance().init(context);
    this.refreshData();
  }

  onPageShow() {
    this.refreshData();
  }

  async refreshData() {
    const dateStr = this.getSelectedDateStr();
    this.dietRecords = await this.dietDao.getRecordsByDate(dateStr);
    const healthData = await this.healthDao.getHealthByDate(dateStr);

    if (healthData) {
      if (healthData.targetCalories > 0) {
        this.dailyTarget = healthData.targetCalories;
      } else if (healthData.weight > 0 && healthData.height > 0) {
        this.dailyTarget = this.calculateTarget(healthData.weight, healthData.height);
      } else {
        this.dailyTarget = 0;
      }
    } else {
      const todayStr = this.getTodayStr();
      if (dateStr === todayStr) {
        this.dailyTarget = this.currentGlobalTarget;
      } else {
        this.dailyTarget = 0;
      }
    }
  }

  calculateTarget(weight: number, height: number): number {
    let bmr = 10 * weight + 6.25 * height - 5 * this.userAge;
    bmr += this.isMale ? 5 : -161;
    return Math.round(bmr * 1.2);
  }

  async openWeekTrend() {
    this.weeklyStats = await this.dietDao.getWeeklyStats();
    this.trendDialogController.open();
  }

  getSelectedDateStr(): string {
    const y = this.selectedDate.getFullYear();
    const m = (this.selectedDate.getMonth() + 1).toString().padStart(2, '0');
    const d = this.selectedDate.getDate().toString().padStart(2, '0');
    return `${y}-${m}-${d}`;
  }

  changeDate(days: number) {
    let newDate = new Date(this.selectedDate);
    newDate.setDate(newDate.getDate() + days);
    this.selectedDate = newDate;
    this.refreshData();
  }

  async addRecord(food: FoodItem, count: number, mealTime: MealTime) {
    const dateStr = this.getSelectedDateStr();
    const newRecord: DietRecord = {
      id: new Date().getTime().toString(),
      foodId: food.id,
      foodName: food.name,
      mealTime: mealTime,
      portion: count,
      totalCalories: Math.round(food.calories * count),
      date: dateStr
    };

    const success = await this.dietDao.insertRecord(newRecord);

    if (success) {
      await this.refreshData();
      promptAction.showToast({ message: `已记录` });
      AppStorage.SetOrCreate(
        'globalDataUpdate',
        Date.now()
      );
    } else {
      promptAction.showToast({ message: `保存失败` });
    }
  }

  async deleteRecord(recordId: string) {
    const success = await this.dietDao.deleteRecord(recordId);
    if (success) {
      await this.refreshData();
      promptAction.showToast({ message: '已删除' });
    }
  }

  getFilteredRecords(meal: MealTime): DietRecord[] {
    return this.dietRecords.filter(r => r.mealTime === meal);
  }

  getTotalIntake(): number {
    return this.dietRecords.reduce((sum, record) => sum + record.totalCalories, 0);
  }

  getMealCalories(meal: MealTime): number {
    return this.getFilteredRecords(meal).reduce((sum, r) => sum + r.totalCalories, 0);
  }

  build() {
    Column() {
      this.DateNavigator()
      this.DashboardCard()

      Scroll() {
        Column({ space: 20 }) {
          this.MealSection(MealTime.BREAKFAST)
          this.MealSection(MealTime.LUNCH)
          this.MealSection(MealTime.DINNER)
          this.MealSection(MealTime.SNACK)
          Blank().height(80)
        }
        .padding(16)
      }
      .layoutWeight(1)

      Button({
        type: ButtonType.Circle,
        stateEffect: true
      }) {
        Text('+')
          .fontSize(30 * this.fontScale)
          .fontColor(Color.White)
          .margin({ bottom: 4 })
      }
      .width(60)
      .height(60)
      .position({
        x: '80%',
        y: '85%'
      })
      .shadow({
        radius: 10,
        color: '#007DFF'
      })
      .backgroundColor('#007DFF')
      .onClick(() => {
        // 4. 【修改】点击全局加号时，猜测一个餐别
        this.currentSelectedMeal = this.guessMealTime();
        this.addDialogController.open();
      })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5')
  }

  @Builder
  DateNavigator() {
    Row() {
      Column() {
        Text('饮食记录')
          .fontSize(24 * this.fontScale)
          .fontWeight(FontWeight.Bold)
        Text('查看周趋势 >')
          .fontSize(12 * this.fontScale)
          .fontColor('#007DFF')
          .margin({ top: 4 })
          .onClick(() => {
            this.openWeekTrend();
          })
      }
      .alignItems(HorizontalAlign.Start)

      Blank()

      Row() {
        Image($r('app.media.ic_public_arrow_left'))
          .width(24).height(24)
          .onClick(() => this.changeDate(-1))

        Text(this.getSelectedDateStr() === this.getTodayStr() ? '今天' : this.getSelectedDateStr())
          .fontSize(16 * this.fontScale)
          .fontWeight(FontWeight.Medium)
          .margin({
            left: 10,
            right: 10
          })
          .onClick(() => {
            CalendarPickerDialog.show({
              selected: this.selectedDate,
              onAccept: (value) => {
                this.selectedDate = value;
                this.refreshData();
              }
            })
          })

        Image($r('app.media.ic_public_arrow_right_filled'))
          .width(24)
          .height(24)
          .onClick(() => this.changeDate(1))
      }
      .backgroundColor(Color.White)
      .padding({
        left: 8,
        right: 8,
        top: 6,
        bottom: 6
      })
      .borderRadius(16)
    }
    .width('100%')
    .padding({
      left: 16,
      right: 16,
      top: 10,
      bottom: 10
    })
  }

  @Builder
  DashboardCard() {
    Stack() {
      Column() {
        Row() {
          Column() {
            Text('推荐上限 (kcal)')
              .fontSize(12 * this.fontScale)
              .fontColor('#EEE')
            Text(`${this.dailyTarget}`)
              .fontSize(20 * this.fontScale)
              .fontWeight(FontWeight.Bold)
              .fontColor(Color.White)
          }
          .alignItems(HorizontalAlign.Start)

          Blank()
          Column() {
            Text('已摄入 (kcal)')
              .fontSize(12 * this.fontScale)
              .fontColor('#EEE')
            Text(`${this.getTotalIntake()}`)
              .fontSize(24 * this.fontScale)
              .fontWeight(FontWeight.Bold)
              .fontColor(this.getTotalIntake() > this.dailyTarget && this.dailyTarget > 0 ? '#FFCCC7' : Color.White)
          }
          .alignItems(HorizontalAlign.End)
        }
        .width('100%')

        Blank().height(15 * this.fontScale)

        Progress({
          value: this.getTotalIntake(),
          total: this.dailyTarget > 0 ? this.dailyTarget : 1,
          type: ProgressType.Linear
        })
          .width('100%')
          .style({ strokeWidth: 10 * this.fontScale, enableSmoothEffect: true })
          .color(this.getTotalIntake() > this.dailyTarget && this.dailyTarget > 0 ? '#FF4D4F' : Color.White)
          .backgroundColor('rgba(255,255,255,0.3)')
      }
      .padding(20)
    }
    .width('100%')
    .borderRadius(20)
    .linearGradient({
      angle: 135,
      colors: [['#00C6FF', 0.0], ['#0072FF', 1.0]]
    })
    .margin({
      left: 16,
      right: 16
    })
  }

  @Builder
  MealSection(meal: MealTime) {
    Column() {
      Row() {
        Text(meal)
          .fontSize(18 * this.fontScale)
          .fontWeight(FontWeight.Bold)
        Blank()
        Text(`${this.getMealCalories(meal)} kcal`)
          .fontSize(14 * this.fontScale)
          .fontColor('#666')
      }
      .width('100%')
      .margin({ bottom: 10 })

      List() {
        ForEach(this.getFilteredRecords(meal), (item: DietRecord) => {
          ListItem() {
            this.FoodItemRow(item)
          }
          .swipeAction({ end: this.DeleteButton(item.id) })
        }, (item: DietRecord) => item.id)
      }

      if (this.getFilteredRecords(meal).length === 0) {
        Text('点击 + 添加记录')
          .fontSize(12 * this.fontScale)
          .fontColor('#CCC')
          .width('100%')
          .textAlign(TextAlign.Center)
          .padding(10)
          .backgroundColor(Color.White)
          .borderRadius(12)
          .borderStyle(BorderStyle.Dashed)
          .borderWidth(1)
          .borderColor('#EEE')
          .onClick(() => {
            // 5. 【修改】点击特定餐别添加时，强制设置餐别
            this.currentSelectedMeal = meal;
            this.addDialogController.open();
          })
      }
    }
  }

  @Builder
  FoodItemRow(item: DietRecord) {
    Row() {
      Image($r('app.media.startIcon'))
        .width(40)
        .height(40)
        .backgroundColor('#F0F0F0')
        .borderRadius(8)
        .margin({ right: 10 })
      Column({ space: 4 }) {
        Text(item.foodName)
          .fontSize(16 * this.fontScale)
        Text(`x ${item.portion} 份`)
          .fontSize(12 * this.fontScale)
          .fontColor('#999')
      }
      .alignItems(HorizontalAlign.Start)

      Blank()
      Text(`${item.totalCalories}`)
        .fontSize(16 * this.fontScale)
        .fontWeight(FontWeight.Bold)
    }
    .width('100%')
    .padding(10)
    .backgroundColor(Color.White)
    .borderRadius(12)
    .margin({ bottom: 8 })
  }

  @Builder
  DeleteButton(recordId: string) {
    Button() {
      SymbolGlyph($r('sys.symbol.minus'))
        .fontSize(20 * this.fontScale)
        .fontWeight(FontWeight.Bold)
        .fontColor(['#fff'])
    }
    .width(60)
    .height('100%')
    .backgroundColor('#FF4D4F')
    .type(ButtonType.Normal)
    .borderRadius({
      topRight: 12,
      bottomRight: 12
    })
    .margin({ bottom: 8 })
    .onClick(() => {
      this.deleteRecord(recordId);
    })
  }
}