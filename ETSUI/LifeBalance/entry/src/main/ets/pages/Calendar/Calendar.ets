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
import { UserStore, DailyData } from '../../model/information';
import { HealthDao } from '../../common/database/HealthDao';
import { WaterDao } from '../../common/database/WaterDao';
import { DietRecordDao } from '../../common/database/DietRecordDao';
import { ExerciseRecordDao } from '../../common/database/ExerciseRecordDao';

@Component
export struct CalendarComponent {
  @State dayData: DailyData | null = null
  @State isLoading: boolean = false
  @State selectedDate: string = ((d) => {
    const y = d.getFullYear();
    const m = (d.getMonth() + 1).toString().padStart(2, '0');
    const day = d.getDate().toString().padStart(2, '0');
    return `${y}-${m}-${day}`;
  })(new Date())

  @StorageProp('globalDataUpdate') @Watch('onUpdateData') updateSignal: number = 0

  private healthDao = new HealthDao()
  private waterDao = new WaterDao()
  private dietDao = new DietRecordDao()
  private exerciseDao = new ExerciseRecordDao()

  async aboutToAppear() {
    this.loadDataByDate(this.selectedDate)
  }
  onUpdateData() {
    console.info('[Calendar] 监听到数据更新信号，正在刷新...');
    this.loadDataByDate(this.selectedDate);
  }
  async loadDataByDate(dateStr: string) {
    this.isLoading = true;
    this.dayData = null;

    try {
      // 1. 从三个 Dao 提取数据
      const health = await this.healthDao.getHealthByDate(dateStr);
      const water = await this.waterDao.getWaterByDate(dateStr);
      const dietRecords = await this.dietDao.getRecordsByDate(dateStr);
      const exerciseRecords = await this.exerciseDao.getRecordsByDate(dateStr);

      // 2. 提取并计算实际饮食总热量（这就是从数据库提取的实际卡路里）
      let totalCals = 0;
      if (dietRecords && dietRecords.length > 0) {
        dietRecords.forEach(item => {
          totalCals += item.totalCalories;
        });
      }

      // 3. 计算当天总运动时间
      let totalExerciseTime = 0;
      if (exerciseRecords && exerciseRecords.length > 0) {
        // 假设一份是 15 分钟
        exerciseRecords.forEach(item => totalExerciseTime += (item.portion * 15));
      }
      // 如果没设置过，默认给个 30 分钟 (2份)
      const targetPortion = AppStorage.Get<number>('targetExercise') ?? 2;
      const targetMin = targetPortion * 15;

      // 4. 【核心修改】确定计划运动时间
      let targetMinn = 0;
      if (health) {
        // --- 场景 A：当天有健康档案 ---

        if (health.targetExercise > 0) {
          // 1. 如果记录里存了有效值（说明是新产生的数据），直接用
          targetMinn = health.targetExercise * 15;
        } else {
          // 2. 【补救逻辑】如果记录里是 0（说明是旧数据），则根据当天的 BMI 倒推
          // 规则：BMI >= 24 (超重) 建议 60分钟(4份)，否则 30分钟(2份)
          // 这里的 bmi 是那天存档时的数值，能反映当时的身高体重情况
          const historyBMI = health.bmi || 0;
          const suggestPortion = historyBMI >= 24 ? 4 : 2;
          targetMinn = suggestPortion * 15;
        }

      } else {
        // --- 场景 B：当天连健康档案都没有 ---

        const todayStr = new Date().toISOString().split('T')[0];
        if (dateStr === todayStr) {
          // 如果是今天且还没生成记录，暂时读取当前的全局目标
          const targetPortion = AppStorage.Get<number>('targetExercise') ?? 2;
          targetMinn = targetPortion * 15;
        } else {
          // 历史日期如果没有记录，确实无法计算（不知道当时多重），只能显示 0
          targetMinn = 0;
        }
      }

      setTimeout(() => {
        // 只要有任何一项记录，就显示卡片
        if (health || water || dietRecords.length > 0) {

          // 计算该日期的基础代谢 (BMR)
          let calculatedBmr = 0;
          if (health) {
            // 使用 Mifflin-St Jeor 公式 (这里以男性为例，如需区分性别需在数据库增加字段)
            calculatedBmr = 10 * health.weight + 6.25 * health.height - 5 * health.age + 5;
          }

          this.dayData = {
            height: health?.height ?? 0,
            weight: health?.weight ?? 0,
            age: health?.age ?? 0,
            bmi: health?.bmi ?? 0,
            bmr: Math.round(calculatedBmr),
            recommendedWater: water?.goal ?? (health?.targetWater ?? 2000),
            actualWater: water?.actual ?? 0,
            // --- 关键：这里就是从数据库饮食表提取出的实际值 ---
            actualCalories: totalCals,
            // 目标热量（可以设定为 BMR 或 BMR * 1.2）
            targetCalories: calculatedBmr > 0 ? Math.round(calculatedBmr * 1.2) : 2000,
            exerciseDuration: totalExerciseTime, // 实际做的时间
            targetExercise: targetMinn            // 建议的时间
          };
          console.info(`[Calendar] 日期 ${dateStr} 提取实际卡路里: ${totalCals}`);
        }
        this.isLoading = false;
      }, 16);
    } catch (e) {
      console.error(`[Calendar] 提取数据失败: ${JSON.stringify(e)}`);
      this.isLoading = false;
    }
  }

  build() {
    Column({ space: 16 }) {
      Text('健康历程')
        .fontSize(22)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 20 })

      CalendarPicker({ selected: new Date(this.selectedDate) })
        .edgeAlign(CalendarAlign.CENTER)
        .onChange((value: Date) => {
          const year = value.getFullYear();
          const month = (value.getMonth() + 1).toString().padStart(2, '0');
          const day = value.getDate().toString().padStart(2, '0');
          const newDate = `${year}-${month}-${day}`;
          this.selectedDate = newDate;
          this.loadDataByDate(newDate);
        })

      Scroll() {
        Column({ space: 12 }) {
          if (!this.isLoading) {
            if (this.dayData) {
              this.renderDataCard(this.dayData)
            } else {
              this.renderEmptyState()
            }
          } else {
            LoadingProgress().width(60).height(60).color('#007DFF').margin({ top: 100 })
          }
        }.width('100%')
      }.layoutWeight(1)
    }
    .width('100%').height('100%').padding(16).backgroundColor('#F1F3F5')
  }

  @Builder
  renderDataCard(data: DailyData) {
    Column({ space: 14 }) {
      Row() {
        SymbolGlyph($r('sys.symbol.calendar')).fontSize(20).fontColor(['#007DFF'])
        Text(` ${this.selectedDate} 记录`).fontSize(18).fontWeight(FontWeight.Bold).fontColor('#333')
      }.width('100%')

      Divider().opacity(0.4)

      this.DataItem(
        '身高',
        `${data.height} cm`
      )
      this.DataItem(
        '体重',
        `${data.weight} kg`
      )
      this.DataItem(
        '年龄',
        `${data.age} 岁`
      )
      this.DataItem(
        'BMI',
        data.bmi.toFixed(2)
      )

      this.ProgressItem(
        '饮食热量',
        data.actualCalories ?? 0,
        data.targetCalories ?? 2000,
        'kcal',
        '#FF9C6E'
      )
      this.ProgressItem(
        '饮水进度',
        data.actualWater,
        data.recommendedWater,
        'ml',
        '#007DFF')

      this.ProgressItem(
        '运动时长',
        data.exerciseDuration ?? 0,
        data.targetExercise ?? 30,
        'min',
        '#52C41A'
      )

    }
    .id(`card_${this.selectedDate}`)
    .padding(20).backgroundColor(Color.White).borderRadius(20).width('100%')
    .shadow({ radius: 15, color: '#15000000' })
  }

  @Builder
  DataItem(label: string, value: string) {
    Row() {
      Text(label).fontColor('#666').fontSize(14)
      Blank()
      Text(value).fontWeight(FontWeight.Medium).fontColor('#333').fontSize(14)
    }.width('100%')
  }

  @Builder
  ProgressItem(label: string, actual: number, target: number, unit: string, color: string) {
    Column({ space: 8 }) {
      Row() {
        Text(label).fontColor('#666').fontSize(14)
        Blank()
        Text(`${actual} / ${target} ${unit}`)
          .fontWeight(FontWeight.Medium)
          .fontSize(14)
          .fontColor(actual >= target ? '#52C41A' : color)
      }.width('100%')

      Progress({ value: actual, total: target, type: ProgressType.Linear })
        .width('100%')
        .height(8)
        .color(actual >= target ? '#52C41A' : color)
    }.width('100%').margin({ top: 5 })
  }

  @Builder
  renderEmptyState() {
    Column({ space: 10 }) {
      Image($r('app.media.icon')).width(100).height(100).opacity(0.1).grayscale(1)
      Text(`${this.selectedDate}`).fontSize(16).fontColor('#999')
      Text('这一天没有留下健康脚印').fontSize(14).fontColor('#CCC')
    }.width('100%').margin({ top: 80 })
  }
}