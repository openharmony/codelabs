import { DietRecord, FoodItem, MealTime } from '../model/DietModel';
import { FoodSelectorDialog } from './Diet/FoodSelector';
import { WeekTrendDialog, WeeklyData } from './Diet/WeekTrendChart'; // 确保这里引入正确
import promptAction from '@ohos.promptAction';
import { RdbManager } from '../common/database/RdbManager';
import { DietRecordDao } from '../common/database/DietRecordDao';
import { HealthDao } from '../common/database/HealthDao'; // 【新增】引入 HealthDao
import common from '@ohos.app.ability.common';

@Component
export struct DietPage {
  @State dietRecords: DietRecord[] = [];

  // 【修改】原有的 targetCalories 改名为 globalTarget，仅作为“今天”的兜底值
  @StorageProp('targetCalories') currentGlobalTarget: number = 2000;

  // 【新增】界面真正显示的“当日目标”，随日期变化
  @State dailyTarget: number = 2000;

  // 【新增】引入年龄和性别，用于现场计算 BMR
  @StorageProp('userAge') userAge: number = 25;
  @StorageProp('isMale') isMale: boolean = true;

  @State selectedDate: Date = new Date();
  @StorageProp('appFontScale') fontScale: number = 1.0;
  @State weeklyStats: WeeklyData[] = [];

  private dietDao: DietRecordDao = new DietRecordDao();
  private healthDao: HealthDao = new HealthDao(); // 【新增】实例化

  addDialogController: CustomDialogController = new CustomDialogController({
    builder: FoodSelectorDialog({
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

  // 【核心修改】刷新数据逻辑
  async refreshData() {
    const dateStr = this.getSelectedDateStr();

    // 1. 加载饮食记录
    this.dietRecords = await this.dietDao.getRecordsByDate(dateStr);

    // 2. 加载当天的健康档案（为了获取历史的目标值）
    const healthData = await this.healthDao.getHealthByDate(dateStr);

    if (healthData) {
      // 场景A：当天已经归档了目标热量，直接用
      if (healthData.targetCalories > 0) {
        this.dailyTarget = healthData.targetCalories;
      }
      // 场景B：当天有记录，但目标是0（可能是只记了水，还没生成目标），尝试补救计算
      else if (healthData.weight > 0 && healthData.height > 0) {
        // 现场计算公式：BMR * 1.2
        this.dailyTarget = this.calculateTarget(healthData.weight, healthData.height);
      }
      else {
        this.dailyTarget = 0; // 有记录但无效
      }
    } else {
      // 场景C：当天完全没有健康档案
      const now = new Date();
      const y = now.getFullYear();
      const m = (now.getMonth() + 1).toString().padStart(2, '0');
      const d = now.getDate().toString().padStart(2, '0');
      const todayStr = `${y}-${m}-${d}`;
      if (dateStr === todayStr) {
        // 如果是今天，就用当前的全局目标
        this.dailyTarget = this.currentGlobalTarget;
      } else {
        // 如果是过去/未来且无记录，显示0
        this.dailyTarget = 0;
      }
    }
  }

  // 【新增】辅助计算函数 (Mifflin-St Jeor 公式)
  calculateTarget(weight: number, height: number): number {
    // BMR = 10*W + 6.25*H - 5*A + (5 or -161)
    let bmr = 10 * weight + 6.25 * height - 5 * this.userAge;
    bmr += this.isMale ? 5 : -161;
    return Math.round(bmr * 1.2); // 假设静坐少动系数 1.2
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
      this.DashboardCard() // 卡片现在会使用 this.dailyTarget

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
        // --- 上半部分：文字信息栏 ---
        Row() {
          Column() {
            Text('推荐上限 (kcal)')
              .fontSize(12 * this.fontScale)
              .fontColor('#EEE')
            // 【修改】这里使用 dailyTarget，不再是全局的 targetCalories
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
              // 【修改】比较时也使用 dailyTarget
              .fontColor(this.getTotalIntake() > this.dailyTarget && this.dailyTarget > 0 ? '#FFCCC7' : Color.White)
          }
          .alignItems(HorizontalAlign.End)
        }
        .width('100%')

        // --- 中间间距 ---
        Blank().height(15 * this.fontScale)

        // --- 下半部分：进度条 ---
        Progress({
          value: this.getTotalIntake(),
          // 【修改】防止除以0，如果 dailyTarget 是 0，进度条设为 1 或 0
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
      colors: [[ '#00C6FF', 0.0 ], [ '#0072FF', 1.0 ]]
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
          .onClick(() => { this.addDialogController.open(); })
      }
    }
  }

  @Builder
  FoodItemRow(item: DietRecord) {
    Row() {
      Image($r('app.media.icon'))
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
    .onClick(() => { this.deleteRecord(recordId); })
  }
}