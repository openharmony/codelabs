// @ts-nocheck
import router from '@system.router';

@Entry
@Component
struct there {

  @State readText: string= (router.getParams()==null?"default":router.getParams().cvs)

  build() {
    Column() {
      Text($r('app.string.title'))
        .width('100%')
        .height(50)
        .backgroundColor('#0D9FFB')
        .fontColor(Color.White)
        .fontSize(20)
        .padding({ left: 15 })

      Text(this.readText)
        .width('100%')
        .height(200)
        .backgroundColor('#DFDFDF')
        .fontSize(20)
        .margin({ top: 20 })
        .textAlign(TextAlign.Start)
        .padding(10)
      }
      .width('100%').height('100%')
      .padding(15)
  }
}