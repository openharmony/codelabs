import { BusinessError } from '@kit.BasicServicesKit';

@Component
export default struct BackContainer {
  @Prop header: string = "battery";
  private backImgRes: string | Resource = $r('app.media.ic_public_back');
  @BuilderParam closer?: () => void;

  build() {
    Row() {
      Button() {
        Image(this.backImgRes ? this.backImgRes : $r('app.media.ic_public_back'))
          .objectFit(ImageFit.Contain) // 通常返回图标建议用 Contain 防止变形
          .width('100%')
          .height('100%')
      }
      .backgroundColor(Color.Transparent)
      .width(24)
      .height(24)
      .margin({ right: 16 })
      .onClick(() => {
        console.info(`Succeeded in clicking the 'Back' button.`)
        let uiContext : UIContext = this.getUIContext();
        let router = uiContext.getRouter();
        try {
          // 返回第一页
          router.back()
          console.info('Succeeded in returning to the first page.')
        } catch (err) {
          let code = (err as BusinessError).code;
          let message = (err as BusinessError).message;
          console.error(`Failed to return to the first page.Code is ${code}, message is ${message}`)
        }
      })

      Text(this.header)
      // .fontSize($r('app.float.page_text_font_size'))
      // .fontWeight(FontWeight.Bold)
      // .layoutWeight(1)
        .fontSize(18)
        .fontWeight(FontWeight.Medium)
        .textAlign(TextAlign.Start)

      if (this.closer) {
        this.closer();
      }
    }
    .padding({
      left: 24,
      right: 24
    })
    .height(56)
    .width("100%")
  }
}
