import router from "@ohos.router"
import display from '@ohos.display'
import { VideoItem } from '../../model/videomodel';

@Entry
@Component
export struct MeditationVideoPlayPage {

  private controller: VideoController = new VideoController()
  private video!: VideoItem

  private screenWidth: number = 0
  private screenHeight: number = 0

  // scale 值
  @State private scaleValue: number = 1

  aboutToAppear() {
    this.video = router.getParams() as VideoItem

    const displayInfo = display.getDefaultDisplaySync()
    this.screenWidth = displayInfo.width
    this.screenHeight = displayInfo.height

    // 横屏视频：计算 scale
    if (this.video.orientation === 'landscape') {
      // 旋转后：视频容器宽 = screenHeight，高 = screenWidth
      const rotatedWidth = this.screenHeight
      const rotatedHeight = this.screenWidth

      // 为了 Cover：取较大的缩放比
      this.scaleValue = Math.max(
        this.screenWidth / rotatedWidth,
        this.screenHeight / rotatedHeight
      )
    }
  }

  build() {
    Stack() {

      if (this.video.orientation === 'landscape') {

        Stack() {
          Video({
            src: this.video.source,
            controller: this.controller
          })
            .width(this.screenHeight)
            .height(this.screenWidth)
            .rotate({ angle: 90 })
            .scale({ x: this.scaleValue, y: this.scaleValue })
            .controls(true)
            .autoPlay(true)
        }
        .width('100%')
        .height('100%')

      } else {

        Video({
          src: this.video.source,
          controller: this.controller
        })
          .width('100%')
          .height('100%')
          .objectFit(ImageFit.Contain)
          .controls(true)
          .autoPlay(true)
      }

      Button('←')
        .position({ x: 16, y: 40 })
        .fontSize(22)
        .backgroundColor('#00000055')
        .fontColor(Color.White)
        .borderRadius(20)
        .padding(8)
        .onClick(() => router.back())
    }
    .width('100%')
    .height('100%')
    .backgroundColor(Color.Black)
  }
}
