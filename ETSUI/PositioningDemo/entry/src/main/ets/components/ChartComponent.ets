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
import { DataPoint } from '../model/SportModel';

// 分析结果接口定义
interface AnalysisResults {
  speedStability: string;
  speedStats: string[];
  milestoneTimes: string[];
  finalCalorie: string;
  riceBowlEquivalent: string;
  generalStats: string[];
}

@Component
export struct ChartComponent {
  @Prop data: DataPoint[];
  @Prop title: string;
  @Prop xLabel: string;
  @Prop yLabel: string;
  @Prop color: string;
  // 分析结果属性
  private analysisResults: AnalysisResults = {
    speedStability: '',
    speedStats: [],
    milestoneTimes: [],
    finalCalorie: '',
    riceBowlEquivalent: '',
    generalStats: []
  };

  // 组件即将显示时计算分析结果
  aboutToAppear() {
    // 输出数据内容log
    console.log(`${this.title} 数据点数量:`, this.data.length);
    if (this.data.length > 0) {
      console.log(`${this.title} 前5个数据点:`);
      for (let i = 0; i < Math.min(5, this.data.length); i++) {
        console.log(`  数据点${i + 1} - 时间戳: ${this.data[i].timestamp} 值: ${this.data[i].value}`);
      }
    }

    // 计算分析结果
    this.analysisResults = this.calculateAnalysisResults();
  }

  build() {
    Column() {
      Text(this.title)
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .margin({ bottom: 12 })

      // 数据分析结果容器
      Column() {
        if (this.data.length === 0) {
          // 无数据时显示
          Text('暂无数据')
            .fontSize(14)
            .fontColor('#999999')
            .textAlign(TextAlign.Center)
            .width('100%')
            .height(200)
        } else {
          // 显示数据分析结果
          this.renderAnalysisResults(this.analysisResults)
        }
      }
      .width('100%')
      .backgroundColor('#F5F5F5')
      .borderRadius(8)
      .padding(16)
    }
    .padding(16)
    .backgroundColor('#FFFFFF')
    .borderRadius(8)
    .shadow({
      color: '#00000010',
      radius: 4,
      offsetX: 0,
      offsetY: 2
    })
  }

  // 计算分析结果
  private calculateAnalysisResults(): AnalysisResults {
    const results: AnalysisResults = {
      speedStability: '',
      speedStats: [],
      milestoneTimes: [],
      finalCalorie: '',
      riceBowlEquivalent: '',
      generalStats: []
    };

    if (this.data.length === 0) {
      return results;
    }

    // 运动稳定性分析（如果是速度数据）
    if (this.title.includes('速度')) {
      const speeds = this.data.map(point => point.value);
      const meanSpeed = speeds.reduce((sum, speed) => sum + speed, 0) / speeds.length;
      const maxSpeed = Math.max(...speeds);
      const minSpeed = Math.min(...speeds);
      const variance = speeds.reduce((sum, speed) => sum + Math.pow(speed - meanSpeed, 2), 0) / speeds.length;
      const stdDev = Math.sqrt(variance);

      let stabilityLevel = '';
      if (stdDev < 0.5) {
        stabilityLevel = '非常稳定';
      } else if (stdDev < 1.0) {
        stabilityLevel = '比较稳定';
      } else {
        stabilityLevel = '不够稳定';
      }

      results.speedStability = `运动过程速度${stabilityLevel}，平均速度为${meanSpeed.toFixed(2)} m/s`;
      results.speedStats.push(`速度标准差为${stdDev.toFixed(2)}，数值越小表示运动越稳定`);
      results.speedStats.push(`运动过程中最高速度为${maxSpeed.toFixed(2)} m/s`);
      results.speedStats.push(`最低速度为${minSpeed.toFixed(2)} m/s`);
    }

    // 卡路里里程碑分析（如果是卡路里数据）
    if (this.title.includes('卡路里')) {
      const milestones = [5, 10, 15, 20, 25, 30];
      const startTime = this.data[0].timestamp;

      for (const milestone of milestones) {
        const milestonePoint = this.data.find(point => point.value >= milestone);
        if (milestonePoint) {
          const milestoneTime = Math.floor((milestonePoint.timestamp - startTime) / 1000);
          results.milestoneTimes.push(`到达${milestone}卡的时间为${milestoneTime}秒`);
        }
      }

      const finalCalorie = this.data[this.data.length - 1].value;
      results.finalCalorie = `运动结束时总消耗卡路里为${finalCalorie.toFixed(1)}卡`;

      // 计算相当于多少碗米饭的热量（假设一碗米饭约116千卡）
      const caloriesPerRiceBowl = 116;
      const riceBowls = finalCalorie / caloriesPerRiceBowl;
      results.riceBowlEquivalent = `消耗的卡路里相当于约${riceBowls.toFixed(2)}碗米饭的热量`;
    }

    // 通用数据统计
    const startTime = this.data[0].timestamp;
    const endTime = this.data[this.data.length - 1].timestamp;
    const duration = Math.floor((endTime - startTime) / 1000);
    const values = this.data.map(point => point.value);
    const meanValue = values.reduce((sum, value) => sum + value, 0) / values.length;
    const maxValue = Math.max(...values);
    const minValue = Math.min(...values);

    results.generalStats.push(`运动持续时间为${duration}秒`);
    results.generalStats.push(`共采集了${this.data.length}个数据点`);
    results.generalStats.push(`平均${this.yLabel.includes('速度') ? '速度' :
      '卡路里'}为${meanValue.toFixed(2)}${this.yLabel.includes('速度') ? ' m/s' : ' 卡'}`);
    results.generalStats.push(`${this.yLabel.includes('速度') ? '速度' :
      '卡路里'}范围：${minValue.toFixed(2)} - ${maxValue.toFixed(2)}${this.yLabel.includes('速度') ? ' m/s' : ' 卡'}`);

    return results;
  }

  @Builder
  renderAnalysisResults(analysisResults: AnalysisResults) {
    Column() {
      // 运动稳定性分析（如果是速度数据）
      if (this.title.includes('速度') && analysisResults.speedStability) {
        Text(analysisResults.speedStability)
          .fontSize(14)
          .margin({ bottom: 8 })

        ForEach(analysisResults.speedStats, (stat: string) => {
          Text(stat)
            .fontSize(14)
            .margin({ bottom: 8 })
        })
      }

      // 卡路里里程碑分析（如果是卡路里数据）
      if (this.title.includes('卡路里')) {
        if (analysisResults.milestoneTimes.length > 0) {
          ForEach(analysisResults.milestoneTimes, (milestoneTime: string) => {
            Text(milestoneTime)
              .fontSize(14)
              .margin({ bottom: 8 })
          })
        } else {
          Text('尚未达到任何卡路里里程碑')
            .fontSize(14)
            .margin({ bottom: 8 })
        }

        if (analysisResults.finalCalorie) {
          Text(analysisResults.finalCalorie)
            .fontSize(14)
            .margin({ bottom: 8 })
        }

        if (analysisResults.riceBowlEquivalent) {
          Text(analysisResults.riceBowlEquivalent)
            .fontSize(14)
            .margin({ bottom: 8 })
        }
      }

      // 通用数据统计
      ForEach(analysisResults.generalStats, (stat: string) => {
        Text(stat)
          .fontSize(14)
          .margin({ bottom: 8 })
      })
    }
  }
}