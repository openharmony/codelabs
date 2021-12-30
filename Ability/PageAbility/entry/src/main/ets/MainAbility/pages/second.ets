@Entry
@Component
struct Second {
  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Button() {
        Text('This is ability1 second page')
          .fontSize(25)
          .fontWeight(FontWeight.Medium)
          .margin({left:20,right:20})
      }
      .padding({ top: 20, left: 20, right: 20, bottom: 20 })
    }
    .width('100%')
    .height('100%')
  }
}