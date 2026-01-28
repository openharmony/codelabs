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
// src/main/ets/pages/WaterTrendDialog.ets

export class WaterChartData {
  date: string;
  val: number;

  constructor(date: string, val: number) {
    this.date = date;
    this.val = val;
  }
}

@CustomDialog
export struct WaterTrendDialog {
  controller: CustomDialogController;

  @Prop data: WaterChartData[] = [];
  @Prop title: string = '近7天饮水趋势';

  // 1. 【新增】引入全局字体缩放比例
  @StorageProp('appFontScale') fontScale: number = 1.0;

  private settings: RenderingContextSettings = new RenderingContextSettings(true);
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings);

  build() {
    Column() {
      // 标题栏
      Text(this.title)
      // 2. 【修改】应用字体缩放
        .fontSize(18 * this.fontScale)
        .fontWeight(FontWeight.Bold)
        .margin({
          top: 20,
          bottom: 20
        })

      // 画布
      Canvas(this.context)
        .width('100%')
        .height(200) // 高度也可以考虑乘 fontScale，但固定高度通常为了布局稳定
        .onReady(() => {
          this.drawChart();
        })
        .margin({ bottom: 20 })

      // 关闭按钮
      Button('关闭')
        .backgroundColor('#F0F0F0')
        .fontColor('#666')
        // 按钮文字大小通常自带适应，但这里也可以强制指定
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

    // 清空画布
    this.context.clearRect(
      0,
      0,
      w,
      h
    );

    if (this.data.length === 0) return;

    // 计算最大值
    const maxVal = Math.max(
      2500,
      ...this.data.map(d => d.val)
    );
    const count = this.data.length;
    const stepX = (w - padding * 2) / count;
    const barWidth = stepX * 0.5;

    this.data.forEach((item, index) => {
      const val = item.val;
      const barHeight = (val / maxVal) * (h - bottomSpace - 20);

      const x = padding + index * stepX + (stepX - barWidth) / 2;
      const y = h - bottomSpace - barHeight;

      this.context.beginPath();

      if (val >= 2000) {
        this.context.fillStyle = '#007DFF';
      } else {
        this.context.fillStyle = '#87CEFA';
      }

      this.context.fillRect(
        x,
        y,
        barWidth,
        barHeight
      );

      // 3. 【修改】Canvas 内部文字大小缩放
      this.context.fillStyle = '#666';
      this.context.textAlign = 'center';
      // 动态拼接 font 字符串，例如 "12px sans-serif"
      this.context.font = `${10 * this.fontScale}px sans-serif`;

      // 绘制数值
      if (val > 0) {
        this.context.fillText(
          val.toString(),
          x + barWidth / 2,
          y - 5
        );
      }

      // 绘制日期
      let dateStr = item.date;
      if (dateStr.length > 5) {
        dateStr = dateStr.substring(5);
      }
      this.context.fillStyle = '#999';
      this.context.fillText(
        dateStr,
        x + barWidth / 2,
        h - 10
      );
    });

    // 底部横线
    this.context.beginPath();
    this.context.strokeStyle = '#EEE';
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