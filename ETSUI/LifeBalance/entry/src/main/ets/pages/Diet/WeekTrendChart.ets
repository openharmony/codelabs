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
// entry/src/main/ets/pages/Diet/WeekTrendChart.ets

// 1. 【修改】将 interface 改为 class，避免 ArkTS 严格模式下的对象字面量报错
export class WeeklyData {
  date: string;
  total: number;

  constructor(date: string, total: number) {
    this.date = date;
    this.total = total;
  }
}

@CustomDialog
export struct WeekTrendDialog {
  controller: CustomDialogController;
  @Prop weeklyData: WeeklyData[] = [];
  @Prop title: string = '近7天趋势';
  @Prop unit: string = '';
  // 2. 【新增】引入全局字体缩放比例
  @StorageProp('appFontScale') fontScale: number = 1.0;
  private settings: RenderingContextSettings = new RenderingContextSettings(true);
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings);

  build() {
    Column() {
      // 标题
      Text(this.title)
      // 3. 【修改】应用缩放
        .fontSize(18 * this.fontScale)
        .fontWeight(FontWeight.Bold)
        .margin({
          top: 20,
          bottom: 20
        })

      // 画布区域
      Canvas(this.context)
        .width('100%')
        .height(200)
        .onReady(() => {
          this.drawChart();
        })
        .margin({ bottom: 20 })

      // 关闭按钮
      Button('关闭')
        .backgroundColor('#F0F0F0')
        .fontColor('#666')
        // 4. 【修改】按钮文字缩放
        .fontSize(16 * this.fontScale)
        .width('80%')
        .onClick(() => {
          this.controller.close();
        })
        .margin({ bottom: 20 })
    }
    .backgroundColor(Color.White)
    .borderRadius(24)
    .width('90%')
  }

  drawChart() {
    const w = this.context.width;
    const h = this.context.height;
    const padding = 30;
    const bottomSpace = 30;

    this.context.clearRect(
      0,
      0,
      w,
      h
    );

    if (this.weeklyData.length === 0) {
      return;
    }

    // 默认最大值逻辑
    const defaultMax = this.title.includes('运动') ? 500 : 2500;
    const maxVal = Math.max(defaultMax, ...this.weeklyData.map(d => d.total));

    const count = this.weeklyData.length;
    const stepX = (w - padding * 2) / count;
    const barWidth = stepX * 0.6;

    this.weeklyData.forEach((item, index) => {
      const val = item.total;
      const barHeight = (val / maxVal) * (h - bottomSpace - 20);

      const x = padding + index * stepX + (stepX - barWidth) / 2;
      const y = h - bottomSpace - barHeight;

      this.context.beginPath();

      // 颜色逻辑
      if (this.title.includes('摄入') && val > 2000) {
        this.context.fillStyle = '#FF4D4F'; // 红
      } else if (this.title.includes('运动')) {
        this.context.fillStyle = '#52C41A'; // 绿
      } else {
        this.context.fillStyle = '#007DFF'; // 蓝
      }

      this.context.fillRect(
        x,
        y,
        barWidth,
        barHeight
      );

      // 5. 【修改】Canvas 内部文字大小缩放
      this.context.fillStyle = '#666';
      this.context.textAlign = 'center';
      // 动态计算字体大小
      this.context.font = `${12 * this.fontScale}px sans-serif`;

      if (val > 0) {
        this.context.fillText(
          val.toString(),
          x + barWidth / 2,
          y - 5
        );
      }

      let shortDate = item.date;
      if (item.date.length > 5) {
        shortDate = item.date.substring(5);
      }

      this.context.fillStyle = '#999';
      this.context.fillText(
        shortDate,
        x + barWidth / 2,
        h - 10
      );
    });

    this.context.beginPath();
    this.context.strokeStyle = '#EEE';
    this.context.lineWidth = 1;
    this.context.moveTo(
      padding,
      h - bottomSpace
    );
    this.context.lineTo(
      w - padding,
      h - bottomSpace
    );
    this.context.stroke();
  }
}