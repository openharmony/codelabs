@Component
export struct NavTitle {
  @Prop title: string
  build() {
    Row() {
      Text(this.title)
        .fontSize(16)
        .fontWeight(600)

    }.width('100%')
    .height(40)
    .backgroundColor($r('app.color.main_background'))
    .justifyContent(FlexAlign.Center)
    .alignItems(VerticalAlign.Center)

  }
}