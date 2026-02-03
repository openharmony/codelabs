import { Setting } from "./Setting"
import { AudioRecordPage } from "./sound"
import {GalleryMainPage} from "./mainPage"


@Entry
@Component
struct Page {
  @State message: string = 'Hello World';

  build() {
    Tabs({ barPosition: BarPosition.End }) {
      TabContent() {
        Row(){
          GalleryMainPage()
        }

      }
      .tabBar('首页')
      .backgroundColor('#FFFFFF')

      // TabContent() {
      //   Text('发现内容')
      //     .fontSize(16)
      //     .fontColor('#333')
      // }
      // .tabBar('视频检视')
      // .backgroundColor('#FFFFFF')

      TabContent() {
        AudioRecordPage()
      }
      .tabBar('录音设置')
      .backgroundColor('#FFFFFF')

      TabContent() {
        Setting()
      }
      .tabBar('视频设置')
      .backgroundColor('#FFFFFF')
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5')
  }
}
