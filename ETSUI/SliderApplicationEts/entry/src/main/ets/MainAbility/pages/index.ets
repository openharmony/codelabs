@Entry
@Component
struct Index {
  @State private speed: number = 5
  @State private imageSize: number = 1
  @State private angle: number = 0
  @State private interval: number = 0

  @Builder DescribeText(text:string, speed: number) {
    Stack() {
      Text(text + speed.toFixed(1))
        .margin({ top: 30 })
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
    }
  }

  build() {
    Column() {
      Row(){
        Image($r('app.media.windmill'))
          .objectFit(ImageFit.Contain)
          .height(150)
          .width(150)
          .position({ x: 110, y: 100 })
          .rotate({ x: 0, y: 0, z: 1, angle: this.angle })
          .scale({ x: this.imageSize, y: this.imageSize })
      }
      .width(375)
      .height(375)

      this.DescribeText('速度：', this.speed)

      Slider({
        value: this.speed,
        min: 1,
        max: 10,
        step: 1,
        style: SliderStyle.OUTSET
      })
        .showTips(true)
        .blockColor(Color.Blue)
        .onChange((value: number, mode: SliderChangeMode) => {
          this.speed = value
          console.log("value:" + value);
          clearInterval(this.interval)
          this.speedChange()
        })

      this.DescribeText('缩放比例：', this.imageSize)

      Slider({
        value: this.imageSize,
        min: 0.5,
        max: 2.5,
        step: 0.1,
        style: SliderStyle.OUTSET
      })
        .showTips(true)
        .blockColor(Color.Blue)
        .onChange((value: number, mode: SliderChangeMode) => {
          this.imageSize = value
        })
    }
    .margin({ left: 30, right: 30 })
  }

  speedChange() {
    var that = this;
    that.angle = 0;
    this.interval = setInterval(function () {
      that.angle += that.speed
    }, 15)
  }

  onPageShow() {
    clearInterval(this.interval)
    this.speedChange()
  }
}
