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
import { WaterDao } from '../../common/database/WaterDao'
import { DietRecordDao } from '../../common/database/DietRecordDao'
import { HealthDao } from '../../common/database/HealthDao'
import { ExerciseRecordDao } from '../../common/database/ExerciseRecordDao'


// 通用图表数据接口
interface HealthChartData {
  date: string
  value: number
}

@Component
export struct StatisticsComponent {
  // --- 状态数据 ---
  @State weeklyWaterList: HealthChartData[] = []
  @State weeklyCaloriesList: HealthChartData[] = []
  @State weeklyWeightList: HealthChartData[] = []
  @State weeklyExerciseList: HealthChartData[] = []
  @State isRefreshing: boolean = true

  // --- DAO 实例 ---
  private waterDao = new WaterDao()
  private dietDao = new DietRecordDao()
  private healthDao = new HealthDao()
  private exerciseDao = new ExerciseRecordDao()
  private settings: RenderingContextSettings = new RenderingContextSettings(true)
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings)

  async aboutToAppear() {
    this.refreshStats()
  }
  private getLocalDateStr(date: Date): string {
    const y = date.getFullYear();
    const m = (date.getMonth() + 1).toString().padStart(2, '0');
    const d = date.getDate().toString().padStart(2, '0');
    return `${y}-${m}-${d}`;
  }
  async refreshStats() {
    this.isRefreshing = true
    let waterList: HealthChartData[] = []
    let calorieList: HealthChartData[] = []
    let weightList: HealthChartData[] = []
    let exerciseList: HealthChartData[] = []

    for (let i = 6; i >= 0; i--) {
      let d = new Date()
      d.setDate(d.getDate() - i)
      let fullKey = this.getLocalDateStr(d)
      let displayKey = `${d.getMonth() + 1}/${d.getDate()}`

      const waterRes = await this.waterDao.getWaterByDate(fullKey)
      waterList.push({ date: displayKey, value: waterRes ? waterRes.actual : 0 })

      const dietRes = await this.dietDao.getRecordsByDate(fullKey)
      let dayCals = 0
      dietRes.forEach(item => dayCals += item.totalCalories)
      calorieList.push({ date: displayKey, value: dayCals })

      const healthRes = await this.healthDao.getHealthByDate(fullKey)
      weightList.push({ date: displayKey, value: healthRes ? healthRes.weight : 0 })

      const exerciseRes = await this.exerciseDao.getRecordsByDate(fullKey)
      let dayExerciseTime = 0
      exerciseRes.forEach(item => dayExerciseTime += (item.portion * 15)) // 假设一份是15分钟
      exerciseList.push({ date: displayKey, value: dayExerciseTime })
    }

    this.weeklyWaterList = waterList
    this.weeklyCaloriesList = calorieList
    this.weeklyWeightList = weightList
    this.weeklyExerciseList = exerciseList
    this.isRefreshing = false
  }

  build() {
    Scroll() {
      Column({ space: 16 }) {
        if (this.isRefreshing) {
          LoadingProgress().width(50).height(50).color('#007DFF').margin({ top: 100 })
        } else {
          this.BarChartCard('近一周饮水趋势 (ml)', this.weeklyWaterList, 2500, '#007DFF')

          this.BarChartCard('近一周热量摄入 (kcal)', this.weeklyCaloriesList, 3000, '#FF9C6E')

          this.BarChartCard('近一周运动时长 (分钟)', this.weeklyExerciseList, 120, '#52C41A')

          this.LineChartCard('体重变化趋势 (kg)', this.weeklyWeightList)

          this.SummaryCard()
        }
      }
      .padding(16)
      .width('100%')
    }
    .scrollBar(BarState.Off)
    .backgroundColor('#F1F3F5')
  }

  @Builder
  BarChartCard(title: string, data: HealthChartData[], max: number, color: string) {
    Column() {
      Text(title).fontSize(16).fontWeight(FontWeight.Bold).margin({ bottom: 20 }).width('100%')

      Row() {
        ForEach(data, (item: HealthChartData) => {
          Column() {
            Stack({ alignContent: Alignment.Bottom }) {
              Rect().width(14).height(120).fill('#F1F3F5').borderRadius(7)
              Rect()
                .width(14)
                .height(Math.min((item.value / max) * 120, 120))
                .fill(color).borderRadius(7)
                .animation({ duration: 600, curve: Curve.FastOutSlowIn })
            }
            Text(item.date).fontSize(10).margin({ top: 8 }).fontColor('#999')
          }
        })
      }.width('100%').justifyContent(FlexAlign.SpaceAround)
    }
    .padding(20).backgroundColor(Color.White).borderRadius(20)
    .shadow({ radius: 10, color: '#05000000' })
  }

  @Builder
  LineChartCard(title: string, data: HealthChartData[]) {
    Column() {
      Text(title).fontSize(16).fontWeight(FontWeight.Bold).margin({ bottom: 20 }).width('100%')

      Stack({ alignContent: Alignment.TopStart }) {
        // 1. 背景参考线 (高度固定 100)
        Column({ space: 24 }) {
          Divider().color('#F5F5F5')
          Divider().color('#F5F5F5')
          Divider().color('#F5F5F5')
        }.width('100%').height(100)

        // 2. 中层：Canvas 绘制连线
        Canvas(this.context)
          .width('100%')
          .height(100)
          .onReady(() => {
            this.drawWeightLine(data)
          })

        ForEach(data, (item: HealthChartData, index: number) => {
          if (item.value > 0) {
            Circle({ width: 8, height: 8 })
              .fill('#52C41A')
              .position({
                x: `${(index + 0.5) * (100 / data.length)}%`,
                y: 100 - ((item.value - 40) * 1.5) - 4
              })
              .markAnchor({ x: 4 })
          }

          Text(item.date)
            .fontSize(10)
            .fontColor('#999')
            .position({
              x: `${(index + 0.5) * (100 / data.length)}%`,
              y: 110
            })
            .markAnchor({ x: '50%' }) // 文字居中对齐
        })
      }
      .height(140)
      .width('100%')
    }
    .padding(20).backgroundColor(Color.White).borderRadius(20)
    .shadow({ radius: 10, color: '#05000000' })
  }

  // --- 绘图逻辑微调 ---
  drawWeightLine(data: HealthChartData[]) {
    if (data.length === 0) return

    // 这一步确保 Canvas 内部尺寸与显示尺寸一致，防止坐标偏移
    const width = this.context.width
    const height = 100
    this.context.clearRect(0, 0, width, height)

    this.context.strokeStyle = '#52C41A'
    this.context.lineWidth = 2
    this.context.lineJoin = 'round'

    const stepX = width / data.length

    this.context.beginPath()
    let isFirst = true

    data.forEach((item, index) => {
      if (item.value > 0) {
        // 必须与 UI 层的 position 逻辑完全一致
        const x = (index + 0.5) * stepX
        const y = 100 - ((item.value - 40) * 1.5)

        if (isFirst) {
          this.context.moveTo(x, y)
          isFirst = false
        } else {
          this.context.lineTo(x, y)
        }
      }
    })
    this.context.stroke()
  }

  getLatestWeightStr(): string {
    const weights = this.weeklyWeightList.filter(v => v.value > 0)
    if (weights.length > 0) {
      return `${weights[weights.length - 1].value} kg`
    }
    return '无数据'
  }

  @Builder
  SummaryCard() {
    Column({ space: 14 }) {
      Text('本周健康摘要').fontSize(16).fontWeight(FontWeight.Bold).width('100%')

      this.StatRow('平均每日饮水', `${this.getAverage(this.weeklyWaterList)} ml`, '#007DFF')
      this.StatRow('平均每日摄入', `${this.getAverage(this.weeklyCaloriesList)} kcal`, '#FF9C6E')
      this.StatRow('平均每日运动', `${this.getAverage(this.weeklyExerciseList)} 分钟`, '#52C41A')

      this.StatRow('最新体重记录', this.getLatestWeightStr(), '#52C41A')
    }
    .padding(20).backgroundColor(Color.White).borderRadius(20)
    .shadow({ radius: 10, color: '#05000000' })
    .margin({ bottom: 20 })
  }

  @Builder
  StatRow(label: string, value: string, color: string) {
    Row() {
      Row() {
        Circle({ width: 6, height: 6 }).fill(color).margin({ right: 8 })
        Text(label).fontColor('#666').fontSize(14)
      }
      Blank()
      Text(value).fontWeight(FontWeight.Bold).fontSize(16)
    }.width('100%')
  }

  // 辅助计算函数
  getAverage(list: HealthChartData[]): number {
    const validData = list.filter(v => v.value > 0)
    if (validData.length === 0) return 0
    const sum = validData.reduce((s, x) => s + x.value, 0)
    return Math.round(sum / validData.length)
  }
}