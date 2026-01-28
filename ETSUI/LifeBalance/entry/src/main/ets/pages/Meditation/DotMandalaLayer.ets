// ---------------- DotMandalaLayer.ets ----------------
@Component
export struct DotMandalaLayer {

  private settings: RenderingContextSettings =
    new RenderingContextSettings(true)

  private context: CanvasRenderingContext2D =
    new CanvasRenderingContext2D(this.settings)

  public attribute: DotMandalaLayerAttribute =
    new DotMandalaLayerAttribute()

  // ===== 动画 State =====
  @State private scaleValue: number = 1.0

  // 动画时长（秒）
  @Prop duration: number = 6

  private canvasSize(): number {
    return (
      this.attribute.baseRadius +
        (this.attribute.layerCount - 1) * this.attribute.radiusStep +
        this.attribute.maxDotRadius * 2
    ) * 2
  }

  build() {
    Canvas(this.context)
      .width(this.canvasSize())
      .height(this.canvasSize())
      .scale({ x: this.scaleValue, y: this.scaleValue })
      .onReady(() => {
        this.draw()
        this.startScaleAnimation()
      })
  }

  private startScaleAnimation() {
    animateTo(
      {
        duration: this.duration * 1000,
        curve: Curve.EaseInOut,
        iterations: -1,
        playMode: PlayMode.Alternate
      },
      () => {
        this.scaleValue = 1.3
      }
    )
  }

  private draw() {
    const size = this.canvasSize()
    this.context.clearRect(0, 0, size, size)

    const center = size / 2

    for (let i = 0; i < this.attribute.layerCount; i++) {
      const radius =
        this.attribute.baseRadius + i * this.attribute.radiusStep

      const count =
        this.attribute.baseDotCount + i * this.attribute.dotStep

      // 计算每层圆点半径，从 innerRadius 到 maxRadius
      const dotRadius =
        this.attribute.minDotRadius +
          ((this.attribute.maxDotRadius - this.attribute.minDotRadius) / (this.attribute.layerCount - 1)) * i

      // 计算渐变颜色
      const t = i / (this.attribute.layerCount - 1)
      const color = this.interpolateColor(
        this.attribute.startColor,
        this.attribute.endColor,
        t
      )
      this.context.fillStyle = color

      for (let j = 0; j < count; j++) {
        const angle = (2 * Math.PI / count) * j
        const x = center + radius * Math.cos(angle)
        const y = center + radius * Math.sin(angle)

        this.context.beginPath()
        this.context.arc(x, y, dotRadius, 0, Math.PI * 2)
        this.context.fill()
      }
    }
  }

  // 颜色插值函数：线性渐变
  private interpolateColor(start: string, end: string, t: number): string {
    const parseHex = (hex: string) => {
      hex = hex.replace('#', '')
      if (hex.length === 3) hex = hex.split('').map((h) => { return h + h }).join('')
      const bigint = parseInt(hex, 16)
      return [(bigint >> 16) & 255, (bigint >> 8) & 255, bigint & 255]
    }

    const color1 = parseHex(start)
    const color2 = parseHex(end)

    const r = Math.round(color1[0] + (color2[0] - color1[0]) * t)
    const g = Math.round(color1[1] + (color2[1] - color1[1]) * t)
    const b = Math.round(color1[2] + (color2[2] - color1[2]) * t)

    return 'rgb(' + r + ',' + g + ',' + b + ')'
  }

  aboutToAppear() {
    this.attribute = DotMandalaLayerAttribute.filter(this.attribute)
  }
}

// ---------------- GradientDotMandalaLayerAttribute ----------------
class DotMandalaLayerAttribute {

  public baseRadius: number = 20       // 内层圆半径
  public layerCount: number = 5        // 圆环层数
  public radiusStep: number = 20       // 每层半径增加值
  public baseDotCount: number = 6      // 内层圆点数
  public dotStep: number = 2           // 外层比内层多圆点数
  public minDotRadius: number = 3      // 内层圆点半径
  public maxDotRadius: number = 8      // 外层圆点半径
  public startColor: string = '#b2f2bb' // 浅绿
  public endColor: string = '#ff83ba8c'   // 深绿

  public static filter(attr: DotMandalaLayerAttribute): DotMandalaLayerAttribute {
    if (!attr) return new DotMandalaLayerAttribute()

    const def = new DotMandalaLayerAttribute()
    attr.baseRadius ??= def.baseRadius
    attr.layerCount ??= def.layerCount
    attr.radiusStep ??= def.radiusStep
    attr.baseDotCount ??= def.baseDotCount
    attr.dotStep ??= def.dotStep
    attr.minDotRadius ??= def.minDotRadius
    attr.maxDotRadius ??= def.maxDotRadius
    attr.startColor ??= def.startColor
    attr.endColor ??= def.endColor

    return attr
  }
}
