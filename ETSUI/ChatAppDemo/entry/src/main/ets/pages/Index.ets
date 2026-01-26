import router from '@ohos.router'

@Entry
@Component
struct Index {

  build() {
    Column() {
      Image($r('app.media.launch'))
        .width('100%')
        .height('80%')

      Row() {
        Button('登录')
          .commonBtn()
          .backgroundColor(Color.Green)
          .fontColor(Color.White)
          .onClick(() => {
            router.replaceUrl({
              url: 'pages/Login'
            })
          })

        Button('注册')
          .commonBtn()
          .backgroundColor(Color.White)
          .fontColor(Color.Grey)
          .onClick(() => {
            router.pushUrl({
              url: 'pages/Register'
            })
          })

      }.width('100%')
      .justifyContent(FlexAlign.SpaceBetween)
      .padding({left: 20, right: 20})
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#020204')
    .justifyContent(FlexAlign.Center)
  }
}

@Extend(Button) function commonBtn() {
  .type(ButtonType.Normal)
  .fontSize(18)
  .width(120)
  .borderRadius(10)
}