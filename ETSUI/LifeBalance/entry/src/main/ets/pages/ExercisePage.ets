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
import promptAction from '@ohos.promptAction';
import { RdbManager } from '../common/database/RdbManager';
import common from '@ohos.app.ability.common';
import { ExerciseItem, ExerciseRecord } from '../model/ExerciseModel';
import { ExerciseRecordDao } from '../common/database/ExerciseRecordDao';
import { ExerciseSelectorDialog } from './Exercise/ExerciseSelector';
import { HealthDao } from '../common/database/HealthDao';
import { WeekTrendDialog } from './Diet/WeekTrendChart';

// 定义接口，防止类型检查报错
interface WeeklyStatsItem {
  date: string;
  total: number;
}

@Component
export struct ExercisePage {
  @State exerciseRecord: ExerciseRecord[] = [];
  @StorageProp('targetExercise') targetExercise: number = 2;
  @State selectedDate: Date = new Date();
  @StorageProp('targetExercise') currentGlobalTarget: number = 2;
  @State displayTarget: number = 2;

  // 1. 引入缩放变量
  @StorageProp('appFontScale') fontScale: number = 1.0;

  // 【新增】用来存储周统计数据
  @State weeklyStats: WeeklyStatsItem[] = [];

  private exerciseDao: ExerciseRecordDao = new ExerciseRecordDao();
  private healthDao: HealthDao = new HealthDao();

  // 运动选择弹窗
  addDialogController: CustomDialogController = new CustomDialogController({
    builder: ExerciseSelectorDialog({
      onConfirm: (item: ExerciseItem, count: number): Promise<void> => this.addRecord(item, count)
    }),
    alignment: DialogAlignment.Bottom,
    offset: {
      dx: 0,
      dy: -20
    },
    customStyle: false,
    autoCancel: true
  });

  // 【新增】周趋势弹窗控制器
  trendDialogController: CustomDialogController = new CustomDialogController({
    builder: WeekTrendDialog({
      weeklyData: this.weeklyStats, // 传入数据
      title: '本周运动消耗趋势',     // 自定义标题
      unit: 'kcal'                  // 单位
    }),
    alignment: DialogAlignment.Center,
    customStyle: false,
    autoCancel: true
  });
  getTodayStr(): string {
    const now = new Date();
    const y = now.getFullYear();
    const m = (now.getMonth() + 1).toString().padStart(2, '0');
    const d = now.getDate().toString().padStart(2, '0');
    return `${y}-${m}-${d}`;
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

    // 1. 并行查询：既查运动记录，也查当天的健康档案
    const healthData = await this.healthDao.getHealthByDate(dateStr);
    this.exerciseRecord = await this.exerciseDao.getRecordsByDate(dateStr);

    // 2. 核心逻辑：决定 displayTarget 显示什么
    if (healthData) {
      // --- 场景A：当天有归档记录 ---
      if (healthData.targetExercise > 0) {
        // 1. 如果记录里有有效值（新数据），直接尊重大于0的记录
        this.displayTarget = healthData.targetExercise;
      } else {
        // 2. 【补救逻辑】如果记录里是0（旧数据），则根据那天的 BMI 倒推建议值
        // 规则：如果当时 BMI >= 24 (超重)，建议 4单位(60分钟)，否则 2单位(30分钟)
        const historyBMI = healthData.bmi || 0;
        this.displayTarget = historyBMI >= 24 ? 4 : 2;
      }
    } else {
      // --- 场景B：当天连健康档案都没有 ---
      const todayStr = new Date().toISOString().split('T')[0];

      if (dateStr === todayStr) {
        // 如果是“今天”，且还没存数据库，那么暂时显示“当前的全局目标”
        this.displayTarget = this.currentGlobalTarget;
      } else {
        // 如果是“过去”或“未来”且无记录，确实无法判断当时体重，只能归零
        this.displayTarget = 0;
      }
    }
  }

  // 【新增】打开周趋势图表的逻辑
  async openWeekTrend() {
    // 1. 调用 Dao 获取数据
    this.weeklyStats = await this.exerciseDao.getWeeklyStats();
    // 2. 打开弹窗
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

  async addRecord(exercise: ExerciseItem, count: number) {
    const dateStr = this.getSelectedDateStr();
    const newRecord: ExerciseRecord = {
      id: new Date().getTime().toString(),
      exerciseId: exercise.id,
      exerciseName: exercise.name,
      portion: count,
      totalCalories: exercise.calories * count,
      date: dateStr
    };

    const success = await this.exerciseDao.insertRecord(newRecord);

    if (success) {
      await this.refreshData();
      promptAction.showToast({ message: `已记录` });
      AppStorage.SetOrCreate('globalDataUpdate', Date.now());
    } else {
      promptAction.showToast({ message: `保存失败` });
    }
  }

  async deleteRecord(recordId: string) {
    const success = await this.exerciseDao.deleteRecord(recordId);
    if (success) {
      await this.refreshData();
      promptAction.showToast({ message: '已删除' });
    }
  }

  getTotalTime(): number {
    return this.exerciseRecord.reduce((sum, record) => sum + record.portion, 0)
  }

  getTotalConsumption(): number {
    return this.exerciseRecord.reduce((sum, record) => sum + record.totalCalories, 0);
  }

  build() {
    Column() {
      // 1. 顶部日期导航 (修改这里，加入按钮)
      this.DateNavigator()

      // ... (DashboardCard, Scroll, Button 等保持不变) ...
      Row() {
        this.DashboardCard1()
      }
      .padding({
        left: 16,
        right: 16
      })


      List() {
        ForEach(this.exerciseRecord, (item: ExerciseRecord) => {
          ListItem() {
            this.ExerciseItemRow(item)
          }
          .swipeAction({ end: this.DeleteButton(item.id) })
        }, (item: ExerciseRecord) => item.id)
      }
      .width('100%')
      .padding(16)
      .layoutWeight(1)

      Button({
        type: ButtonType.Circle,
        stateEffect: true
      }) {
        Text('+')
          .fontSize(30 * this.fontScale) // 修改
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
        Text('运动记录')
          .fontSize(24 * this.fontScale)
          .fontWeight(FontWeight.Bold)

        // 【新增】查看趋势的文字按钮
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

  // ... (DashboardCard, MealSection, FoodItemRow, DeleteButton 保持不变) ...
  @Builder
  DashboardCard1() {
    Column() {
      Row() {
        Text('已消耗')
          .fontSize(20 * this.fontScale)
          .fontWeight(FontWeight.Bold)
          .fontColor('#EEE')
        Blank()
        Text(`${this.getTotalConsumption()} kcal`)
          .fontSize(24 * this.fontScale)
          .fontWeight(FontWeight.Bold)
          .fontColor('#EEE')
      }
      .width('100%')
      .padding({ bottom: 20 })
      .alignItems(VerticalAlign.Center)

      Row() {
        Column() {
          Text('目标 (分钟)')
            .fontSize(12 * this.fontScale)
            .fontColor('#EEE')
          Text(`${this.displayTarget * 15}`)
            .fontSize(20 * this.fontScale)
            .fontWeight(FontWeight.Bold)
            .fontColor(Color.White)
        }
        .alignItems(HorizontalAlign.Start)
        Blank()
        Column() {
          Text('已运动 (分钟)')
            .fontSize(12 * this.fontScale)
            .fontColor('#EEE')
          Text(`${this.getTotalTime() * 15}`)
            .fontSize(24 * this.fontScale)
            .fontWeight(FontWeight.Bold)
            .fontColor(this.getTotalTime() >= this.displayTarget ? '#bff3a5' : Color.White)
        }
        .alignItems(HorizontalAlign.End)
      }
      .width('100%')
      .padding({ bottom: 8 })

      Progress({
        value: this.getTotalTime(),
        total: this.displayTarget === 0 ? 1 : this.displayTarget,
        type: ProgressType.Linear }
      )
        .width('100%')
        .style({
          strokeWidth: 10,
          enableSmoothEffect: true
        })
        .color(this.getTotalTime() >= this.targetExercise ? '#52c41a' : Color.White)
        .backgroundColor('rgba(255,255,255,0.3)')
    }
    .width('100%')
    .padding(20)
    .linearGradient({
      angle: 135,
      colors: [[ '#00C6FF', 0.0 ], [ '#0072FF', 1.0 ]] }
    )
    .borderRadius(20)
  }

  @Builder
  ExerciseItemRow(item: ExerciseRecord) {
    Row() {
      Image($r('app.media.icon'))
        .width(40)
        .height(40)
        .backgroundColor('#F0F0F0')
        .borderRadius(8)
        .margin({ right: 10 })
      Column({ space: 4 }) {
        Text(item.exerciseName)
          .fontSize(16 * this.fontScale)
        Text(`${item.portion * 15} 分钟`)
          .fontSize(14 * this.fontScale)
          .fontColor('#777')
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
    .onClick(() => { this.deleteRecord(recordId); })
  }
}
