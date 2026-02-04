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
import { WaterDao, WaterRecord } from '../common/database/WaterDao';
import { RdbManager } from '../common/database/RdbManager';
import promptAction from '@ohos.promptAction';
import { WaterChartData, WaterTrendDialog } from './WaterTrendDialog';

interface TimeInfo {
  timePercent: number;
  stepIndex: number;
}

@Component
export struct WaterPage {
  @State actualWater: number = 0;
  @StorageLink('targetWater') goalWater: number = 2000;
  @State inputWater: string = '';
  @StorageProp('appFontScale') fontScale: number = 1.0;
  @State weeklyData: WaterChartData[] = [];
  @State selectedDate: Date = new Date();
  @StorageProp('globalDataUpdate') @Watch('onUpdateData') updateSignal: number = 0;
  // 【修改点1】移除 @State 设置变量，改为内部固定常量
  dialogController: CustomDialogController | null = null;
  // 每天分为 6 个时段进行检查（约每2小时一次），防止频繁弹窗打扰
  private readonly REMIND_COUNT: number = 6;
  private readonly WAKE_START_HOUR: number = 8;
  private readonly WAKE_END_HOUR: number = 20;
  private lastNotifiedStep: number = -1;
  // 样式常量
  private readonly COLOR_PRIMARY: string = '#007DFF';
  private readonly COLOR_BACKGROUND: string = '#F1F7FF';
  private readonly COLOR_CARD_BG: string = '#FFFFFF';
  private readonly COLOR_TEXT_MAIN: string = '#000000';
  private readonly COLOR_TEXT_SUB: string = '#666666';
  private readonly BUTTON_SHADOW_COLOR: string = '#10000000';
  private readonly BUTTON_RADIUS: number = 15;
  private readonly SHADOW_RADIUS: number = 4;
  private waterDao = new WaterDao();

  onUpdateData() {
    this.refreshData();
  }

  getSelectedDateStr(): string {
    const y = this.selectedDate.getFullYear();
    const m = (this.selectedDate.getMonth() + 1).toString().padStart(2, '0');
    const d = this.selectedDate.getDate().toString().padStart(2, '0');
    return `${y}-${m}-${d}`;
  }

  getSystemTodayStr(): string {
    const now = new Date();
    const y = now.getFullYear();
    const m = (now.getMonth() + 1).toString().padStart(2, '0');
    const d = now.getDate().toString().padStart(2, '0');
    return `${y}-${m}-${d}`;
  }

  changeDate(days: number) {
    let newDate = new Date(this.selectedDate);
    newDate.setDate(newDate.getDate() + days);
    this.selectedDate = newDate;
    this.refreshData();
  }

  async aboutToAppear() {
    const store = RdbManager.getInstance().getRdbStore();
    if (!store) {
      return;
    }
    await this.refreshData();

    // 如果是今天，检查是否需要弹窗提醒
    if (this.getSelectedDateStr() === this.getSystemTodayStr()) {
      this.checkAndNotify();
    }
  }

  async refreshData() {
    try {
      const dateStr = this.getSelectedDateStr();
      let record = await this.waterDao.getWaterByDate(dateStr);
      if (record) {
        this.actualWater = record.actual;
        this.goalWater = record.goal;
      } else {
        this.actualWater = 0;
      }
    } catch (error) {
      console.error(`WaterPage refresh error: ${JSON.stringify(error)}`);
    }
  }

  async openTrendDialog() {
    const dates: string[] = [];
    for (let i = 6; i >= 0; i--) {
      const d = new Date();
      d.setDate(d.getDate() - i);
      const y = d.getFullYear();
      const m = (d.getMonth() + 1).toString().padStart(2, '0');
      const day = d.getDate().toString().padStart(2, '0');
      dates.push(`${y}-${m}-${day}`);
    }
    const records = await this.waterDao.getWaterBatch(dates);
    this.weeklyData = records.map(r => new WaterChartData(r.date, r.actual));

    this.dialogController = new CustomDialogController({
      builder: WaterTrendDialog({ data: this.weeklyData }),
      customStyle: true,
      alignment: DialogAlignment.Center,
      autoCancel: true
    });
    this.dialogController.open();
  }

  async addWater(amount: number) {
    if (amount <= 0) {
      return;
    }
    this.actualWater += amount;
    const record: WaterRecord = {
      date: this.getSelectedDateStr(),
      actual: this.actualWater,
      goal: this.goalWater
    };
    const success = await this.waterDao.saveWaterRecord(record);
    if (success) {
      promptAction.showToast({ message: `已记录`, duration: 2000 });
      AppStorage.SetOrCreate('globalDataUpdate', Date.now());

      // 记录数据后，重新检查进度，如果达标了就不再弹窗
      if (this.getSelectedDateStr() === this.getSystemTodayStr()) {
        this.checkAndNotify();
      }
    }
    this.inputWater = '';
  }

  calculatePercent(): number {
    if (this.goalWater <= 0) {
      return 0;
    }
    const percent = Math.floor((this.actualWater / this.goalWater) * 100);
    return Math.min(percent, 100);
  }

  getTimeInfo(): TimeInfo {
    const now = new Date();
    const start = new Date();
    const end = new Date();

    // 使用内部常量
    start.setHours(this.WAKE_START_HOUR, 0, 0, 0);
    end.setHours(this.WAKE_END_HOUR, 0, 0, 0);

    if (now <= start) {
      return { timePercent: 0, stepIndex: -1 };
    }
    if (now >= end) {
      return { timePercent: 100, stepIndex: this.REMIND_COUNT - 1 };
    }

    const passed = now.getTime() - start.getTime();
    const total = end.getTime() - start.getTime();
    const timePercent = Math.floor((passed / total) * 100);
    // 使用内部常量 REMIND_COUNT
    const stepLength = total / this.REMIND_COUNT;
    const stepIndex = Math.floor(passed / stepLength);
    return { timePercent: timePercent, stepIndex: stepIndex };
  }

  async checkAndNotify() {
    if (this.getSelectedDateStr() !== this.getSystemTodayStr()) {
      return;
    }
    // 【修改点2】移除开关判断，默认总是检查（只要是在今天）

    const info = this.getTimeInfo();

    // 如果当前时间段（例如 10:00-12:00）已经提醒过了，就不再打扰
    if (info.stepIndex <= this.lastNotifiedStep) {
      return;
    }

    const drinkPercent = this.calculatePercent();

    // 如果喝水进度达标，标记该时段已处理，不弹窗
    if (drinkPercent >= info.timePercent) {
      this.lastNotifiedStep = info.stepIndex;
      return;
    }

    // 记录该时段已提醒
    this.lastNotifiedStep = info.stepIndex;
    this.showReminderDialog(info.timePercent, drinkPercent);
  }

  showReminderDialog(timePercent: number, drinkPercent: number) {
    AlertDialog.show({
      title: '温馨提示 🥤',
      message: `今日时间已过 ${timePercent}%，但您的饮水目标仅完成 ${drinkPercent}%。\n\n身体缺水啦，快喝杯水休息一下吧！`,
      autoCancel: true,
      alignment: DialogAlignment.Center,
      offset: { dx: 0, dy: -20 },
      confirm: {
        value: '立刻去喝',
        fontColor: '#FFFFFF',
        backgroundColor: this.COLOR_PRIMARY,
        action: () => {
          console.info('用户点击了去喝水');
        }
      },
      cancel: () => {
        console.info('用户暂时忽略');
      }
    });
  }

  build() {
    Column() {
      this.DateNavigator()
      this.waterProgressCard()

      Text('快捷加水')
        .fontSize(16 * this.fontScale)
        .fontColor(this.COLOR_TEXT_SUB)
        .alignSelf(ItemAlign.Start)
        .margin({ bottom: 12, left: 10 })

      this.quickActionButtons()
      this.waterInputRow()

      // 【修改点3】删除了底部的 Row（提醒设置区域）
    }
    .width('100%')
    .height('100%')
    .padding(20)
    .backgroundColor(this.COLOR_BACKGROUND)
  }

  // ... (DateNavigator, waterProgressCard, quickActionButtons, quickBtn, waterInputRow 保持不变)

  @Builder
  DateNavigator() {
    Row() {
      Column() {
        Text('饮水追踪')
          .fontSize(24 * this.fontScale)
          .fontWeight(FontWeight.Bold)
          .fontColor(this.COLOR_TEXT_MAIN)
        Text('周趋势 >')
          .fontSize(12 * this.fontScale)
          .fontColor(this.COLOR_PRIMARY)
          .margin({ top: 4 })
          .onClick(() => this.openTrendDialog())
      }
      .alignItems(HorizontalAlign.Start)

      Blank()
      Row() {
        Image($r('app.media.ic_public_arrow_left'))
          .width(24).height(24)
          .onClick(() => this.changeDate(-1))
        Text(this.getSelectedDateStr() === this.getSystemTodayStr() ? '今天' : this.getSelectedDateStr())
          .fontSize(16 * this.fontScale)
          .fontWeight(FontWeight.Medium)
          .fontColor(this.COLOR_TEXT_MAIN)
          .margin({ left: 10, right: 10 })
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
          .width(24).height(24)
          .onClick(() => this.changeDate(1))
      }
      .backgroundColor(this.COLOR_CARD_BG)
      .padding({
        left: 8,
        right: 8,
        top: 6,
        bottom: 6
      })
      .borderRadius(16)
    }
    .width('100%')
    .padding({ top: 10, bottom: 20 })
    .alignItems(VerticalAlign.Center)
  }

  @Builder
  waterProgressCard() {
    Stack() {
      Progress({
        value: this.actualWater,
        total: this.goalWater,
        type: ProgressType.Ring
      })
        .width(220 * this.fontScale)
        .height(220 * this.fontScale)
        .style({ strokeWidth: 20 * this.fontScale })
        .color(this.COLOR_PRIMARY)
      Column() {
        Image($r('app.media.startIcon'))
          .width(40)
          .height(40)
          .margin({ bottom: 10 })
          .objectFit(ImageFit.Contain)
        Text(`${this.calculatePercent()}%`)
          .fontSize(40 * this.fontScale)
          .fontWeight(FontWeight.Bolder)
          .fontColor(this.COLOR_PRIMARY)
        Text(`${this.actualWater} / ${this.goalWater} ml`)
          .fontSize(16 * this.fontScale)
          .fontColor(this.COLOR_TEXT_SUB)
          .margin({ top: 5 })
      }
    }
    .margin({ bottom: 40 })
  }

  @Builder
  quickActionButtons() {
    Row({ space: 15 }) {
      this.quickBtn(300, '杯')
      this.quickBtn(500, '瓶')
      this.quickBtn(1000, '大瓶')
    }
    .width('100%')
    .margin({ bottom: 30 })
    .justifyContent(FlexAlign.SpaceBetween)
  }

  @Builder
  quickBtn(amount: number, label: string) {
    Button() {
      Column() {
        Text(`+${amount}`)
          .fontSize(18 * this.fontScale)
          .fontWeight(FontWeight.Bold)
          .fontColor(this.COLOR_PRIMARY)
        Text(label)
          .fontSize(12 * this.fontScale)
          .fontColor(this.COLOR_TEXT_SUB)
      }
    }
    .layoutWeight(1)
    .height(70 * this.fontScale)
    .backgroundColor(this.COLOR_CARD_BG)
    .borderRadius(this.BUTTON_RADIUS)
    .shadow({ radius: this.SHADOW_RADIUS, color: this.BUTTON_SHADOW_COLOR })
    .onClick(() => this.addWater(amount))
  }

  @Builder
  waterInputRow() {
    Row({ space: 10 }) {
      TextInput({ placeholder: '手动输入 (ml)', text: this.inputWater })
        .layoutWeight(1)
        .height(50)
        .type(InputType.Number)
        .fontSize(16 * this.fontScale)
        .backgroundColor(this.COLOR_CARD_BG)
        .onChange(v => this.inputWater = v)
      Button('喝水')
        .height(50)
        .width(100)
        .fontSize(16 * this.fontScale)
        .backgroundColor(this.COLOR_PRIMARY)
        .onClick(() => {
          const val = Number(this.inputWater);
          if (val > 0) {
            this.addWater(val);
          }
        })
    }
  }
}