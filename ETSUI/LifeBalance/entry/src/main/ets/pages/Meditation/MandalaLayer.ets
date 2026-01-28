// ---------------- MandalaLayer.ets ----------------
@Component
export struct MandalaLayer {

  private settings: RenderingContextSettings =
    new RenderingContextSettings(true)

  private context: CanvasRenderingContext2D =
    new CanvasRenderingContext2D(this.settings)

  public attribute: MandalaLayerAttribute =
    new MandalaLayerAttribute()

  // ===== 动画 State =====
  @State private scaleValue: number = 1.0

  // 动画时长（秒）
  @Prop duration: number

  private canvasSize(): number {
    return (
      this.attribute.baseRadius +
        (this.attribute.layerCount - 1) * this.attribute.radiusStep +
        this.attribute.ringWidth * 2
    ) * 2
  }

  build() {
    Canvas(this.context)
      .width(this.canvasSize())
      .height(this.canvasSize() + this.attribute.baseRadius * this.attribute.lowerBiasFactor)
      .scale({ x: this.scaleValue, y: this.scaleValue })
      .onReady(() => {
        this.draw()
        this.startScaleAnimation()
      })
  }

  private startScaleAnimation() {
    animateTo(
      {
        duration: 3000,
        curve: Curve.EaseInOut,
        iterations: -1,
        playMode: PlayMode.Alternate
      },
      () => {
        this.scaleValue = 1.5
      }
    )
  }

  private draw() {
    const size = this.canvasSize()
    this.context.clearRect(0, 0, size, size)

    const center =
      this.attribute.baseRadius +
        (this.attribute.layerCount - 1) * this.attribute.radiusStep +
      this.attribute.ringWidth

    const ringWidth =
      this.attribute.ringWidth

    // ⭐ 下半圆整体 Y 偏移（核心）
    const lowerYOffset =
      this.attribute.baseRadius * this.attribute.lowerBiasFactor
    // const lowerYOffset = 0

    // ⭐ 下半圆 dot 数等差增长
    const dotStep = 3

    for (let i = 0; i < this.attribute.layerCount; i++) {
      const radius =
        this.attribute.baseRadius + i * this.attribute.radiusStep

      // ===== 上半圆 Ring（不变）=====
      this.context.strokeStyle = this.attribute.ringColor.toString()
      this.context.lineWidth = ringWidth
      this.context.beginPath()
      this.context.arc(center, center, radius, Math.PI, Math.PI * 2)
      this.context.stroke()

      // ===== 下半圆 DotRing（整体下移）=====
      this.context.fillStyle = this.attribute.ringColor.toString()

      const count =
        this.attribute.dotCount + i * dotStep

      const dotRadius = ringWidth / 2

      for (let j = 0; j < count; j++) {
        const angle = (Math.PI / (count - 1)) * j

        const x =
          center + radius * Math.cos(angle)

        const y =
          center + radius * Math.sin(angle) + lowerYOffset

        this.context.beginPath()
        this.context.arc(x, y, dotRadius, 0, Math.PI * 2)
        this.context.fill()
      }
    }
  }

  aboutToAppear() {
    this.attribute = MandalaLayerAttribute.filter(this.attribute)
  }
}

// ---------------- MandalaLayerAttribute ----------------
class MandalaLayerAttribute {

  public baseRadius: number = 20
  public layerCount: number = 3
  public radiusStep: number = 20
  public ringWidth: number = 15
  public dotCount: number = 4

  // ⭐ 下半圆 Y 偏移系数（相对于 ringWidth）
  public lowerBiasFactor: number = 0.4

  public ringColor: Color | number | string | Resource = '#d0e9c6'

  public static filter(attr: MandalaLayerAttribute): MandalaLayerAttribute {
    if (!attr) return new MandalaLayerAttribute()

    const def = new MandalaLayerAttribute()
    attr.baseRadius ??= def.baseRadius
    attr.layerCount ??= def.layerCount
    attr.radiusStep ??= def.radiusStep
    attr.ringWidth ??= def.ringWidth
    attr.dotCount ??= def.dotCount
    attr.lowerBiasFactor ??= def.lowerBiasFactor
    attr.ringColor ??= def.ringColor

    return attr
  }
}
