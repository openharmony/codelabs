
import { Item, ItemUseStatus, DEFAULT_CATEGORY_OPTIONS } from '../model/Item';
import curves from '@ohos.curves';

/**
 * 颜色配置中心 - 集中管理全站配色
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

  static readonly RAINBOW_COLORS: string[] = [
    "#FF3B30", "#FF9500", "#FFCC00", "#34C759", "#30B0C7",
    "#007AFF", "#5856D6", "#AF52DE", "#FF2D55"
  ];

  static readonly STATUS_COLORS: Record<string, string> = {
    "在用": "#34C759", // Green
    "闲置": "#FF9500", // Orange
    "已丢弃": "#8E8E93"  // Gray
  };
}

/**
 * 分类统计数据模型
 */
class CategoryStat {
  name: string = "";
  count: number = 0;
  totalPrice: number = 0;
  percentage: number = 0;
  color: string = "";
}

/**
 * 状态统计数据模型
 */
class StatusStat {
  status: ItemUseStatus = "在用";
  count: number = 0;
  percentage: number = 0;
  color: string = "";
}

/**
 * 位置词云数据模型
 */
class PositionStat {
  name: string = "";
  count: number = 0;
  fontSize: number = 12;
  fontWeight: number = 400;
  color: string = "";
  opacity: number = 1.0; // 用于选中时的动画状态
}

/**
 * 粒子动画对象 - 用于顶部 Header 的气泡特效
 */
class BubbleParticle {
  x: number;
  y: number;
  radius: number;
  speedY: number;
  opacity: number;

  constructor(width: number, height: number) {
    this.x = Math.random() * width;
    this.y = height + Math.random() * 20; // 从底部下方生成
    this.radius = Math.random() * 10 + 5;
    this.speedY = Math.random() * 0.5 + 0.2;
    this.opacity = Math.random() * 0.3 + 0.1;
  }

  update(height: number) {
    this.y -= this.speedY;
    if (this.y < -20) {
      this.y = height + 20; // 循环复用
      this.x = Math.random() * 300; // 简单假设宽度，实际会动态调整
    }
  }
}


/**
 * 组件：通用卡片容器
 * 封装了投影、圆角和背景色
 */
@Component
struct BaseCard {
  @BuilderParam content: () => void = this.emptyBuilder;
  @Prop title: string = "";
  @Prop subTitle: string = "";

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
        }
        .width("100%")
        .margin({ bottom: 20 })
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

/**
 * 组件：顶部动态资产概览
 * 包含 Canvas 粒子背景特效
 */
@Component
struct AssetHeader {
  @Prop totalValue: number = 0;
  @Prop totalCount: number = 0;

  // 动画相关
  private settings: RenderingContextSettings = new RenderingContextSettings(true);
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings);
  private particles: BubbleParticle[] = [];
  private timerId: number = -1;
  private canvasWidth: number = 300; // 初始值
  private canvasHeight: number = 150;

  aboutToAppear() {
    // 初始化粒子
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
    }, 20); // 50fps
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
      // 1. 动态背景层
      Canvas(this.context)
        .width("100%")
        .height("100%")
        .onReady(() => {
          this.canvasWidth = this.context.width;
          this.canvasHeight = this.context.height;
          this.startAnimation();
        })
        .hitTestBehavior(HitTestMode.None) // 不拦截点击

      // 2. 渐变背景色
      Column()
        .width("100%")
        .height("100%")
        .linearGradient({
          angle: 135,
          colors: [[ThemeConfig.PRIMARY, 0.0], [ThemeConfig.PRIMARY_DARK, 1.0]]
        })
        .opacity(0.9) // 让粒子透出来一点

      // 3. 内容层
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
    .clip(true) // 裁剪超出部分
    .shadow({ radius: 20, color: "#404A6CF7", offsetY: 10 })
    .margin({ bottom: 24 })
  }
}

/**
 * 组件：分类图例行
 * 单个分类的显示条目
 */
@Component
struct CategoryLegendItem {
  @Prop stat: CategoryStat;

  build() {
    Row() {
      Circle({ width: 10, height: 10 })
        .fill(this.stat.color)
        .margin({ right: 10 })

      Text(this.stat.name)
        .fontSize(14)
        .fontColor(ThemeConfig.TEXT_SECONDARY)
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
    .border({ width: { bottom: 0.5 }, color: "#F0F0F0" })
  }
}

/**
 * 组件：可交互的存放位置标签云
 * 支持点击筛选
 */
@Component
struct InteractiveWordCloud {
  @Prop stats: PositionStat[];
  @Prop selectedLocation: string; // 当前选中的位置
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
          .animation({ duration: 300, curve: curves.springMotion() }) // 添加动画
          .onClick(() => {
            // 点击切换选中状态
            if (this.selectedLocation === stat.name) {
              this.onLocationSelect(""); // 取消选中
            } else {
              this.onLocationSelect(stat.name);
            }
          })
      })

      // 显示清除按钮（如果有选中）
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

/**
 * 组件：高级列表项
 * 包含详细信息的卡片设计
 */
@Component
struct ComplexItemCard {
  @Prop item: Item;

  // 辅助函数：日期格式化
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
      // 左侧装饰条
      Column()
        .width(4)
        .height(50)
        .backgroundColor(ThemeConfig.STATUS_COLORS[this.item.useStatus] || "#999")
        .borderRadius(2)
        .margin({ right: 12 })

      // 主体内容
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

        // 详细信息行
        Row() {
          // 分类标签
          Text(this.item.category)
            .fontSize(10)
            .fontColor(Color.White)
            .backgroundColor("#B0B0B0")
            .padding({ left: 6, right: 6, top: 2, bottom: 2 })
            .borderRadius(4)
            .margin({ right: 8 })

          // 位置
          // Image($r('app.media.icon_location_fill')) // 如果没有图标，用文字代替逻辑
          //   .width(12).height(12)
          //   .fillColor(ThemeConfig.TEXT_LIGHT)
          //   .visibility(Visibility.None)

          Text(this.item.location || "未归位")
            .fontSize(12)
            .fontColor(ThemeConfig.TEXT_SECONDARY)
            .margin({ right: 12 })

          // 日期
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
export struct StatisticsPage {
  @Prop itemList: Item[] = [];

  // ================= 状态管理 =================
  @State animatedValues: number[] = [];
  @State currentStatusFilter: number = 0; // 0:全部, 1:在用, 2:闲置, 3:已丢弃
  @State currentLocationFilter: string = ""; // 存放位置筛选（空字符串表示不筛选）

  @State sortField: 'price' | 'time' = 'time';
  @State isAscending: boolean = false;

  @State isListReady: boolean = false; // 用于控制列表入场动画

  private readonly FILTER_TABS: string[] = ["全部", "在用", "闲置", "已丢弃"];
  private scroller: Scroller = new Scroller();


  /**
   * 计算资产总值
   */
  private getTotalPrice(): number {
    return this.itemList.reduce((sum, item) => sum + item.price, 0);
  }

  /**
   * 生成分类图表数据
   */
  private getCategoryStats(): CategoryStat[] {
    const total = this.itemList.length;
    if (total === 0) return [];

    const statsMap = new Map<string, CategoryStat>();
    DEFAULT_CATEGORY_OPTIONS.forEach(cat => {
      statsMap.set(cat, {
        name: cat, count: 0, totalPrice: 0, percentage: 0, color: "#DDD"
      });
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
   * 生成状态进度条数据
   */
  private getStatusStats(): StatusStat[] {
    const total = this.itemList.length;
    if (total === 0) return [];

    const statuses: ItemUseStatus[] = ["在用", "闲置", "已丢弃"];
    let result: StatusStat[] = [];

    statuses.forEach(status => {
      const count = this.itemList.filter(item => item.useStatus === status).length;
      result.push({
        status: status,
        count: count,
        percentage: (count / total) * 100,
        color: ThemeConfig.STATUS_COLORS[status] || "#333"
      });
    });

    return result;
  }

  /**
   * 生成位置词云数据
   */
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
      // 动态计算大小
      const minSize = 12;
      const maxSize = 24;
      const scale = maxCount > 1 ? (count - 1) / (maxCount - 1) : 1;

      // 选中状态特殊处理
      const isSelected = this.currentLocationFilter === name;

      result.push({
        name: name,
        count: count,
        fontSize: minSize + (scale * (maxSize - minSize)),
        fontWeight: 400 + Math.floor(scale * 300),
        color: ThemeConfig.RAINBOW_COLORS[colorIndex % ThemeConfig.RAINBOW_COLORS.length],
        opacity: (this.currentLocationFilter && !isSelected) ? 0.3 : 1.0 // 未被选中时淡化
      });
      colorIndex++;
    });

    // 排序：数量多的在中间或前面
    return result.sort((a, b) => b.count - a.count);
  }

  /**
   * 核心筛选与排序逻辑
   */
  private getFilteredAndSortedItems(): Item[] {
    let result = this.itemList;

    // 1. 状态筛选
    if (this.currentStatusFilter === 1) result = result.filter(i => i.useStatus === '在用');
    else if (this.currentStatusFilter === 2) result = result.filter(i => i.useStatus === '闲置');
    else if (this.currentStatusFilter === 3) result = result.filter(i => i.useStatus === '已丢弃');

    // 2. 位置筛选 (新增联动)
    if (this.currentLocationFilter) {
      result = result.filter(i => {
        const iLoc = i.location && i.location.trim() !== "" ? i.location : "未归位";
        return iLoc === this.currentLocationFilter;
      });
    }

    // 3. 排序
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
    const targetValues = this.getCategoryStats().map(s => s.count);
    this.animatedValues = targetValues.map(() => 0);
    // 延迟一点触发
    setTimeout(() => {
      animateTo({ duration: 800, curve: curves.cubicBezierCurve(0.2, 0.0, 0.2, 1.0) }, () => {
        this.animatedValues = targetValues;
      })
    }, 200);
  }

  private getDataPanelColors(): ResourceColor[] {
    return this.getCategoryStats().map(s => s.color);
  }


  build() {
    Column() {
      // 唯一的滚动容器
      List({ scroller: this.scroller }) {

        ListItem() {
          AssetHeader({
            totalValue: this.getTotalPrice(),
            totalCount: this.itemList.length
          })
            .padding({ left: 16, right: 16, top: 16, bottom: 8 }) // 调整一下间距
        }

        if (this.itemList.length === 0) {
          // --- 空状态 ---
          ListItem() {
            Column() {
              // Image($r('app.media.icon_empty_data'))
              //   .width(100).height(100)
              //   .opacity(0.5)
              //   .margin({ bottom: 16 })
              Text("暂无物品数据")
                .fontSize(16)
                .fontWeight(FontWeight.Bold)
                .fontColor(ThemeConfig.TEXT_SECONDARY)
<<<<<<< HEAD
              Text("点击左下角添加你的第一件物品吧")
=======
              Text("点击右下角添加你的第一件物品吧")
>>>>>>> d0bdc00cacd020ce75470fda7a548f84d8adc874
                .fontSize(14)
                .fontColor(ThemeConfig.TEXT_LIGHT)
                .margin({ top: 8 })
            }
            .width("100%")
            .height(400)
            .justifyContent(FlexAlign.Center)
          }
        } else {

          // --- 图表区: 分类 ---
          ListItem() {
            BaseCard({ title: "物品分类", subTitle: "查看各类目占比情况" }) {
              Row() {
                // 左侧环形图
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

                // 右侧图例列表
                Column() {
                  ForEach(this.getCategoryStats().slice(0, 5), (stat: CategoryStat) => {
                    CategoryLegendItem({ stat: stat })
                  })
                }
                .layoutWeight(6)
                .padding({ left: 16 })
              }
            }
          }
          .padding({ left: 16, right: 16 })

          // --- 图表区: 状态 ---
          ListItem() {
            BaseCard({ title: "使用状态", subTitle: "物品的使用状态分布" }) {
              Column({ space: 16 }) {
                ForEach(this.getStatusStats(), (stat: StatusStat) => {
                  Column() {
                    Row() {
                      Text(stat.status)
                        .fontSize(14)
                        .fontColor(ThemeConfig.TEXT_MAIN)
                      Blank()
                      Text(`${stat.count}件`)
                        .fontSize(14)
                        .fontWeight(FontWeight.Bold)
                        .fontColor(stat.color)
                    }
                    .width("100%")
                    .margin({ bottom: 8 })

                    // 自定义进度条背景
                    Stack({ alignContent: Alignment.Start }) {
                      // 背景
                      Row()
                        .width("100%")
                        .height(8)
                        .backgroundColor("#F0F0F0")
                        .borderRadius(4)

                      // 前景
                      Row()
                        .width(`${stat.percentage}%`)
                        .height(8)
                        .backgroundColor(stat.color)
                        .borderRadius(4)
                        .animation({ duration: 1000, curve: Curve.EaseOut }) // 进度条动画
                    }
                  }
                })
              }
            }
          }
          .padding({ left: 16, right: 16 })

          // --- 图表区: 词云 ---
          ListItem() {
            BaseCard({
              title: "空间分布图谱",
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

          // --- 列表区 Header: 筛选与排序 ---
          ListItem() {
            Column() {
              Text("物品明细")
                .fontSize(20)
                .fontWeight(FontWeight.Bold)
                .margin({ bottom: 12, left: 4 })

              // 1. 状态筛选 Tabs
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

              // 2. 排序与计数栏
              Row() {
                Text(`共 ${this.getFilteredAndSortedItems().length} 条数据`)
                  .fontSize(12)
                  .fontColor(ThemeConfig.TEXT_LIGHT)
                  .layoutWeight(1)

                // 排序按钮组
                Row() {
                  // 价格排序
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

                  // 时间排序
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
            .backgroundColor(ThemeConfig.BACKGROUND) // 防止透明透出下面的内容
          }
          // .sticky(Sticky.Normal)

          // --- 列表区内容 ---
          ForEach(this.getFilteredAndSortedItems(), (item: Item, index: number) => {
            ListItem() {
              ComplexItemCard({ item: item })
                .margin({ left: 16, right: 16 })
                .opacity(this.isListReady ? 1 : 0)
                .translate({ y: this.isListReady ? 0 : 20 })
                .animation({
                  duration: 400,
                  delay: index * 50,
                  curve: Curve.EaseOut
                })
            }
          }, (item: Item) => `${item.id}_${item.useStatus}_${item.price}`)

          // 底部留白
          ListItem().height(80)
        }
      }
      .width("100%")
      .height("100%")
      .edgeEffect(EdgeEffect.Spring)
      // 如果你想让筛选栏吸顶，需要开启 sticky
      // .sticky(StickyStyle.Header)
    }
    .width("100%")
    .height("100%")
    .backgroundColor(ThemeConfig.BACKGROUND)
    .onVisibleAreaChange([0.0, 1.0], (isVisible: boolean, ratio: number) => {
      if (isVisible && ratio > 0) {
        this.triggerChartsAnimation();
        setTimeout(() => { this.isListReady = true; }, 100);
      }
    })
  }
}