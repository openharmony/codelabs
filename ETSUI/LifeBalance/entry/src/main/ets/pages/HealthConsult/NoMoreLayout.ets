/**
 * The No more data layout component.
 */
@Component
export default struct NoMoreLayout {
  // 1. 接收缩放比例
  @Prop fontScale: number = 1.0;

  build() {
    Row() {
      Text('没有更多数据了')
        .margin({ left: 8 })
        // 2. 使用动态大小，替换原来的 '16fp'
        .fontSize(16 * this.fontScale)
        .textAlign(TextAlign.Center)
        .fontColor('#666')
    }
    .width('100%')
    .justifyContent(FlexAlign.Center)
    // 容器高度也可以适当适配，或者保持固定
    .height(70 * this.fontScale)
  }
}