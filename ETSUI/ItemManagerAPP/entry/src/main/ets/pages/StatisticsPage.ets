/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
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
import { Item, ItemUseStatus, DEFAULT_CATEGORY_OPTIONS } from '../model/Item';
import curves from '@ohos.curves';

/**
 * ====================================================================================
 * CONFIG & UTILS & TYPES
 * ====================================================================================
 */

/**
 * 颜色配置中心
 */
class ThemeConfig {
  static readonly PRIMARY = "#4A6CF7";
  static readonly PRIMARY_DARK = "#3A5CCC";
  static readonly ACCENT = "#FFD700";
  static readonly BACKGROUND = "#F5F7FA";
  static readonly CARD_BG = "#FFFFFF";
  static readonly TEXT_MAIN = "#1A1A1A";
  static readonly TEXT_SECONDARY = "#666666";
  static readonly TEXT_LIGHT = "#999999";
  static readonly DANGER = "#FF3B30";
  static readonly SUCCESS = "#34C759";
  static readonly WARNING = "#FF9500";

  static readonly RAINBOW_COLORS: string[] = [
    "#FF3B30", "#FF9500", "#FFCC00", "#34C759", "#30B0C7",
    "#007AFF", "#5856D6", "#AF52DE", "#FF2D55", "#A2845E",
    "#4A6CF7", "#3A5CCC", "#2AC759", "#1A1A1A", "#666666"
  ];

  static readonly STATUS_COLORS: Record<string, string> = {
    "在用": "#34C759", // Green
    "闲置": "#FF9500", // Orange
    "已丢弃": "#8E8E93"  // Gray
  };

  // 图表专用渐变色
  static readonly CHART_GRADIENT = [
    "rgba(74, 108, 247, 0.5)",
    "rgba(74, 108, 247, 0.0)"
  ];

  // 雷达图颜色
  static readonly RADAR_FILL = "rgba(74, 108, 247, 0.3)";
  static readonly RADAR_STROKE = "#4A6CF7";
}

/**
 * 辅助类定义
 */
class ControlPoints {
  p1x: number;
  p1y: number;
  p2x: number;
  p2y: number;

  constructor(p1x: number, p1y: number, p2x: number, p2y: number) {
    this.p1x = p1x;
    this.p1y = p1y;
    this.p2x = p2x;
    this.p2y = p2y;
  }
}

class ChartPoint {
  x: number;
  y: number;

  constructor(x: number, y: number) {
    this.x = x;
    this.y = y;
  }
}

/**
 * 工具类
 */
class ChartUtils {
  // 生成贝塞尔曲线控制点
  static getControlPoints(x1: number, y1: number, x2: number, y2: number, x3?: number, y3?: number): ControlPoints {
    const tension = 0.3;
    const d1 = Math.sqrt(Math.pow(x2 - x1, 2) + Math.pow(y2 - y1, 2));
    const d2 = (x3 !== undefined && y3 !== undefined) ? Math.sqrt(Math.pow(x3 - x2, 2) + Math.pow(y3 - y2, 2)) : 0;

    const fa = tension * d1 / (d1 + d2);
    const fb = tension * d2 / (d1 + d2);

    const p1x = x2 - fa * (x3 !== undefined ? (x3 - x1) : (x2 - x1));
    const p1y = y2 - fa * ((x3 !== undefined && y3 !== undefined) ? (y3 - y1) : (y2 - y1));
    const p2x = x2 + fb * (x3 !== undefined ? (x3 - x1) : (x2 - x1));
    const p2y = y2 + fb * ((x3 !== undefined && y3 !== undefined) ? (y3 - y1) : (y2 - y1));

    return new ControlPoints(p1x, p1y, p2x, p2y);
  }

  static formatCurrency(val: number): string {
    if (val >= 10000) return (val / 10000).toFixed(1) + "w";
    if (val >= 1000) return (val / 1000).toFixed(1) + "k";
    return val.toFixed(0);
  }

  static getMonthKey(dateStr: string): string {
    if (!dateStr) return "Unknown";
    try {
      const d = new Date(dateStr);
      return `${d.getFullYear()}-${(d.getMonth() + 1).toString().padStart(2, '0')}`;
    } catch (e) {
      return "Unknown";
    }
  }

  // 极坐标转直角坐标
  static polarToCartesian(centerX: number, centerY: number, radius: number, angleInDegrees: number): ChartPoint {
    const angleInRadians = (angleInDegrees - 90) * Math.PI / 180.0;
    return new ChartPoint(
      centerX + (radius * Math.cos(angleInRadians)),
      centerY + (radius * Math.sin(angleInRadians))
    );
  }
}

/**
 * ====================================================================================
 * DATA MODELS (所有类都添加了构造函数)
 * ====================================================================================
 */

class IdleStatsResult {
  value: number;
  count: number;

  constructor(value: number, count: number) {
    this.value = value;
    this.count = count;
  }
}

class CategoryStat {
  name: string;
  count: number;
  totalPrice: number;
  percentage: number;
  color: string;

  constructor(name: string, count: number = 0, totalPrice: number = 0, percentage: number = 0, color: string = "#DDD") {
    this.name = name;
    this.count = count;
    this.totalPrice = totalPrice;
    this.percentage = percentage;
    this.color = color;
  }
}

class StatusStat {
  status: ItemUseStatus;
  count: number;
  percentage: number;
  color: string;
  totalPrice: number;

  constructor(status: ItemUseStatus, count: number, percentage: number, color: string, totalPrice: number) {
    this.status = status;
    this.count = count;
    this.percentage = percentage;
    this.color = color;
    this.totalPrice = totalPrice;
  }
}

class PositionStat {
  name: string;
  count: number;
  fontSize: number;
  fontWeight: number;
  color: string;
  opacity: number;

  constructor(name: string, count: number, fontSize: number, fontWeight: number, color: string, opacity: number) {
    this.name = name;
    this.count = count;
    this.fontSize = fontSize;
    this.fontWeight = fontWeight;
    this.color = color;
    this.opacity = opacity;
  }
}

class TrendPoint {
  label: string;
  fullDate: string;
  value: number;
  x: number;
  y: number;

  constructor(label: string, fullDate: string, value: number, x: number = 0, y: number = 0) {
    this.label = label;
    this.fullDate = fullDate;
    this.value = value;
    this.x = x;
    this.y = y;
  }
}

class RadarIndicator {
  name: string;
  max: number;
  value: number;

  constructor(name: string, value: number, max: number = 100) {
    this.name = name;
    this.value = value;
    this.max = max;
  }
}

class HistogramBin {
  rangeLabel: string;
  min: number;
  max: number;
  count: number;
  value: number;

  constructor(rangeLabel: string, min: number, max: number, count: number, value: number) {
    this.rangeLabel = rangeLabel;
    this.min = min;
    this.max = max;
    this.count = count;
    this.value = value;
  }
}

class StackedBarData {
  category: string;
  usedCount: number;
  idleCount: number;
  wasteCount: number;
  total: number;

  constructor(category: string, usedCount: number, idleCount: number, wasteCount: number, total: number) {
    this.category = category;
    this.usedCount = usedCount;
    this.idleCount = idleCount;
    this.wasteCount = wasteCount;
    this.total = total;
  }
}

/**
 * ====================================================================================
 * BASE COMPONENTS
 * ====================================================================================
 */

class BubbleParticle {
  x: number;
  y: number;
  radius: number;
  speedY: number;
  opacity: number;

  constructor(width: number, height: number) {
    this.x = Math.random() * width;
    this.y = height + Math.random() * 20;
    this.radius = Math.random() * 10 + 5;
    this.speedY = Math.random() * 0.5 + 0.2;
    this.opacity = Math.random() * 0.3 + 0.1;
  }

  update(height: number) {
    this.y -= this.speedY;
    if (this.y < -20) {
      this.y = height + 20;
      this.x = Math.random() * 300;
    }
  }
}

@Component
struct BaseCard {
  @BuilderParam content: () => void = this.emptyBuilder;
  @Prop title: string = "";
  @Prop subTitle: string = "";
  @Prop showArrow: boolean = false;
  onHeaderClick: () => void = () => {};

  @Builder emptyBuilder() {}

  build() {
    Column() {
      if (this.title) {
        Row() {
          Column() {
            Text(this.title)
              .fontSize(18)
              .fontWeight(FontWeight.Bold)
              .fontColor(ThemeConfig.TEXT_MAIN)
            if (this.subTitle) {
              Text(this.subTitle)
                .fontSize(12)
                .fontColor(ThemeConfig.TEXT_LIGHT)
                .margin({ top: 4 })
            }
          }
          .alignItems(HorizontalAlign.Start)
          .layoutWeight(1)

          if (this.showArrow) {
            Text("Details >")
              .fontSize(12)
              .fontColor(ThemeConfig.PRIMARY)
          }
        }
        .width("100%")
        .margin({ bottom: 20 })
        .onClick(() => this.onHeaderClick())
      }

      this.content()
    }
    .width("100%")
    .backgroundColor(ThemeConfig.CARD_BG)
    .borderRadius(16)
    .padding(20)
    .shadow({
      radius: 12,
      color: "rgba(0,0,0,0.05)",
      offsetY: 4
    })
    .margin({ bottom: 16 })
  }
}

@Component
struct AssetHeader {
  @Prop totalValue: number = 0;
  @Prop totalCount: number = 0;

  private settings: RenderingContextSettings = new RenderingContextSettings(true);
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings);
  private particles: BubbleParticle[] = [];
  private timerId: number = -1;
  private canvasWidth: number = 300;
  private canvasHeight: number = 150;

  aboutToAppear() {
    for (let i = 0; i < 15; i++) {
      this.particles.push(new BubbleParticle(this.canvasWidth, this.canvasHeight));
    }
  }

  aboutToDisappear() {
    if (this.timerId !== -1) clearInterval(this.timerId);
  }

  startAnimation() {
    if (this.timerId !== -1) clearInterval(this.timerId);
    this.timerId = setInterval(() => {
      this.drawParticles();
    }, 20);
  }

  drawParticles() {
    this.context.clearRect(0, 0, this.canvasWidth, this.canvasHeight);
    this.particles.forEach(p => {
      p.update(this.canvasHeight);
      this.context.beginPath();
      this.context.arc(p.x, p.y, p.radius, 0, Math.PI * 2);
      this.context.fillStyle = `rgba(255, 255, 255, ${p.opacity})`;
      this.context.fill();
    });
  }

  build() {
    Stack() {
      Canvas(this.context)
        .width("100%")
        .height("100%")
        .onReady(() => {
          this.canvasWidth = this.context.width;
          this.canvasHeight = this.context.height;
          this.startAnimation();
        })
        .hitTestBehavior(HitTestMode.None)

      Column()
        .width("100%")
        .height("100%")
        .linearGradient({
          angle: 135,
          colors: [[ThemeConfig.PRIMARY, 0.0], [ThemeConfig.PRIMARY_DARK, 1.0]]
        })
        .opacity(0.9)

      Row() {
        Column() {
          Text("资产总值")
            .fontSize(14)
            .fontColor("rgba(255,255,255,0.8)")
            .margin({ bottom: 8 })

          Row() {
            Text("¥")
              .fontSize(20)
              .fontColor(Color.White)
              .margin({ top: 8, right: 4 })
              .baselineOffset(0)
            Text(this.totalValue.toFixed(2))
              .fontSize(36)
              .fontWeight(FontWeight.Bold)
              .fontColor(Color.White)
          }
          .alignItems(VerticalAlign.Center)
        }
        .alignItems(HorizontalAlign.Start)
        .layoutWeight(1)

        Divider()
          .vertical(true)
          .height(40)
          .color("rgba(255,255,255,0.2)")
          .margin({ left: 20, right: 20 })

        Column() {
          Text("物品总数")
            .fontSize(14)
            .fontColor("rgba(255,255,255,0.8)")
            .margin({ bottom: 8 })
          Text(`${this.totalCount}`)
            .fontSize(32)
            .fontWeight(FontWeight.Medium)
            .fontColor(Color.White)
        }
        .alignItems(HorizontalAlign.End)
      }
      .padding({ left: 24, right: 24, top: 30, bottom: 30 })
    }
    .width("100%")
    .height(160)
    .borderRadius(24)
    .clip(true)
    .shadow({ radius: 20, color: "#404A6CF7", offsetY: 10 })
    .margin({ bottom: 24 })
  }
}

/**
 * ====================================================================================
 * ADVANCED VISUALIZATIONS - 1. RADAR CHART (雷达图)
 * ====================================================================================
 */
@Component
struct RadarChart {
  @Prop indicators: RadarIndicator[];

  private settings: RenderingContextSettings = new RenderingContextSettings(true);
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings);
  private chartSize: number = 0;
  private centerX: number = 0;
  private centerY: number = 0;
  private radius: number = 0;

  @State animProgress: number = 0;

  build() {
    BaseCard({ title: "多维分析", subTitle: "从不同维度评估你的消费习惯" }) {
      Column() {
        Canvas(this.context)
          .width("100%")
          .height(300)
          .onReady(() => {
            this.chartSize = this.context.width;
            this.centerX = this.chartSize / 2;
            this.centerY = 150; // height/2
            this.radius = 100;
            this.startAnimation();
          })
      }
    }
  }

  startAnimation() {
    this.animProgress = 0;
    animateTo({ duration: 1500, curve: Curve.FastOutSlowIn }, () => {
      this.animProgress = 1;
    });

    let start = Date.now();
    let frame = () => {
      let now = Date.now();
      let p = Math.min(1, (now - start) / 1000);
      let easeP = 1 - Math.pow(1 - p, 3);
      this.draw(easeP);
      if (p < 1) setTimeout(frame, 16);
    };
    frame();
  }

  draw(progress: number) {
    const ctx = this.context;
    ctx.clearRect(0, 0, this.chartSize, 300);

    const count = this.indicators.length;
    if (count < 3) return;

    const angleSlice = 360 / count;

    // 1. 绘制网格背景 (5层)
    ctx.lineWidth = 1;
    ctx.strokeStyle = "#E0E0E0";
    ctx.fillStyle = "#F9F9F9";

    for (let i = 5; i > 0; i--) {
      const levelRadius = this.radius * (i / 5);
      ctx.beginPath();
      for (let j = 0; j < count; j++) {
        const pt = ChartUtils.polarToCartesian(this.centerX, this.centerY, levelRadius, j * angleSlice);
        if (j === 0) ctx.moveTo(pt.x, pt.y);
        else ctx.lineTo(pt.x, pt.y);
      }
      ctx.closePath();
      if (i === 5) {
        ctx.fill(); // 最外层填充背景
      }
      ctx.stroke();
    }

    // 2. 绘制轴线
    ctx.strokeStyle = "#E0E0E0";
    for (let j = 0; j < count; j++) {
      const pt = ChartUtils.polarToCartesian(this.centerX, this.centerY, this.radius, j * angleSlice);
      ctx.beginPath();
      ctx.moveTo(this.centerX, this.centerY);
      ctx.lineTo(pt.x, pt.y);
      ctx.stroke();
    }

    // 3. 绘制数据区域
    ctx.beginPath();
    const dataPoints: ChartPoint[] = [];
    for (let j = 0; j < count; j++) {
      const indicator = this.indicators[j];
      const ratio = (indicator.value / indicator.max) * progress; // 应用动画
      const valRadius = this.radius * Math.min(1.1, Math.max(0, ratio)); // 限制在范围内
      const pt = ChartUtils.polarToCartesian(this.centerX, this.centerY, valRadius, j * angleSlice);
      dataPoints.push(pt);
      if (j === 0) ctx.moveTo(pt.x, pt.y);
      else ctx.lineTo(pt.x, pt.y);
    }
    ctx.closePath();
    ctx.fillStyle = ThemeConfig.RADAR_FILL;
    ctx.fill();
    ctx.lineWidth = 2;
    ctx.strokeStyle = ThemeConfig.RADAR_STROKE;
    ctx.stroke();

    // 4. 绘制数据点
    ctx.fillStyle = ThemeConfig.PRIMARY;
    dataPoints.forEach(pt => {
      ctx.beginPath();
      ctx.arc(pt.x, pt.y, 4, 0, Math.PI * 2);
      ctx.fill();
    });

    // 5. 绘制标签
    ctx.font = "40px sans-serif";
    ctx.fillStyle = ThemeConfig.TEXT_SECONDARY;
    ctx.textAlign = "center";
    ctx.textBaseline = "middle";

    for (let j = 0; j < count; j++) {
      const pt = ChartUtils.polarToCartesian(this.centerX, this.centerY, this.radius + 20, j * angleSlice);
      ctx.fillText(this.indicators[j].name, pt.x, pt.y);
    }
  }
}

/**
 * ====================================================================================
 * ADVANCED VISUALIZATIONS - 2. HISTOGRAM (价格分布直方图)
 * ====================================================================================
 */
@Component
struct PriceHistogram {
  @Prop bins: HistogramBin[];

  private settings: RenderingContextSettings = new RenderingContextSettings(true);
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings);
  private w: number = 0;
  private h: number = 0;
  @State touchedIndex: number = -1;

  build() {
    BaseCard({ title: "价格分布", subTitle: "物品价格区间统计" }) {
      Column() {
        Canvas(this.context)
          .width("100%")
          .height(220)
          .onReady(() => {
            this.w = this.context.width;
            this.h = this.context.height;
            this.draw();
          })
          .onTouch((e) => this.handleTouch(e))
      }
    }
  }

  handleTouch(e: TouchEvent) {
    if (e.type === TouchType.Up || e.type === TouchType.Cancel) {
      this.touchedIndex = -1;
      this.draw();
      return;
    }

    const x = e.touches[0].x;
    const padding = 30;
    const chartW = this.w - padding * 2;
    const barWidth = chartW / this.bins.length;

    // 计算点击了哪个柱子
    if (x > padding && x < this.w - padding) {
      const idx = Math.floor((x - padding) / barWidth);
      if (idx >= 0 && idx < this.bins.length) {
        this.touchedIndex = idx;
        this.draw();
      }
    }
  }

  draw() {
    const ctx = this.context;
    ctx.clearRect(0, 0, this.w, this.h);

    if (this.bins.length === 0) return;

    const padding = 30;
    const chartH = this.h - padding * 2;
    const chartW = this.w - padding * 2;
    const maxCount = Math.max(...this.bins.map(b => b.count), 5); // 最小刻度5

    const barWidth = (chartW / this.bins.length) * 0.8; // 柱子宽度占80%
    const gap = (chartW / this.bins.length) * 0.2;

    // 绘制坐标轴
    ctx.strokeStyle = "#EEE";
    ctx.lineWidth = 1;
    // Y轴线 (3条)
    ctx.font = "24px sans-serif";
    ctx.textAlign = "right";
    ctx.fillStyle = ThemeConfig.TEXT_LIGHT;
    for(let i=0; i<=4; i++) {
      const y = padding + chartH - (chartH * (i/4));
      ctx.beginPath();
      ctx.moveTo(padding, y);
      ctx.lineTo(this.w - padding, y);
      ctx.stroke();
      ctx.fillText(Math.round(maxCount * (i/4)).toString(), padding - 5, y + 3);
    }

    // 绘制柱子
    this.bins.forEach((bin, i) => {
      const x = padding + (i * (chartW / this.bins.length)) + gap/2;
      const barH = (bin.count / maxCount) * chartH;
      const y = padding + chartH - barH;

      // 柱体
      ctx.fillStyle = (i === this.touchedIndex) ? ThemeConfig.ACCENT : ThemeConfig.PRIMARY;

      // 绘制圆角柱顶 (简化为矩形)
      ctx.fillRect(x, y, barWidth, barH);

      // 底部标签
      ctx.font = "24px sans-serif"; // 新增这行
      ctx.textAlign = "center";
      ctx.fillStyle = ThemeConfig.TEXT_SECONDARY;
      // 简略显示标签，防止重叠
      if (this.bins.length > 8 && i % 2 !== 0) {
        // skip
      } else {
        ctx.fillText(bin.rangeLabel.split('-')[0], x + barWidth/2, this.h - 10);
      }

      // 选中时显示数值Tooltip
      if (i === this.touchedIndex) {
        const text = `${bin.count}件`;
        const tw = ctx.measureText(text).width + 10;
        ctx.fillStyle = ThemeConfig.TEXT_MAIN;
        ctx.fillRect(x + barWidth/2 - tw/2, y - 25, tw, 20);
        ctx.fillStyle = Color.White;
        ctx.fillText(text, x + barWidth/2, y - 11);
      }
    });
  }
}

/**
 * ====================================================================================
 * ADVANCED VISUALIZATIONS - 3. STACKED BAR CHART (分类堆叠对比图)
 * ====================================================================================
 */
@Component
struct StackedBarChart {
  @Prop data: StackedBarData[];

  private settings: RenderingContextSettings = new RenderingContextSettings(true);
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings);
  private w: number = 0;
  private h: number = 0;

  build() {
    BaseCard({ title: "分类健康度", subTitle: "各分类下的在用/闲置对比" }) {
      Column() {
        // 图例
        Row() {
          this.LegendItem("在用", ThemeConfig.SUCCESS)
          this.LegendItem("闲置", ThemeConfig.WARNING)
          this.LegendItem("已丢弃", ThemeConfig.TEXT_LIGHT)
        }
        .width("100%")
        .justifyContent(FlexAlign.End)
        .margin({ bottom: 10 })

        Canvas(this.context)
          .width("100%")
          .height(250)
          .onReady(() => {
            this.w = this.context.width;
            this.h = this.context.height;
            this.draw();
          })
      }
    }
  }

  @Builder LegendItem(label: string, color: string) {
    Row() {
      Circle({ width: 8, height: 8 }).fill(color).margin({ right: 4 })
      Text(label).fontSize(12).fontColor(ThemeConfig.TEXT_SECONDARY)
    }
    .margin({ left: 12 })
  }

  draw() {
    const ctx = this.context;
    ctx.clearRect(0, 0, this.w, this.h);

    if (this.data.length === 0) return;

    // 排序取前6个主要分类
    const displayData = this.data.sort((a,b) => b.total - a.total).slice(0, 6);

    const paddingLeft = 40;
    const paddingBottom = 30;
    const paddingRight = 10;
    const chartW = this.w - paddingLeft - paddingRight;
    const chartH = this.h - paddingBottom;

    const maxTotal = Math.max(...displayData.map(d => d.total));
    const barHeight = (chartH / displayData.length) * 0.6; // 柱子高度
    const gap = (chartH / displayData.length) * 0.4;

    // 绘制
    displayData.forEach((d, i) => {
      const y = i * (barHeight + gap);

      // 绘制分类名
      ctx.textAlign = "right";
      ctx.textBaseline = "middle";
      ctx.fillStyle = ThemeConfig.TEXT_SECONDARY;
      ctx.font = "32px sans-serif";
      ctx.fillText(d.category, paddingLeft - 5, y + barHeight/2);

      // 计算各部分宽度
      const wUsed = (d.usedCount / maxTotal) * chartW;
      const wIdle = (d.idleCount / maxTotal) * chartW;
      const wWaste = (d.wasteCount / maxTotal) * chartW;

      let currentX = paddingLeft;

      // 绘制"在用"
      if (wUsed > 0) {
        ctx.fillStyle = ThemeConfig.SUCCESS;
        ctx.fillRect(currentX, y, wUsed, barHeight);
        currentX += wUsed;
      }
      // 绘制"闲置"
      if (wIdle > 0) {
        ctx.fillStyle = ThemeConfig.WARNING;
        ctx.fillRect(currentX, y, wIdle, barHeight);
        currentX += wIdle;
      }
      // 绘制"丢弃"
      if (wWaste > 0) {
        ctx.fillStyle = ThemeConfig.TEXT_LIGHT;
        ctx.fillRect(currentX, y, wWaste, barHeight);
        currentX += wWaste;
      }

      // 绘制总数标签
      ctx.textAlign = "left";
      ctx.fillStyle = ThemeConfig.TEXT_LIGHT;
      ctx.fillText(d.total.toString(), currentX + 5, y + barHeight/2);
    });
  }
}

/**
 * ====================================================================================
 * EXISTING COMPONENTS (优化版)
 * ====================================================================================
 */

@Component
struct CategoryLegendItem {
  @Prop stat: CategoryStat;
  @Prop isSelected: boolean = false;
  onClickItem: () => void = () => {};

  build() {
    Row() {
      Circle({ width: 10, height: 10 })
        .fill(this.stat.color)
        .margin({ right: 10 })

      Text(this.stat.name)
        .fontSize(14)
        .fontColor(this.isSelected ? ThemeConfig.PRIMARY : ThemeConfig.TEXT_SECONDARY)
        .fontWeight(this.isSelected ? FontWeight.Bold : FontWeight.Normal)
        .layoutWeight(1)

      Text(`${this.stat.count}件`)
        .fontSize(13)
        .fontColor(ThemeConfig.TEXT_MAIN)
        .margin({ right: 8 })

      Text(`${this.stat.percentage.toFixed(0)}%`)
        .fontSize(12)
        .fontColor(ThemeConfig.TEXT_LIGHT)
        .width(36)
        .textAlign(TextAlign.End)
    }
    .width("100%")
    .padding({ top: 8, bottom: 8 })
    .backgroundColor(this.isSelected ? "#F0F5FF" : Color.Transparent)
    .borderRadius(4)
    .onClick(() => this.onClickItem())
  }
}

@Component
struct InteractiveWordCloud {
  @Prop stats: PositionStat[];
  @Prop selectedLocation: string;
  onLocationSelect: (loc: string) => void = () => {};

  build() {
    Flex({ wrap: FlexWrap.Wrap, justifyContent: FlexAlign.Start, alignContent: FlexAlign.Start }) {
      ForEach(this.stats, (stat: PositionStat) => {
        Text(stat.name)
          .fontSize(stat.fontSize)
          .fontWeight(stat.name === this.selectedLocation ? FontWeight.Bold : stat.fontWeight)
          .fontColor(stat.name === this.selectedLocation ? Color.White : stat.color)
          .backgroundColor(stat.name === this.selectedLocation ? stat.color : "#F5F7FA")
          .padding({ left: 12, right: 12, top: 6, bottom: 6 })
          .borderRadius(20)
          .margin({ right: 8, bottom: 8 })
          .animation({ duration: 300, curve: curves.springMotion() })
          .onClick(() => {
            if (this.selectedLocation === stat.name) {
              this.onLocationSelect("");
            } else {
              this.onLocationSelect(stat.name);
            }
          })
      })

      if (this.selectedLocation) {
        Row() {
          Text("清除筛选 X")
            .fontSize(12)
            .fontColor(ThemeConfig.TEXT_LIGHT)
        }
        .padding({ left: 12, right: 12, top: 6, bottom: 6 })
        .backgroundColor("transparent")
        .border({ width: 1, color: "#E0E0E0", radius: 20 })
        .margin({ bottom: 8 })
        .onClick(() => this.onLocationSelect(""))
      }
    }
    .width("100%")
  }
}

@Component
struct ComplexItemCard {
  @Prop item: Item;

  private formatDate(dateStr?: string): string {
    if (!dateStr) return "未知日期";
    try {
      const d = new Date(dateStr);
      return `${d.getFullYear()}-${(d.getMonth() + 1).toString().padStart(2, '0')}-${d.getDate().toString().padStart(2, '0')}`;
    } catch (e) {
      return dateStr;
    }
  }

  build() {
    Row() {
      Column()
        .width(4)
        .height(50)
        .backgroundColor(ThemeConfig.STATUS_COLORS[this.item.useStatus] || "#999")
        .borderRadius(2)
        .margin({ right: 12 })

      Column() {
        Row() {
          Text(this.item.name)
            .fontSize(16)
            .fontWeight(FontWeight.Bold)
            .fontColor(ThemeConfig.TEXT_MAIN)
            .layoutWeight(1)
            .maxLines(1)
            .textOverflow({ overflow: TextOverflow.Ellipsis })

          Text(`¥${this.item.price.toFixed(0)}`)
            .fontSize(16)
            .fontWeight(FontWeight.Bold)
            .fontColor(ThemeConfig.PRIMARY)
        }
        .width("100%")
        .margin({ bottom: 8 })

        Row() {
          Text(this.item.category)
            .fontSize(10)
            .fontColor(Color.White)
            .backgroundColor("#B0B0B0")
            .padding({ left: 6, right: 6, top: 2, bottom: 2 })
            .borderRadius(4)
            .margin({ right: 8 })

          Text(this.item.location || "未归位")
            .fontSize(12)
            .fontColor(ThemeConfig.TEXT_SECONDARY)
            .margin({ right: 12 })

          Text(this.formatDate(this.item.buyDate))
            .fontSize(12)
            .fontColor(ThemeConfig.TEXT_LIGHT)
        }
      }
      .layoutWeight(1)
      .alignItems(HorizontalAlign.Start)
    }
    .width("100%")
    .padding(16)
    .backgroundColor(Color.White)
    .borderRadius(12)
    .margin({ bottom: 12 })
    .shadow({ radius: 8, color: "rgba(0,0,0,0.03)", offsetY: 2 })
  }
}


@Component
struct IdleAnalysisCard {
  @Prop totalValue: number = 0;
  @Prop idleValue: number = 0;
  @Prop idleCount: number = 0;

  private getWasteLevel(): string {
    const ratio = this.totalValue > 0 ? this.idleValue / this.totalValue : 0;
    if (ratio < 0.1) return "极简大师";
    if (ratio < 0.3) return "理性消费";
    if (ratio < 0.5) return "有点浪费";
    return "囤积达人";
  }

  build() {
    BaseCard({ title: "闲置成本分析", subTitle: "发现被遗忘的价值" }) {
      Column() {
        Row() {
          Column() {
            Text("闲置总额")
              .fontSize(12)
              .fontColor(ThemeConfig.TEXT_LIGHT)
            Text(`¥${this.idleValue.toFixed(0)}`)
              .fontSize(32)
              .fontWeight(FontWeight.Bold)
              .fontColor(ThemeConfig.DANGER)
              .margin({ top: 4, bottom: 8 })

            Text(`约占总资产 ${(this.totalValue > 0 ? (this.idleValue / this.totalValue * 100) : 0).toFixed(1)}%`)
              .fontSize(12)
              .fontColor(ThemeConfig.TEXT_SECONDARY)
              .backgroundColor("#FFF0F0")
              .padding({ left: 8, right: 8, top: 4, bottom: 4 })
              .borderRadius(4)
          }
          .alignItems(HorizontalAlign.Start)
          .layoutWeight(1)

          Column() {
            Stack() {
              Circle({ width: 80, height: 80 })
                .strokeWidth(6)
                .stroke(ThemeConfig.BACKGROUND)
                .fill(Color.Transparent)

              Circle({ width: 80, height: 80 })
                .strokeWidth(6)
                .stroke(ThemeConfig.DANGER)
                .strokeDashArray([this.idleValue > 0 ? 100 : 0, 100])
                .fill(Color.Transparent)
                .rotate({ angle: -90 })
                .opacity(0.6)

              Column() {
                Text(this.getWasteLevel())
                  .fontSize(14)
                  .fontWeight(FontWeight.Bold)
                  .fontColor(ThemeConfig.DANGER)
                Text(`${this.idleCount}件闲置`)
                  .fontSize(10)
                  .fontColor(ThemeConfig.TEXT_LIGHT)
                  .margin({ top: 2 })
              }
            }
          }
        }
        .width("100%")
        .margin({ bottom: 20 })

        Row() {
          Text(this.idleValue > 1000 ? "💡 建议处理高价闲置物品，可回血约 ¥" + (this.idleValue * 0.6).toFixed(0) : "✨ 保持得很好，继续保持高利用率！")
            .fontSize(12)
            .fontColor(ThemeConfig.PRIMARY)
            .layoutWeight(1)
        }
        .padding(12)
        .backgroundColor("#F0F5FF")
        .borderRadius(8)
        .width("100%")
      }
    }
  }
}

@Component
struct TrendChart {
  @Prop trendData: TrendPoint[];
  @Prop selectedMonth: string;
  onMonthSelect: (monthKey: string) => void = () => {};

  private settings: RenderingContextSettings = new RenderingContextSettings(true);
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings);
  private chartWidth: number = 0;
  private chartHeight: number = 0;
  private paddingLeft: number = 40;
  private paddingBottom: number = 30;
  private paddingTop: number = 20;
  private paddingRight: number = 20;

  @State animProgress: number = 0;
  @State touchedPointIndex: number = -1;

  build() {
    BaseCard({
      title: "消费趋势",
      subTitle: this.selectedMonth ? `筛选月份: ${this.selectedMonth}` : "近6个月支出走势 (点击节点筛选)",
      showArrow: false
    }) {
      Column() {
        Canvas(this.context)
          .width("100%")
          .height(200)
          .backgroundColor(Color.Transparent)
          .onReady(() => {
            this.chartWidth = this.context.width;
            this.chartHeight = this.context.height;
            this.startEnterAnimation();
          })
          .onTouch((event: TouchEvent) => {
            this.handleTouch(event);
          })
      }
    }
  }

  startEnterAnimation() {
    this.animProgress = 0;
    animateTo({ duration: 1000, curve: Curve.FastOutSlowIn, onFinish: () => {} }, () => {
      this.animProgress = 1;
    })

    let start = Date.now();
    let frame = () => {
      let now = Date.now();
      let p = Math.min(1, (now - start) / 1000);
      let easeP = 1 - Math.pow(1 - p, 3);
      this.drawChart(easeP);
      if (p < 1) {
        setTimeout(frame, 16);
      }
    };
    frame();
  }

  drawChart(progress: number) {
    if (this.trendData.length === 0) return;

    const ctx = this.context;
    const w = this.chartWidth;
    const h = this.chartHeight;
    const pL = this.paddingLeft;
    const pB = this.paddingBottom;
    const pT = this.paddingTop;
    const pR = this.paddingRight;

    ctx.clearRect(0, 0, w, h);

    let maxVal = Math.max(...this.trendData.map(d => d.value));
    if (maxVal === 0) maxVal = 100;
    maxVal = maxVal * 1.2;

    const drawingW = w - pL - pR;
    const drawingH = h - pB - pT;

    this.trendData.forEach((point, index) => {
      point.x = pL + (drawingW / (this.trendData.length - 1)) * index;
      point.y = pT + drawingH - (point.value / maxVal) * drawingH * progress;
    });

    ctx.lineWidth = 1;
    ctx.strokeStyle = "#F0F0F0";
    ctx.font = "30px sans-serif";
    ctx.fillStyle = ThemeConfig.TEXT_LIGHT;
    ctx.textAlign = "right";
    ctx.textBaseline = "middle";

    for (let i = 0; i <= 3; i++) {
      let y = pT + (drawingH / 3) * i;
      let val = maxVal - (maxVal / 3) * i;
      ctx.beginPath();
      ctx.moveTo(pL, y);
      ctx.lineTo(w - pR, y);
      ctx.stroke();
      ctx.fillText(ChartUtils.formatCurrency(val), pL - 5, y);
    }

    if (this.trendData.length > 1) {
      ctx.beginPath();
      ctx.moveTo(this.trendData[0].x, this.trendData[0].y);

      for (let i = 0; i < this.trendData.length - 1; i++) {
        let p1 = this.trendData[i];
        let p2 = this.trendData[i + 1];
        let cp = ChartUtils.getControlPoints(
          i > 0 ? this.trendData[i-1].x : p1.x - (p2.x - p1.x),
          i > 0 ? this.trendData[i-1].y : p1.y,
          p1.x, p1.y,
          p2.x, p2.y
        );
        ctx.bezierCurveTo(cp.p1x, cp.p1y, cp.p2x, cp.p2y, p2.x, p2.y);
      }

      ctx.lineWidth = 3;
      ctx.strokeStyle = ThemeConfig.PRIMARY;
      ctx.stroke();

      ctx.lineTo(this.trendData[this.trendData.length - 1].x, h - pB);
      ctx.lineTo(this.trendData[0].x, h - pB);
      ctx.closePath();

      let grad = ctx.createLinearGradient(0, pT, 0, h - pB);
      grad.addColorStop(0, ThemeConfig.CHART_GRADIENT[0]);
      grad.addColorStop(1, ThemeConfig.CHART_GRADIENT[1]);
      ctx.fillStyle = grad;
      ctx.fill();
    }

    this.trendData.forEach((point, index) => {
      ctx.fillStyle = (this.selectedMonth === point.fullDate) ? ThemeConfig.PRIMARY : ThemeConfig.TEXT_LIGHT;
      ctx.font = (this.selectedMonth === point.fullDate) ? "bold 20px sans-serif" : "24px sans-serif";
      ctx.textAlign = "center";
      ctx.fillText(point.label, point.x, h - pB + 15);

      ctx.beginPath();
      let radius = (index === this.touchedPointIndex || this.selectedMonth === point.fullDate) ? 6 : 3;
      ctx.arc(point.x, point.y, radius, 0, Math.PI * 2);
      ctx.fillStyle = ThemeConfig.CARD_BG;
      ctx.fill();
      ctx.lineWidth = 2;
      ctx.strokeStyle = ThemeConfig.PRIMARY;
      ctx.stroke();

      if (index === this.touchedPointIndex || (this.selectedMonth === point.fullDate && this.touchedPointIndex === -1)) {
        this.drawTooltip(ctx, point);
      }
    });
  }

  drawTooltip(ctx: CanvasRenderingContext2D, point: TrendPoint) {
    let text = `¥${point.value}`;
    ctx.font = "30px sans-serif";
    let tm = ctx.measureText(text);
    let tw = tm.width + 16;
    let th = 24;
    let tx = point.x - tw / 2;
    let ty = point.y - 35;

    if (tx < 0) tx = 0;
    if (tx + tw > this.chartWidth) tx = this.chartWidth - tw;

    ctx.fillStyle = ThemeConfig.PRIMARY_DARK;
    ctx.beginPath();
    // 简化圆角矩形绘制
    ctx.fillRect(tx, ty, tw, th);

    ctx.beginPath();
    ctx.moveTo(point.x, ty + th);
    ctx.lineTo(point.x - 5, ty + th - 2);
    ctx.lineTo(point.x + 5, ty + th - 2);
    ctx.fill();

    ctx.fillStyle = Color.White;
    ctx.textAlign = "center";
    ctx.textBaseline = "middle";
    ctx.fillText(text, tx + tw / 2, ty + th / 2);
  }

  handleTouch(event: TouchEvent) {
    if (event.type === TouchType.Up || event.type === TouchType.Cancel) {
      this.touchedPointIndex = -1;
      this.drawChart(1);
      return;
    }

    let touchX = event.touches[0].x;
    let touchY = event.touches[0].y;

    let minDist = 1000;
    let closestIndex = -1;

    this.trendData.forEach((p, i) => {
      let dist = Math.abs(p.x - touchX);
      if (dist < minDist && dist < 30) {
        minDist = dist;
        closestIndex = i;
      }
    });

    if (closestIndex !== -1 && closestIndex !== this.touchedPointIndex) {
      this.touchedPointIndex = closestIndex;
      this.drawChart(1);
      this.onMonthSelect(this.trendData[closestIndex].fullDate);
    }
  }
}

/**
 * ====================================================================================
 * MAIN PAGE CONTROLLER
 * ====================================================================================
 */

@Component
export struct StatisticsPage {
  @Prop itemList: Item[] = [];

  // ================= 状态管理 =================
  @State animatedValues: number[] = [];

  // 筛选状态
  @State currentStatusFilter: number = 0; // 0:全部, 1:在用, 2:闲置, 3:已丢弃
  @State currentLocationFilter: string = "";
  @State currentCategoryFilter: string = "";
  @State currentMonthFilter: string = "";

  @State sortField: 'price' | 'time' = 'time';
  @State isAscending: boolean = false;
  @State isListReady: boolean = false;

  private readonly FILTER_TABS: string[] = ["全部", "在用", "闲置", "已丢弃"];
  private scroller: Scroller = new Scroller();

  // 缓存数据
  private cachedCategoryStats: CategoryStat[] = [];
  private cachedTrendData: TrendPoint[] = [];

  // 新增缓存数据
  private cachedRadarData: RadarIndicator[] = [];
  private cachedHistogramBins: HistogramBin[] = [];
  private cachedStackedData: StackedBarData[] = [];

  aboutToAppear() {
    this.refreshCharts();
  }

  onItemListChange() {
    this.refreshCharts();
  }

  refreshCharts() {
    this.cachedCategoryStats = this.calculateCategoryStats();
    this.cachedTrendData = this.calculateTrendData();
    this.cachedRadarData = this.calculateRadarData();
    this.cachedHistogramBins = this.calculateHistogramBins();
    this.cachedStackedData = this.calculateStackedData();

    this.triggerChartsAnimation();
  }

  /**
   * 1. 资产总值
   */
  private getTotalPrice(): number {
    return this.itemList.reduce((sum, item) => sum + item.price, 0);
  }

  /**
   * 2. 闲置统计 (修复：使用 new 关键字实例化)
   */
  private getIdleStats(): IdleStatsResult {
    const idleItems = this.itemList.filter(i => i.useStatus === "闲置");
    return new IdleStatsResult(
      idleItems.reduce((sum, i) => sum + i.price, 0),
      idleItems.length
    );
  }

  /**
   * 3. 雷达图数据计算 (修复：使用 new 关键字实例化)
   * 维度：均价、最大价、闲置率、数量、新品率
   */
  private calculateRadarData(): RadarIndicator[] {
    if (this.itemList.length === 0) return [];

    // 1. 均价 (Standard: 500)
    const avgPrice = this.getTotalPrice() / this.itemList.length;
    const avgScore = Math.min(100, (avgPrice / 1000) * 100);

    // 2. 最大价 (Standard: 2000)
    const maxPrice = Math.max(...this.itemList.map(i => i.price));
    const maxScore = Math.min(100, (maxPrice / 5000) * 100);

    // 3. 闲置率 (反向指标：闲置越少分越高)
    const idleCount = this.itemList.filter(i => i.useStatus === "闲置").length;
    const idleRate = idleCount / this.itemList.length;
    const idleScore = (1 - idleRate) * 100;

    // 4. 数量 (Standard: 50)
    const countScore = Math.min(100, (this.itemList.length / 50) * 100);

    // 5. 新品率 (今年购买的占比)
    const currentYear = new Date().getFullYear();
    const newItems = this.itemList.filter(i => {
      try { return new Date(i.buyDate).getFullYear() === currentYear }
      catch { return false }
    }).length;
    const newRate = newItems / this.itemList.length;
    const newScore = newRate * 100;

    return [
      new RadarIndicator("消费力", avgScore),
      new RadarIndicator("峰值消费", maxScore),
      new RadarIndicator("利用率", idleScore),
      new RadarIndicator("囤积度", countScore),
      new RadarIndicator("新鲜度", newScore)
    ];
  }

  /**
   * 4. 直方图分箱计算 (修复：使用 new 关键字实例化)
   */
  private calculateHistogramBins(): HistogramBin[] {
    if (this.itemList.length === 0) return [];

    // 定义区间：0-100, 100-300, 300-1000, 1000-3000, 3000+
    const ranges = [0, 100, 300, 1000, 3000, 999999];
    const bins: HistogramBin[] = [];

    for (let i = 0; i < ranges.length - 1; i++) {
      const min = ranges[i];
      const max = ranges[i+1];
      const label = max > 10000 ? `${min}+` : `${min}-${max}`;

      const items = this.itemList.filter(item => item.price >= min && item.price < max);

      bins.push(new HistogramBin(
        label,
        min,
        max,
        items.length,
        items.reduce((sum, item) => sum + item.price, 0)
      ));
    }
    return bins;
  }

  /**
   * 5. 堆叠图数据计算 (修复：使用 new 关键字实例化)
   */
  private calculateStackedData(): StackedBarData[] {
    const map = new Map<string, StackedBarData>();

    DEFAULT_CATEGORY_OPTIONS.forEach(cat => {
      map.set(cat, new StackedBarData(cat, 0, 0, 0, 0));
    });

    this.itemList.forEach(item => {
      const cat = item.category || "其他";
      const data = map.get(cat);
      if (data) {
        if (item.useStatus === "在用") data.usedCount++;
        else if (item.useStatus === "闲置") data.idleCount++;
        else data.wasteCount++;
        data.total++;
      }
    });

    return Array.from(map.values()).filter(d => d.total > 0);
  }

  /**
   * 6. 分类统计 (修复：使用 new 关键字实例化)
   */
  private calculateCategoryStats(): CategoryStat[] {
    const total = this.itemList.length;
    if (total === 0) return [];

    const statsMap = new Map<string, CategoryStat>();
    DEFAULT_CATEGORY_OPTIONS.forEach(cat => {
      statsMap.set(cat, new CategoryStat(cat));
    });

    this.itemList.forEach(item => {
      const cat = item.category || "其他";
      const stat = statsMap.get(cat);
      if (stat) {
        stat.count++;
        stat.totalPrice += item.price;
      }
    });

    let result: CategoryStat[] = [];
    statsMap.forEach((value) => {
      if (value.count > 0) {
        value.percentage = (value.count / total) * 100;
        result.push(value);
      }
    });

    result.sort((a, b) => b.count - a.count);
    result.forEach((stat, index) => {
      stat.color = ThemeConfig.RAINBOW_COLORS[index % ThemeConfig.RAINBOW_COLORS.length];
    });

    return result;
  }

  /**
   * 7. 趋势图数据 (修复：使用 new 关键字实例化)
   */
  private calculateTrendData(): TrendPoint[] {
    const map = new Map<string, number>();
    const now = new Date();
    const result: TrendPoint[] = [];

    for (let i = 5; i >= 0; i--) {
      const d = new Date(now.getFullYear(), now.getMonth() - i, 1);
      const key = `${d.getFullYear()}-${(d.getMonth() + 1).toString().padStart(2, '0')}`;
      const label = `${d.getMonth() + 1}月`;
      map.set(key, 0);
      result.push(new TrendPoint(label, key, 0));
    }

    this.itemList.forEach(item => {
      const key = ChartUtils.getMonthKey(item.buyDate);
      if (map.has(key)) {
        map.set(key, map.get(key)! + item.price);
      }
    });

    result.forEach(pt => {
      pt.value = map.get(pt.fullDate) || 0;
    });

    return result;
  }

  /**
   * 8. 状态与位置 (修复：使用 new 关键字实例化)
   */
  private getStatusStats(): StatusStat[] {
    const total = this.itemList.length;
    if (total === 0) return [];
    const statuses: ItemUseStatus[] = ["在用", "闲置", "已丢弃"];
    return statuses.map(status => {
      const items = this.itemList.filter(item => item.useStatus === status);
      return new StatusStat(
        status,
        items.length,
        (items.length / total) * 100,
        ThemeConfig.STATUS_COLORS[status] || "#333",
        items.reduce((sum, i) => sum + i.price, 0)
      );
    });
  }

  private getPositionStats(): PositionStat[] {
    if (this.itemList.length === 0) return [];
    const countMap = new Map<string, number>();
    let maxCount = 0;

    this.itemList.forEach(item => {
      const loc = item.location && item.location.trim() !== "" ? item.location : "未归位";
      const current = (countMap.get(loc) || 0) + 1;
      countMap.set(loc, current);
      if (current > maxCount) maxCount = current;
    });

    let result: PositionStat[] = [];
    let colorIndex = 0;
    countMap.forEach((count, name) => {
      const scale = maxCount > 1 ? (count - 1) / (maxCount - 1) : 1;
      const isSelected = this.currentLocationFilter === name;
      result.push(new PositionStat(
        name,
        count,
        12 + (scale * 12),
        400 + Math.floor(scale * 300),
        ThemeConfig.RAINBOW_COLORS[colorIndex % ThemeConfig.RAINBOW_COLORS.length],
        (this.currentLocationFilter && !isSelected) ? 0.3 : 1.0
      ));
      colorIndex++;
    });
    return result.sort((a, b) => b.count - a.count);
  }

  /**
   * 筛选与排序
   */
  private getFilteredAndSortedItems(): Item[] {
    let result = this.itemList;
    if (this.currentStatusFilter === 1) result = result.filter(i => i.useStatus === '在用');
    else if (this.currentStatusFilter === 2) result = result.filter(i => i.useStatus === '闲置');
    else if (this.currentStatusFilter === 3) result = result.filter(i => i.useStatus === '已丢弃');

    if (this.currentLocationFilter) {
      result = result.filter(i => {
        const iLoc = i.location && i.location.trim() !== "" ? i.location : "未归位";
        return iLoc === this.currentLocationFilter;
      });
    }
    if (this.currentCategoryFilter) {
      result = result.filter(i => i.category === this.currentCategoryFilter);
    }
    if (this.currentMonthFilter) {
      result = result.filter(i => ChartUtils.getMonthKey(i.buyDate) === this.currentMonthFilter);
    }

    return [...result].sort((a, b) => {
      let valA: number;
      let valB: number;
      if (this.sortField === 'price') {
        valA = a.price;
        valB = b.price;
      } else {
        valA = a.buyDate ? new Date(a.buyDate).getTime() : 0;
        valB = b.buyDate ? new Date(b.buyDate).getTime() : 0;
        if (isNaN(valA)) valA = 0;
        if (isNaN(valB)) valB = 0;
      }
      return this.isAscending ? valA - valB : valB - valA;
    });
  }

  private toggleSort(field: 'price' | 'time') {
    if (this.sortField === field) {
      this.isAscending = !this.isAscending;
    } else {
      this.sortField = field;
      this.isAscending = false;
    }
  }

  private triggerChartsAnimation() {
    const stats = this.calculateCategoryStats();
    const targetValues = stats.map(s => s.count);
    this.animatedValues = targetValues.map(() => 0);
    setTimeout(() => {
      animateTo({ duration: 800, curve: curves.cubicBezierCurve(0.2, 0.0, 0.2, 1.0) }, () => {
        this.animatedValues = targetValues;
      })
    }, 200);
  }

  private getDataPanelColors(): ResourceColor[] {
    return this.calculateCategoryStats().map(s => s.color);
  }

  private resetFilters() {
    animateTo({ duration: 300 }, () => {
      this.currentCategoryFilter = "";
      this.currentMonthFilter = "";
      this.currentLocationFilter = "";
      this.currentStatusFilter = 0;
    })
  }

  private getFilterStatusText(): string {
    let parts: string[] = [];
    if (this.currentCategoryFilter) parts.push(`分类:${this.currentCategoryFilter}`);
    if (this.currentMonthFilter) parts.push(`月份:${this.currentMonthFilter}`);
    if (this.currentLocationFilter) parts.push(`位置:${this.currentLocationFilter}`);
    return parts.length > 0 ? `正在筛选: ${parts.join(" + ")}` : "";
  }

  build() {
    Column() {
      List({ scroller: this.scroller }) {

        // 1. 顶部资产卡片
        ListItem() {
          AssetHeader({
            totalValue: this.getTotalPrice(),
            totalCount: this.itemList.length
          })
            .padding({ left: 16, right: 16, top: 16, bottom: 8 })
        }

        if (this.itemList.length === 0) {
          ListItem() {
            Column() {
              Text("暂无物品数据")
                .fontSize(16)
                .fontWeight(FontWeight.Bold)
                .fontColor(ThemeConfig.TEXT_SECONDARY)
              Text("点击左下角添加你的第一件物品吧")
                .fontSize(14)
                .fontColor(ThemeConfig.TEXT_LIGHT)
                .margin({ top: 8 })
            }
            .width("100%")
            .height(400)
            .justifyContent(FlexAlign.Center)
          }
        } else {

          // ========================= 第一梯队：核心分析 =========================

          // 2. 闲置成本分析
          ListItem() {
            IdleAnalysisCard({
              totalValue: this.getTotalPrice(),
              idleValue: this.getIdleStats().value,
              idleCount: this.getIdleStats().count
            })
          }
          .padding({ left: 16, right: 16 })

          // 3. 雷达图 (NEW)
          ListItem() {
            RadarChart({ indicators: this.cachedRadarData })
          }
          .padding({ left: 16, right: 16 })

          // ========================= 第二梯队：趋势与分布 =========================

          // 4. 消费趋势图
          ListItem() {
            TrendChart({
              trendData: this.cachedTrendData.length > 0 ? this.cachedTrendData : this.calculateTrendData(),
              selectedMonth: this.currentMonthFilter,
              onMonthSelect: (month) => {
                animateTo({ duration: 300 }, () => {
                  this.currentMonthFilter = this.currentMonthFilter === month ? "" : month;
                })
              }
            })
          }
          .padding({ left: 16, right: 16 })

          // 5. 价格分布直方图 (NEW)
          ListItem() {
            PriceHistogram({ bins: this.cachedHistogramBins })
          }
          .padding({ left: 16, right: 16 })

          // ========================= 第三梯队：分类与详情 =========================

          // 6. 分类堆叠对比图 (NEW)
          ListItem() {
            StackedBarChart({ data: this.cachedStackedData })
          }
          .padding({ left: 16, right: 16 })

          // 7. 分类环形图
          ListItem() {
            BaseCard({
              title: "物品分类",
              subTitle: this.currentCategoryFilter ? `已选中: ${this.currentCategoryFilter}` : "点击右侧列表可筛选",
              showArrow: !!this.currentCategoryFilter,
              onHeaderClick: () => { this.currentCategoryFilter = "" }
            }) {
              Row() {
                Stack() {
                  DataPanel({
                    values: this.animatedValues,
                    max: this.itemList.length,
                    type: DataPanelType.Circle
                  })
                    .width(130).height(130)
                    .valueColors(this.getDataPanelColors())
                }
                .layoutWeight(4)

                Column() {
                  ForEach(this.calculateCategoryStats().slice(0, 5), (stat: CategoryStat) => {
                    CategoryLegendItem({
                      stat: stat,
                      isSelected: this.currentCategoryFilter === stat.name,
                      onClickItem: () => {
                        animateTo({ duration: 300 }, () => {
                          this.currentCategoryFilter = this.currentCategoryFilter === stat.name ? "" : stat.name;
                        })
                      }
                    })
                  })
                }
                .layoutWeight(6)
                .padding({ left: 16 })
              }
            }
          }
          .padding({ left: 16, right: 16 })

          // 8. 空间分布
          ListItem() {
            BaseCard({
              title: "空间分布",
              subTitle: this.currentLocationFilter ? `正在筛选: ${this.currentLocationFilter}` : "点击标签可筛选列表"
            }) {
              InteractiveWordCloud({
                stats: this.getPositionStats(),
                selectedLocation: this.currentLocationFilter,
                onLocationSelect: (loc) => {
                  animateTo({ duration: 300 }, () => {
                    this.currentLocationFilter = loc;
                  })
                }
              })
            }
          }
          .padding({ left: 16, right: 16 })

          // 9. 列表控制栏
          ListItem() {
            Column() {
              Row() {
                Text("物品明细")
                  .fontSize(20)
                  .fontWeight(FontWeight.Bold)
                  .layoutWeight(1)

                if (this.currentCategoryFilter || this.currentMonthFilter || this.currentLocationFilter) {
                  Row() {
                    Text("清除筛选")
                      .fontSize(12)
                      .fontColor(ThemeConfig.PRIMARY)
                      .margin({ right: 4 })
                    Text("✕")
                      .fontSize(10)
                      .fontColor(ThemeConfig.PRIMARY)
                  }
                  .padding({ top: 4, bottom: 4, left: 8, right: 8 })
                  .backgroundColor("#Eef2ff")
                  .borderRadius(12)
                  .onClick(() => this.resetFilters())
                }
              }
              .width("100%")
              .margin({ bottom: 12, left: 4 })

              // 状态筛选
              Scroll() {
                Row() {
                  ForEach(this.FILTER_TABS, (tab: string, index: number) => {
                    Text(tab)
                      .fontSize(14)
                      .fontWeight(this.currentStatusFilter === index ? FontWeight.Medium : FontWeight.Normal)
                      .fontColor(this.currentStatusFilter === index ? Color.White : ThemeConfig.TEXT_SECONDARY)
                      .backgroundColor(this.currentStatusFilter === index ? ThemeConfig.PRIMARY : "#E0E4E8")
                      .padding({ top: 8, bottom: 8, left: 16, right: 16 })
                      .borderRadius(20)
                      .margin({ right: 10 })
                      .onClick(() => {
                        animateTo({ duration: 300, curve: curves.springMotion() }, () => {
                          this.currentStatusFilter = index;
                        })
                      })
                  })
                }
              }
              .scrollable(ScrollDirection.Horizontal)
              .scrollBar(BarState.Off)
              .width("100%")
              .margin({ bottom: 16 })

              // 排序栏
              Row() {
                Text(this.getFilterStatusText() || `共 ${this.getFilteredAndSortedItems().length} 条数据`)
                  .fontSize(12)
                  .fontColor(ThemeConfig.TEXT_LIGHT)
                  .layoutWeight(1)
                  .maxLines(1)
                  .textOverflow({ overflow: TextOverflow.Ellipsis })

                Row() {
                  Row() {
                    Text("价格")
                      .fontSize(12)
                      .fontColor(this.sortField === 'price' ? ThemeConfig.PRIMARY : ThemeConfig.TEXT_LIGHT)
                    if (this.sortField === 'price') {
                      Text(this.isAscending ? "▲" : "▼")
                        .fontSize(10)
                        .fontColor(ThemeConfig.PRIMARY)
                        .margin({ left: 4 })
                    }
                  }
                  .padding(8)
                  .backgroundColor(this.sortField === 'price' ? "#Eef2ff" : "Transparent")
                  .borderRadius(8)
                  .onClick(() => {
                    animateTo({ duration: 250 }, () => { this.toggleSort('price') })
                  })

                  Row() {
                    Text("日期")
                      .fontSize(12)
                      .fontColor(this.sortField === 'time' ? ThemeConfig.PRIMARY : ThemeConfig.TEXT_LIGHT)
                    if (this.sortField === 'time') {
                      Text(this.isAscending ? "▲" : "▼")
                        .fontSize(10)
                        .fontColor(ThemeConfig.PRIMARY)
                        .margin({ left: 4 })
                    }
                  }
                  .padding(8)
                  .margin({ left: 8 })
                  .backgroundColor(this.sortField === 'time' ? "#Eef2ff" : "Transparent")
                  .borderRadius(8)
                  .onClick(() => {
                    animateTo({ duration: 250 }, () => { this.toggleSort('time') })
                  })
                }
              }
              .width("100%")
              .padding({ bottom: 12 })
              .border({ width: { bottom: 1 }, color: "#F0F0F0" })
            }
            .padding({ left: 16, right: 16, bottom: 10 })
            .backgroundColor(ThemeConfig.BACKGROUND)
          }

          // 列表内容
          ForEach(this.getFilteredAndSortedItems(), (item: Item, index: number) => {
            ListItem() {
              ComplexItemCard({ item: item })
                .margin({ left: 16, right: 16 })
                .opacity(this.isListReady ? 1 : 0)
                .translate({ y: this.isListReady ? 0 : 20 })
                .animation({
                  duration: 400,
                  delay: index * 30,
                  curve: Curve.EaseOut
                })
            }
          }, (item: Item) => `${item.id}_${item.useStatus}_${item.price}`)

          ListItem().height(80)
        }
      }
      .width("100%")
      .height("100%")
      .edgeEffect(EdgeEffect.Spring)
    }
    .width("100%")
    .height("100%")
    .backgroundColor(ThemeConfig.BACKGROUND)
    .onVisibleAreaChange([0.0, 1.0], (isVisible: boolean, ratio: number) => {
      if (isVisible && ratio > 0) {
        this.refreshCharts();
        setTimeout(() => { this.isListReady = true; }, 100);
      }
    })
  }
}