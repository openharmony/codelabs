export class PieData {
  name: string;
  count: number;
  color: string;

  constructor(name: string, count: number, color: string) {
    this.name = name;
    this.count = count;
    this.color = color;
  }
}

@Component
export struct PieChart {
  @Prop @Watch('onDataChange') data: PieData[];

  private settings: RenderingContextSettings = new RenderingContextSettings(true);
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings);

  onDataChange() {
    this.draw(); // 数据变了重画
  }

  build() {
    Column() {
      Text("习惯占比分析")
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .width('100%')
        .margin({ bottom: 10 })

      Row() {
        // 左侧：画布
        Canvas(this.context)
          .width(140)
          .height(140)
          .onReady(() => {
            this.draw();
          })

        // 右侧：图例 (Legend)
        Column({ space: 8 }) {
          ForEach(this.data, (item: PieData) => {
            Row() {
              // 1. 颜色圆点
              Circle({ width: 8, height: 8 }).fill(item.color)

              // 2. 标签名称
              Text(item.name)
                .fontSize(12)
                .fontColor('#666')
                .margin({ left: 6 })

              // 🔴 修改点：删除了 Blank()，防止两端对齐

              // 3. 具体数量 (增加左边距，保持适当间隔)
              Text(item.count.toString())
                .fontSize(12)
                .fontWeight(FontWeight.Bold)
                .margin({ left: 12 }) // 👈 这里的 12 可以根据喜好调整
            }
            .width('100%')
            // 默认对齐方式就是 FlexAlign.Start (从左到右紧挨着)，所以不需要额外设置
          })
        }
        .layoutWeight(1)
        .padding({ left: 20 })
      }
    }
    .padding(15)
    .backgroundColor(Color.White)
    .borderRadius(16)
    .width('100%')
  }

  // 🎨 核心绘图逻辑 (保持不变)
  draw() {
    // 1. 计算总数
    let total = 0;
    this.data.forEach(item => total += item.count);

    // 如果没有数据，画个灰色的圆
    if (total === 0) {
      this.context.clearRect(0, 0, 140, 140);
      this.context.beginPath();
      this.context.arc(70, 70, 60, 0, 2 * Math.PI);
      this.context.fillStyle = '#F0F0F0';
      this.context.fill();
      return;
    }

    // 2. 开始画扇形
    let startAngle = -0.5 * Math.PI; // 从 12 点钟方向开始
    let centerX = 70;
    let centerY = 70;
    let radius = 60;

    this.context.clearRect(0, 0, 140, 140);

    for (let i = 0; i < this.data.length; i++) {
      let item = this.data[i];
      // 计算这个扇形占的弧度
      let sliceAngle = (item.count / total) * 2 * Math.PI;
      let endAngle = startAngle + sliceAngle;

      this.context.beginPath();
      this.context.moveTo(centerX, centerY); // 移动到圆心
      this.context.arc(centerX, centerY, radius, startAngle, endAngle);
      this.context.closePath();

      this.context.fillStyle = item.color;
      this.context.fill();

      // 给扇形加个白边，看起来有间隙
      this.context.lineWidth = 2;
      this.context.strokeStyle = '#FFFFFF';
      this.context.stroke();

      startAngle = endAngle; // 下一个扇形的起点是当前的终点
    }

    // 3. (可选) 画中间的白洞，做成甜甜圈效果
    this.context.beginPath();
    this.context.arc(centerX, centerY, 30, 0, 2 * Math.PI);
    this.context.fillStyle = '#FFFFFF';
    this.context.fill();
  }
}