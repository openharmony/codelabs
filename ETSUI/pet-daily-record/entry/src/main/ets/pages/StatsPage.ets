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
import { Pet, PetCost } from '../models/PetModel';
import { DatabaseManager } from '../models/DatabaseManager';
import promptAction from '@ohos.promptAction';
import display from '@ohos.display'; // 【新增1】引入屏幕显示能力

// ====================================================================================
// 模块一：企业级图表配置系统
// ====================================================================================

interface IFontBase {
  title: number;
  axis: number;
  legendTitle: number;
  legendDesc: number;
  emptyState: number;
}

interface ILayoutConfig {
  baseHeight: number;
  padding: ChartPadding;
  pieRadiusRatio: number;
}

interface IColorTheme {
  primary: string;
  secondary: string;
  background: string;
  gridLine: string;
  textPrimary: string;
  textSecondary: string;
  palette: string[];
}

class ChartThemeConfig {
  // 这里定义的数值是“视觉大小(vp)”，大概相当于 word 文档里的字号
  // 14vp 在任何手机上看起来都应该是正常文字大小
  static readonly FONT_SIZES: IFontBase = {
    title: 14,
    axis: 10,
    legendTitle: 12,
    legendDesc: 10,
    emptyState: 14
  };
  static readonly LAYOUT: ILayoutConfig = {
    baseHeight: 220,
    padding: {
      top: 40,
      right: 20,
      bottom: 30,
      left: 40
    },
    pieRadiusRatio: 0.23
  };
  static readonly COLORS: IColorTheme = {
    primary: '#007AFF',
    secondary: '#5856D6',
    background: '#FAFAFA',
    gridLine: '#E5E5EA',
    textPrimary: '#1C1C1E',
    textSecondary: '#8E8E93',
    palette: [
      '#FF9500', '#FF2D55', '#007AFF', '#5856D6',
      '#34C759', '#AF52DE', '#FFCC00', '#5AC8FA'
    ]
  };

  static getPaletteColor(index: number): string {
    return ChartThemeConfig.COLORS.palette[index % ChartThemeConfig.COLORS.palette.length];
  }

  static getCategoryColor(cat: string): string {
    switch (cat) {
      case '伙食':
        return '#FF9500';
      case '医疗':
        return '#FF2D55';
      case '玩具':
        return '#007AFF';
      case '美容':
        return '#AF52DE';
      case '洗澡':
        return '#5AC8FA';
      case '零食':
        return '#34C759';
      case '寄养':
        return '#5856D6';
      default:
        return '#8E8E93';
    }
  }
}

// ====================================================================================
// 模块二：智能 Mock 数据生成器 (保持原样)
// ====================================================================================

class MockDataGenerator {
  static async generateSmartData(startDateStr: string, endDateStr: string, pets: Pet[],
    dbManager: DatabaseManager): Promise<number> {
    if (pets.length === 0) {
      return 0;
    }
    const categories = ['伙食', '医疗', '玩具', '美容', '零食', '其他'];
    let count = 0;
    const start = new Date(startDateStr);
    const end = new Date(endDateStr);
    const current = new Date(start);

    while (current <= end) {
      if (Math.random() < 0.8) {
        const dailyRecords = Math.floor(Math.random() * 1) + 3;
        const y = current.getFullYear();
        const m = (current.getMonth() + 1).toString().padStart(2, '0');
        const d = current.getDate().toString().padStart(2, '0');
        const dateStr = `${y}-${m}-${d}`;
        for (let i = 0; i < dailyRecords; i++) {
          const pet = MockDataGenerator.randomItem(pets);
          const cat = MockDataGenerator.randomItem(categories);
          const amt = MockDataGenerator.randomAmount(10, 30);
          await dbManager.insertCost(pet.id, amt, cat, dateStr);
          count++;
        }
      }
      current.setDate(current.getDate() + 1);
    }
    return count;
  }

  private static randomAmount(min: number, max: number): number {
    const val = Math.random() * (max - min) + min;
    return parseFloat(val.toFixed(2));
  }

  private static randomItem<T>(arr: T[]): T {
    const idx = Math.floor(Math.random() * arr.length);
    return arr[idx];
  }
}

// ====================================================================================
// 模块三：绘图辅助工具类 (核心修复位置)
// ====================================================================================

interface ChartPadding {
  top: number;
  right: number;
  bottom: number;
  left: number;
}

interface ChartPoint {
  x: number;
  y: number;
  val: number;
  dateStr: string;
}

interface YAxisScale {
  max: number;
  step: number;
}

class ChartUtils {
  // 【新增2】获取当前设备的屏幕密度（倍率）
  // 普通手机是2.0或3.0，你的Nova15可能是3.0+
  static getDensity(): number {
    try {
      return display.getDefaultDisplaySync().densityPixels;
    } catch (err) {
      return 3; // 获取失败时的兜底值，给高一点保证看得清
    }
  }

  static getAllDates(startStr: string, endStr: string): string[] {
    let dates: string[] = [];
    let current = new Date(startStr);
    const end = new Date(endStr);
    let safeGuard = 0;
    while (current <= end && safeGuard < 1825) {
      const y = current.getFullYear();
      const m = (current.getMonth() + 1).toString().padStart(2, '0');
      const d = current.getDate().toString().padStart(2, '0');
      dates.push(`${y}-${m}-${d}`);
      current.setDate(current.getDate() + 1);
      safeGuard++;
    }
    return dates;
  }

  static getNiceScale(originalMax: number, ticks: number): YAxisScale {
    if (originalMax <= 0) {
      return { max: 500, step: 100 };
    }
    const roughStep = originalMax / ticks;
    const magnitude = Math.pow(10, Math.floor(Math.log10(roughStep)));
    const normalizedStep = roughStep / magnitude;
    let niceStep: number;
    if (normalizedStep <= 1) {
      niceStep = 1;
    } else if (normalizedStep <= 2) {
      niceStep = 2;
    } else if (normalizedStep <=
      5) {
      niceStep = 5;
    } else {
      niceStep = 10;
    }
    const finalStep = niceStep * magnitude;
    const finalMax = Math.ceil(originalMax / finalStep) * finalStep;
    return { max: finalMax, step: finalStep };
  }

  /**
   * 【关键修复】动态字体计算器
   * 逻辑：基准字号 * 画布缩放比 * 屏幕密度
   */
  static getDynamicFont(baseSize: number, currentW: number, bold: boolean = false): string {
    // 1. 获取屏幕密度
    const density = ChartUtils.getDensity();

    // 2. 计算画布宽度的缩放比 (基于360vp设计稿)
    const ratio = currentW > 0 ? currentW / 360 : 1;
    // 限制一下缩放范围，防止太夸张
    const scale = Math.min(Math.max(ratio, 0.8), 1.2);

    // 3. 计算最终像素值 = 基准 * 缩放 * 密度
    // 例如：14 * 1.0 * 3.0 = 42px (这在你的手机上就清晰了)
    const finalSize = Math.floor(baseSize * scale * density);

    return `${bold ? 'bold ' : ''}${finalSize}px sans-serif`;
  }
}

// ====================================================================================
// 模块四：记账弹窗组件 (保持原样)
// ====================================================================================

// ====================================================================================
// 模块四：记账弹窗组件 (优化版)
// ====================================================================================

// 在 AddCostDialog 结构体上方添加这个接口定义，解决 ArkTS 校验报错
interface CustomMenuElement {
  value: string;
  action: () => void;
}

@CustomDialog
struct AddCostDialog {
  controller: CustomDialogController
  pets: Pet[] = []
  @State amount: string = ''
  @State selectedPetIndex: number = 0
  @State selectedCategory: string = '伙食'
  @State selectedDate: string = ''
  private categories: string[] = ['伙食', '医疗', '玩具', '美容', '零食', '其他']
  onRefreshPets: () => void = () => {
  }
  confirm: (pId: number, amt: number, cat: string, date: string) => void = () => {
  }

  aboutToAppear() {
    const now = new Date();
    this.selectedDate = `${now.getFullYear()}-${(now.getMonth() + 1).toString().padStart(2, '0')}-${now.getDate()
      .toString()
      .padStart(2, '0')}`;
  }

  build() {
    Column({ space: 15 }) {
      Text('记一笔').fontSize(20).fontWeight(FontWeight.Bold).margin({ top: 15, bottom: 5 })

      Column({ space: 5 }) {
        Text('金额 (¥)').fontSize(14).fontColor('#666').width('90%')
        TextInput({ placeholder: '0.00', text: this.amount })
          .type(InputType.Normal)
          .fontSize(24)
          .placeholderFont({ size: 24 })
          .height(55)
          .fontColor('#0A84FF')
          .fontWeight(FontWeight.Medium)
          .onChange((v: string) => this.amount = v)
          .width('90%')
          .backgroundColor('#F2F2F7')
          .borderRadius(12)
      }

      // 消费对象 - 修复了类型报错
      Row() {
        Text('消费对象').fontSize(16)
        Row() {
          Text(this.pets[this.selectedPetIndex]?.name || '选择宠物').fontSize(16).fontColor('#333')
          Text(' ▼').fontSize(12).fontColor('#999')
        }
        .padding({
          left: 12,
          right: 12,
          top: 8,
          bottom: 8
        })
        .backgroundColor('#F2F2F7')
        .borderRadius(8)
        .bindMenu(this.pets.map((p, index): CustomMenuElement => {
          // 【修复报错关键】显式声明类型并返回
          let item: CustomMenuElement = {
            value: p.name,
            action: () => {
              this.selectedPetIndex = index;
            }
          }
          return item;
        }))
        .onClick(() => {
          this.closeKeyboard();
        })
      }
      .width('90%').justifyContent(FlexAlign.SpaceBetween)

      // 消费类型 - 修复了类型报错
      Row() {
        Text('消费类型').fontSize(16)
        Row() {
          Text(this.selectedCategory).fontSize(16).fontColor('#333')
          Text(' ▼').fontSize(12).fontColor('#999')
        }
        .padding({
          left: 12,
          right: 12,
          top: 8,
          bottom: 8
        })
        .backgroundColor('#F2F2F7')
        .borderRadius(8)
        .bindMenu(this.categories.map((cat): CustomMenuElement => {
          // 【修复报错关键】显式声明类型并返回
          let item: CustomMenuElement = {
            value: cat,
            action: () => {
              this.selectedCategory = cat;
            }
          }
          return item;
        }))
        .onClick(() => {
          this.closeKeyboard();
        })
      }
      .width('90%').justifyContent(FlexAlign.SpaceBetween)

      Row() {
        Text('日期').fontSize(16)
        Text(this.selectedDate)
          .fontSize(16)
          .fontColor('#0A84FF')
          .padding(8)
          .backgroundColor('#F2F2F7')
          .borderRadius(8)
          .onClick(() => {
            this.closeKeyboard();
            DatePickerDialog.show({
              onAccept: (v: DatePickerResult) => {
                const y = v.year ?? 2026;
                const m = (v.month ?? 0) + 1;
                const d = v.day ?? 1;
                this.selectedDate = `${y}-${m.toString().padStart(2, '0')}-${d.toString().padStart(2, '0')}`;
              }
            })
          })
      }.width('90%').justifyContent(FlexAlign.SpaceBetween)

      Row({ space: 15 }) {
        Button('取消')
          .onClick(() => {
            this.closeKeyboard();
            this.controller.close();
          })
          .backgroundColor('#F2F2F7')
          .fontColor('#007AFF')
          .layoutWeight(1)
          .height(45)
        Button('确认保存')
          .onClick(() => {
            let val = parseFloat(this.amount);
            if (isNaN(val) || val <= 0) {
              promptAction.showToast({ message: '请输入有效金额' });
              return;
            }
            this.closeKeyboard();
            this.confirm(this.pets[this.selectedPetIndex].id, val, this.selectedCategory, this.selectedDate);
            this.controller.close();
          })
          .backgroundColor('#007AFF')
          .fontColor(Color.White)
          .layoutWeight(1)
          .height(45)
      }.width('90%').margin({ bottom: 20, top: 10 })
    }
    .padding({ left: 10, right: 10 })
    .backgroundColor(Color.White)
    .borderRadius(24)
  }

  private closeKeyboard() {
    try {
      focusControl.requestFocus('');
    } catch (e) {
    }
  }
}


// ====================================================================================
// 模块五：核心页面逻辑 (StatsPage)
// ====================================================================================

@Component
export struct StatsPage {
  @State pets: Pet[] = [];
  @State costs: PetCost[] = [];
  @State totalAmount: number = 0.00;
  @State maxAmount: number = 0.00;
  @State avgAmount: number = 0.00;
  @State selectedPetId: number = -1;
  @State selectedCategory: string = '全部';
  @State startDate: string = '';
  @State endDate: string = '';
  @State chartMode: string = 'trend';
  @State actualCanvasWidth: number = 0;
  @State actualCanvasHeight: number = ChartThemeConfig.LAYOUT.baseHeight;
  @StorageLink('petDataChangeSignal') @Watch('onDataUpdate') dataSignal: number = 0;
  dialogController: CustomDialogController = new CustomDialogController({
    builder: AddCostDialog({
      pets: this.pets,
      onRefreshPets: () => {
        this.initData();
      },
      confirm: (pId: number, amt: number, cat: string, date: string) => {
        this.handleSave(pId, amt, cat, date);
      }
    }),
    alignment: DialogAlignment.Bottom,
    offset: { dx: 0, dy: -15 },
    customStyle: true
  })
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(new RenderingContextSettings(true));
  // 缓存像素密度，避免频繁调用系统API
  private density: number = 1;
  private chartMaxValCache: number = 100;

  onDataUpdate(): void {
    const db = DatabaseManager.getInstance();
    db.getAllPets().then((latestPets: Pet[]) => {
      this.pets = latestPets;
      this.refreshUI();
    });
  }

  aboutToAppear() {
    const now = new Date();
    this.endDate = `${now.getFullYear()}-${(now.getMonth() + 1).toString().padStart(2, '0')}-${now.getDate()
      .toString()
      .padStart(2, '0')}`;
    this.startDate = `${now.getFullYear()}-01-01`;
    // 初始化时获取一次密度
    this.density = ChartUtils.getDensity();
    this.initData();
  }

  async initData() {
    const res = await DatabaseManager.getInstance().queryAllPets();
    let arr: Pet[] = [];
    if (res) {
      while (res.goToNextRow()) {
        arr.push({
          id: res.getLong(res.getColumnIndex('id')),
          name: res.getString(res.getColumnIndex('name')),
          species: '',
          gender: '',
          birthday: '',
          age: 0
        });
      }
      res.close();
    }
    this.pets = arr;
    await this.refreshUI();
  }

  async refreshUI() {
    const res = await DatabaseManager.getInstance().queryCostsFiltered(
      this.selectedPetId, this.selectedCategory, this.startDate, this.endDate
    );

    let list: PetCost[] = [];
    if (res) {
      while (res.goToNextRow()) {
        let val = res.getDouble(res.getColumnIndex('amount'));
        list.push({
          id: res.getLong(res.getColumnIndex('id')),
          petId: res.getLong(res.getColumnIndex('petId')),
          amount: val,
          category: res.getString(res.getColumnIndex('category')),
          date: res.getString(res.getColumnIndex('date'))
        });
      }
      res.close();
    }

    list.sort((a, b) => new Date(a.date).getTime() - new Date(b.date).getTime());
    this.costs = list;

    let sum = 0;
    let singleMax = 0;
    let chartMax = 0;
    let dailyPetMap = new Map<string, number>();

    for (let i = 0; i < list.length; i++) {
      let item = list[i];
      let v = item.amount;
      sum += v;
      if (v > singleMax) {
        singleMax = v;
      }
      const key = `${item.petId}_${item.date}`;
      const currentSum = (dailyPetMap.get(key) || 0) + v;
      dailyPetMap.set(key, currentSum);
      if (currentSum > chartMax) {
        chartMax = currentSum;
      }
    }

    this.totalAmount = sum;
    this.maxAmount = singleMax;
    this.avgAmount = list.length > 0 ? sum / list.length : 0;

    if (chartMax === 0) {
      chartMax = 100;
    }
    this.chartMaxValCache = chartMax;

    setTimeout(() => {
      this.triggerRedraw();
    }, 50);
  }

  async handleMockData() {
    if (this.pets.length === 0) {
      promptAction.showToast({ message: '请先添加至少一只宠物' });
      return;
    }
    promptAction.showToast({ message: '生成数据中...' });
    try {
      await MockDataGenerator.generateSmartData(
        this.startDate, this.endDate, this.pets, DatabaseManager.getInstance()
      );
      await this.refreshUI();
      promptAction.showToast({ message: '完成' });
    } catch (err) {
      promptAction.showToast({ message: '生成失败' });
    }
  }

  // ==================================================================================
  // 绘图系统
  // ==================================================================================

  triggerRedraw() {
    if (this.actualCanvasWidth > 0) {
      this.renderChart(this.costs, this.chartMaxValCache);
    }
  }

  renderChart(data: PetCost[], maxVal: number) {
    const ctx = this.context;
    const W = this.actualCanvasWidth;
    const H = this.actualCanvasHeight;

    ctx.clearRect(0, 0, W, H);
    ctx.fillStyle = ChartThemeConfig.COLORS.background;
    ctx.fillRect(0, 0, W, H);

    if (this.chartMode === 'category') {
      this.drawPieChart(ctx, data, W, H);
    } else {
      this.drawTrendChart(ctx, data, maxVal, W, H);
    }
  }

  drawTrendChart(ctx: CanvasRenderingContext2D, data: PetCost[], rawMaxVal: number, W: number, H: number) {
    const cfg = ChartThemeConfig.LAYOUT.padding;
    const padding: ChartPadding = {
      top: cfg.top,
      right: cfg.right,
      bottom: cfg.bottom,
      left: cfg.left
    };

    const chartW = W - padding.left - padding.right;
    const chartH = H - padding.top - padding.bottom;

    let startStr = this.startDate;
    let endStr = this.endDate;
    const allDates = ChartUtils.getAllDates(startStr, endStr);
    const totalDays = allDates.length > 0 ? allDates.length : 1;

    const scale = ChartUtils.getNiceScale(rawMaxVal, 5);
    const yScale = chartH / scale.max;

    this.drawGridAndAxes(ctx, W, H, padding, scale, startStr, endStr);

    if (data.length === 0) {
      this.drawEmptyState(ctx, W, H);
      return;
    }

    let targetPets = (this.selectedPetId === -1) ? this.pets : this.pets.filter(p => p.id === this.selectedPetId);

    targetPets.forEach((pet) => {
      const petCosts = data.filter(c => c.petId === pet.id);
      if (petCosts.length === 0) {
        return;
      }

      let costMap = new Map<string, number>();
      petCosts.forEach(item => {
        costMap.set(item.date, (costMap.get(item.date) || 0) + item.amount);
      });

      let points: ChartPoint[] = allDates.map((dateStr, index) => {
        let val = costMap.get(dateStr) || 0;
        let x = padding.left + (index / (totalDays - 1 || 1)) * chartW;
        let y = (H - padding.bottom) - (val * yScale);
        return {
          x: x,
          y: y,
          val: val,
          dateStr: dateStr
        } as ChartPoint;
      });

      let color = ChartThemeConfig.getPaletteColor(pet.id);
      let needFill = (this.selectedPetId !== -1);
      this.drawDirectLine(ctx, points, color, H - padding.bottom, needFill);
    });
  }

  drawDirectLine(ctx: CanvasRenderingContext2D, points: ChartPoint[], color: string, groundY: number,
    needFill: boolean) {
    if (points.length < 2) {
      return;
    }

    ctx.beginPath();
    ctx.moveTo(points[0].x, points[0].y);
    for (let i = 1; i < points.length; i++) {
      ctx.lineTo(points[i].x, points[i].y);
    }
    ctx.strokeStyle = color;
    // 【新增】线宽也需要根据密度缩放，否则在高清屏上会细如发丝
    ctx.lineWidth = 3 * this.density / 2; // 调整一个合适的比例
    ctx.lineCap = 'round';
    ctx.lineJoin = 'round';
    ctx.stroke();

    if (needFill) {
      ctx.lineTo(points[points.length - 1].x, groundY);
      ctx.lineTo(points[0].x, groundY);
      ctx.closePath();
      let grad = ctx.createLinearGradient(0, 0, 0, groundY);
      grad.addColorStop(0, color + '55');
      grad.addColorStop(1, '#FFFFFF00');
      ctx.fillStyle = grad;
      ctx.fill();
    }

    if (points.length < 15) {
      points.forEach(p => {
        if (p.val > 0) {
          ctx.beginPath();
          ctx.fillStyle = '#FFFFFF';
          // 【新增】圆点大小也要适配密度
          ctx.arc(p.x, p.y, 4 * (this.density / 2), 0, 2 * Math.PI);
          ctx.fill();
          ctx.beginPath();
          ctx.fillStyle = color;
          ctx.arc(p.x, p.y, 2.5 * (this.density / 2), 0, 2 * Math.PI);
          ctx.fill();
        }
      });
    }
  }

  drawGridAndAxes(ctx: CanvasRenderingContext2D, W: number, H: number, pad: ChartPadding, scale: YAxisScale,
    startStr: string, endStr: string) {
    // 调用更新后的动态字体方法
    const titleFont = ChartUtils.getDynamicFont(ChartThemeConfig.FONT_SIZES.title, W, true);
    const axisFont = ChartUtils.getDynamicFont(ChartThemeConfig.FONT_SIZES.axis, W);

    ctx.fillStyle = ChartThemeConfig.COLORS.textPrimary;
    ctx.font = titleFont;
    ctx.textAlign = 'left';
    // Y轴文字坐标微调
    ctx.fillText('金额', 10, 25);

    ctx.strokeStyle = ChartThemeConfig.COLORS.gridLine;
    ctx.lineWidth = 1; // 网格线细一点没关系
    ctx.fillStyle = ChartThemeConfig.COLORS.textSecondary;
    ctx.font = axisFont;
    ctx.textAlign = 'right';

    const chartH = H - pad.top - pad.bottom;
    const ticks = scale.max / scale.step;

    for (let i = 0; i <= ticks; i++) {
      let val = i * scale.step;
      let y = (H - pad.bottom) - (val / scale.max) * chartH;

      ctx.beginPath();
      ctx.moveTo(pad.left, y);
      ctx.lineTo(W - pad.right, y);
      ctx.stroke();

      ctx.fillText(val.toFixed(0), pad.left - 5, y + 4);
    }

    ctx.font = axisFont;
    ctx.textAlign = 'left';
    ctx.fillText(startStr.substring(5), pad.left, H - 5);

    ctx.textAlign = 'right';
    ctx.fillText(endStr.substring(5), W - pad.right, H - 5);

    ctx.textAlign = 'center';
    ctx.fillText('~', W / 2, H - 5);
  }

  drawEmptyState(ctx: CanvasRenderingContext2D, W: number, H: number) {
    ctx.fillStyle = '#C7C7CC';
    ctx.font = ChartUtils.getDynamicFont(ChartThemeConfig.FONT_SIZES.emptyState, W);
    ctx.textAlign = 'center';
    ctx.fillText('暂无消费数据', W / 2, H / 2);
  }

  drawPieChart(ctx: CanvasRenderingContext2D, data: PetCost[], W: number, H: number) {
    if (data.length === 0) {
      this.drawEmptyState(ctx, W, H);
      return;
    }

    let map = new Map<string, number>();
    let total = 0;
    data.forEach(c => {
      map.set(c.category, (map.get(c.category) || 0) + c.amount);
      total += c.amount;
    });

    // --- 1. 绘制饼图 (极左侧) ---
    const cx = W * ChartThemeConfig.LAYOUT.pieRadiusRatio;
    const cy = H / 2;

    // 限制半径：不能太大，否则会碰到右边的字
    // 逻辑：半径最大为宽度的 20%
    const maxR = Math.min(W * 0.20, H * 0.38);
    const r = maxR;

    let startAngle = 0;
    let keys = Array.from(map.keys());

    keys.forEach((key, index) => {
      let val = map.get(key) || 0;
      let sweep = (val / total) * 2 * Math.PI;

      ctx.beginPath();
      ctx.moveTo(cx, cy);
      ctx.arc(cx, cy, r, startAngle, startAngle + sweep);
      ctx.fillStyle = ChartThemeConfig.getCategoryColor(key);
      ctx.fill();
      startAngle += sweep;
    });

    // --- 2. 绘制图例 (双列网格布局) ---

    // 获取屏幕密度系数 (用于缩放间距)
    const scale = this.density / 2; // 如果觉得太挤，可以改成 this.density / 2.2

    // 布局参数配置
    const legendStartX = W * 0.45; // 从画布 45% 的位置开始画字
    const legendWidth = W * 0.55; // 右侧可用总宽度
    const colWidth = legendWidth / 2; // 分成两列，每列宽度

    // 调小字号：8个分类必须用小字体才好看
    // 这里的 11 和 9 是基准字号 (vp)
    const titleFont = ChartUtils.getDynamicFont(11, W, true);
    const descFont = ChartUtils.getDynamicFont(9, W);

    // 紧凑行高
    const lineHeight = 35 * scale;
    const colorBoxSize = 10 * scale;

    // 计算垂直居中起始点
    // 既然是双列，行数 = 总数 / 2 (向上取整)
    const totalRows = Math.ceil(keys.length / 2);
    const contentHeight = totalRows * lineHeight;
    const startY = (H - contentHeight) / 2;

    keys.forEach((key, index) => {
      let val = map.get(key) || 0;
      let percent = (val / total * 100).toFixed(0) + '%';

      // 【核心逻辑】计算行号和列号
      const col = index % 2; // 0 或 1
      const row = Math.floor(index / 2); // 0, 1, 2, 3...

      // 计算坐标
      // x = 起始点 + (列号 * 列宽)
      let x = legendStartX + (col * colWidth);
      // y = 起始点 + (行号 * 行高)
      let y = startY + (row * lineHeight);

      // 1. 画色块
      ctx.fillStyle = ChartThemeConfig.getCategoryColor(key);
      ctx.fillRect(x, y + (lineHeight * 0.2), colorBoxSize, colorBoxSize);

      // 2. 画类别名称 (Title)
      ctx.fillStyle = ChartThemeConfig.COLORS.textPrimary;
      ctx.font = titleFont;
      ctx.textAlign = 'left';
      // 文字紧跟色块
      ctx.fillText(key, x + colorBoxSize + (6 * scale), y + (lineHeight * 0.45));

      // 3. 画百分比 (Desc)
      ctx.fillStyle = ChartThemeConfig.COLORS.textSecondary;
      ctx.font = descFont;
      // 百分比显示在类别下方 (或者右侧，看空间)
      // 这里选择显示在下方，更稳妥
      ctx.fillText(percent, x + colorBoxSize + (6 * scale), y + (lineHeight * 0.85));
    });
  }

  async handleSave(pId: number, amt: number, cat: string, date: string) {
    await DatabaseManager.getInstance().insertCost(pId, amt, cat, date);
    await this.refreshUI();
  }

  async handleDelete(id: number) {
    await DatabaseManager.getInstance().deleteCost(id);
    await this.refreshUI();
  }

  @Builder
  TabItem(text: string, mode: string) {
    Text(text)
      .fontSize(13)
      .fontWeight(FontWeight.Medium)
      .fontColor(this.chartMode === mode ? '#000' : '#8E8E93')
      .backgroundColor(this.chartMode === mode ? Color.White : Color.Transparent)
      .borderRadius(8)
      .padding({
        left: 15,
        right: 15,
        top: 6,
        bottom: 6
      })
      .shadow(this.chartMode === mode ? { radius: 2, color: '#1A000000', offsetY: 1 } : { radius: 0 })
      .onClick(() => {
        this.chartMode = mode;
        this.triggerRedraw();
      })
  }

  @Builder
  Divider() {
    Line().width(1).height(25).backgroundColor('#E5E5EA')
  }

  build() {
    Column() {
      Row() {
        Text('花销数据看板').fontSize(24).fontWeight(FontWeight.Bolder).fontColor('#1C1C1E')
        Blank()
      }
      .width('100%')
      .padding({
        top: 20,
        bottom: 5,
        left: 20,
        right: 20
      })

      Column({ space: 8 }) {
        Row({ space: 10 }) {
          Select([{ value: '所有宠物' } as SelectOption].concat(this.pets.map(p => ({
            value: p.name
          } as SelectOption))))
            .value(this.selectedPetId === -1 ? '所有宠物' :
              this.pets.find(p => p.id === this.selectedPetId)?.name || '所有宠物')
            .onSelect((i: number) => {
              this.selectedPetId = (i === 0) ? -1 : this.pets[i-1].id;
              this.refreshUI();
            })
            .backgroundColor(Color.White)
            .borderRadius(12)
            .height(40)
            .layoutWeight(1)

          Select([{ value: '全部' } as SelectOption].concat(['伙食', '医疗', '玩具', '美容', '其他'].map(c => ({
            value: c
          } as SelectOption))))
            .value(this.selectedCategory)
            .onSelect((i: number, v: string) => {
              this.selectedCategory = v;
              this.refreshUI();
            })
            .backgroundColor(Color.White)
            .borderRadius(12)
            .height(40)
            .layoutWeight(1)

          Row() {
            this.TabItem('趋势', 'trend')
            this.TabItem('占比', 'category')
          }.backgroundColor('#E5E5EA').borderRadius(10).padding(2).height(40)
        }

        Row() {
          Button(this.startDate || '开始日期')
            .fontSize(13)
            .fontColor('#0A84FF')
            .backgroundColor(Color.White)
            .height(30)
            .onClick(() => {
              DatePickerDialog.show({
                onAccept: (v: DatePickerResult) => {
                  this.startDate =
                    `${v.year}-${((v.month ?? 0) + 1).toString().padStart(2, '0')}-${(v.day ?? 1).toString()
                      .padStart(2, '0')}`;
                  this.refreshUI();
                }
              })
            })
          Text('至').fontSize(14).fontColor('#8E8E93').margin({ left: 10, right: 10 })
          Button(this.endDate || '结束日期')
            .fontSize(13)
            .fontColor('#0A84FF')
            .backgroundColor(Color.White)
            .height(30)
            .onClick(() => {
              DatePickerDialog.show({
                onAccept: (v: DatePickerResult) => {
                  this.endDate =
                    `${v.year}-${((v.month ?? 0) + 1).toString().padStart(2, '0')}-${(v.day ?? 1).toString()
                      .padStart(2, '0')}`;
                  this.refreshUI();
                }
              })
            })

        }.justifyContent(FlexAlign.Start).width('100%')
      }.padding(12).margin({ left: 16, right: 16, bottom: 5 }).backgroundColor('#F2F2F7').borderRadius(20)

      Column() {
        Canvas(this.context)
          .width('100%')
          .height(this.actualCanvasHeight)
          .onAreaChange((oldValue: Area, newValue: Area) => {
            let newWidth = Number(newValue.width);
            if (newWidth > 0 && Math.abs(newWidth - this.actualCanvasWidth) > 1) {
              this.actualCanvasWidth = newWidth;
              this.triggerRedraw();
            }
          })
          .onReady(() => {
            if (this.actualCanvasWidth > 0) {
              this.triggerRedraw();
            }
          })

        Row() {
          Column() {
            Text('总支出').fontSize(12).fontColor('#8E8E93')
            Text(`¥${this.totalAmount.toFixed(2)}`)
              .fontSize(18)
              .fontWeight(FontWeight.Bold)
              .fontColor('#1C1C1E')
              .margin({ top: 2 })
          }

          this.Divider()
          Column() {
            Text('单笔最高').fontSize(12).fontColor('#8E8E93')
            Text(`¥${this.maxAmount.toFixed(2)}`)
              .fontSize(18)
              .fontWeight(FontWeight.Bold)
              .fontColor('#FF453A')
              .margin({ top: 2 })
          }

          this.Divider()
          Column() {
            Text('日均消费').fontSize(12).fontColor('#8E8E93')
            Text(`¥${this.avgAmount.toFixed(2)}`)
              .fontSize(18)
              .fontWeight(FontWeight.Bold)
              .fontColor('#0A84FF')
              .margin({ top: 2 })
          }
        }.width('100%').justifyContent(FlexAlign.SpaceEvenly).padding({ top: 5, bottom: 5 })

      }
      .padding(10)
      .margin({ left: 16, right: 16, bottom: 10 })
      .backgroundColor(Color.White)
      .borderRadius(24)
      .shadow({ radius: 20, color: '#1A000000', offsetY: 5 })

      List({ space: 10 }) {
        ListItem() {
          Text('近期明细').fontSize(18).fontWeight(FontWeight.Bold).margin({ left: 5, top: 5 })
        }

        ForEach(this.costs, (item: PetCost) => {
          ListItem() {
            Row() {
              Stack() {
                Circle({ width: 40, height: 40 }).fill(ChartThemeConfig.getCategoryColor(item.category) + '20')
                Text(item.category.substring(0, 1))
                  .fontSize(16)
                  .fontColor(ChartThemeConfig.getCategoryColor(item.category))
                  .fontWeight(FontWeight.Bold)
              }.margin({ right: 12 })

              Column() {
                Row() {
                  Text(item.category).fontSize(16).fontWeight(FontWeight.Medium)
                  Text(this.pets.find(p => p.id === item.petId)?.name || '未知')
                    .fontSize(12)
                    .fontColor('#FFF')
                    .backgroundColor('#C7C7CC')
                    .borderRadius(4)
                    .padding({
                      left: 4,
                      right: 4,
                      top: 1,
                      bottom: 1
                    })
                    .margin({ left: 8 })
                }

                Text(item.date).fontSize(13).fontColor('#8E8E93').margin({ top: 2 })
              }.layoutWeight(1).alignItems(HorizontalAlign.Start)

              Column() {
                Text(`- ¥${item.amount.toFixed(2)}`).fontSize(18).fontWeight(FontWeight.Bold).fontColor('#1C1C1E')
              }.alignItems(HorizontalAlign.End)

              Text('🗑️').fontSize(16).opacity(0.4).margin({ left: 10 }).onClick(() => this.handleDelete(item.id))
            }.padding(12).backgroundColor(Color.White).borderRadius(16)
          }
        }, (item: PetCost) => item.id.toString())
      }.layoutWeight(1).padding({ left: 16, right: 16, bottom: 80 })

      Button() {
        Row() {
          Text('+').fontSize(24).fontColor(Color.White).fontWeight(FontWeight.Lighter).margin({ right: 5 })
          Text('记一笔').fontSize(16).fontColor(Color.White).fontWeight(FontWeight.Bold)
        }
      }
      .width(140)
      .height(50)
      .backgroundColor('#0A84FF')
      .position({ x: '50%', y: '88%' })
      .markAnchor({ x: 70, y: 0 })
      .shadow({ radius: 10, color: '#4D0A84FF', offsetY: 5 })
      .onClick(() => this.dialogController.open())
    }.width('100%').height('100%').backgroundColor('#F2F2F7')
  }
}