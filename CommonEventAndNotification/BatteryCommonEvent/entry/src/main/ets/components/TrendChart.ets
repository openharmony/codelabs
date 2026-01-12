/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
@Component
export struct TrendChart {
  values: number[] = [];
  label: string = 'SOC';
  heightPx: number = 140;

  private settings: RenderingContextSettings = new RenderingContextSettings(true);
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings);

  private canvasW: number = 0;
  private canvasH: number = 0;

  private redraw(): void {
    const w = this.canvasW;
    const h = this.canvasH;
    if (w <= 0 || h <= 0) {
      return;
    }

    this.context.clearRect(0, 0, w, h);

    const vs = this.values;
    if (!vs || vs.length < 2) {
      return;
    }

    let minV = vs[0];
    let maxV = vs[0];
    for (let i = 1; i < vs.length; i++) {
      const v = vs[i];
      if (v < minV) {
        minV = v;
      }
      if (v > maxV) {
        maxV = v;
      }
    }
    const span = Math.max(1e-6, maxV - minV);

    const padL = 10;
    const padR = 10;
    const padT = 10;
    const padB = 12;

    this.context.globalAlpha = 0.6;
    this.context.strokeStyle = '#999';
    this.context.lineWidth = 1;
    for (let i = 0; i <= 4; i++) {
      const y = padT + (h - padT - padB) * i / 4;
      this.context.beginPath();
      this.context.moveTo(padL, y);
      this.context.lineTo(w - padR, y);
      this.context.stroke();
    }
    this.context.globalAlpha = 1.0;

    this.context.strokeStyle = '#2E7DFF';
    this.context.lineWidth = 2;
    this.context.beginPath();
    for (let i = 0; i < vs.length; i++) {
      const x = padL + (w - padL - padR) * i / (vs.length - 1);
      const y = padT + (h - padT - padB) * (1 - (vs[i] - minV) / span);
      if (i === 0) {
        this.context.moveTo(x, y);
      } else {
        this.context.lineTo(x, y);
      }
    }
    this.context.stroke();

    this.context.fillStyle = '#666';
    this.context.font = '12px sans-serif';
    this.context.fillText(this.label, padL, 16);
  }

  build() {
    Column() {
      Canvas(this.context)
        .height(this.heightPx)
        .width('100%')
        .onReady(() => {
          this.canvasW = Number(this.context.width);
          this.canvasH = Number(this.context.height);
          this.redraw();
        })
        .onAreaChange(() => {
          this.canvasW = Number(this.context.width);
          this.canvasH = Number(this.context.height);
          this.redraw();
        });
    }
    .backgroundColor(0x08000000)
    .borderRadius(12);
  }
}
