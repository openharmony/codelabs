
import router from '@system.router'

@Component
struct Item {

// 文本信息
  private text: string

// 页面跳转uri
  private uri: string

  build() {
    Stack({ alignContent: Alignment.Center }) {
      Image($r('app.media.image3'))
        .objectFit(ImageFit.Cover)
        .width('100%')
        .height(100)
        .borderRadius(15)

      Text(this.text)
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .fontColor(Color.Black)
    }
    .onClick(() => {
      router.push({ uri: this.uri })
    })
    .height(100)
    .borderRadius(15)
    .width('80%')
    .margin({ bottom: 20 })
  }
}

@Entry
@Component
struct Index {
  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Item({ text: '页面间转场：底部滑入', uri: 'pages/page/BottomTransition' })
      Item({ text: '页面间转场：自定义1', uri: 'pages/page/CustomTransition' })
      Item({ text: '页面间转场：自定义2', uri: 'pages/page/FullCustomTransition' })
      Item({ text: '组件内转场', uri: 'pages/ComponentTransition' })
      Item({ text: '共享元素转场', uri: 'pages/share/ShareItem' })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#FFECECEC')
  }
}