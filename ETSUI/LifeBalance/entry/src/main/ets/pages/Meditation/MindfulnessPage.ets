import { DotMandalaLayer } from './DotMandalaLayer'
import router from '@ohos.router'
import { AUDIO_DATA } from '../../model/AudioItem'

@Entry
@Component
export struct MindfulnessPage {

  @State duration: number = 10 * 60
  @State isRunning: boolean = false

  // 1. 【新增】引入全局字体缩放
  @StorageProp('appFontScale') fontScale: number = 1.0;

  private timerId?: number

  // 🎵 音频播放器（仍然用 Video）
  private audioController: VideoController = new VideoController()

  // 当前背景音（先固定一个）
  private audio = AUDIO_DATA[0]

  aboutToAppear() {
    // 从路由参数获取 duration（分钟）
    const params = router.getParams() as Record<string, number>
    if (params?.duration !== undefined) {
      this.duration = params.duration * 60
    } else {
      this.duration = 5 * 60
    }
  }

  build() {
    Stack() {

      // ===== 🎵 隐形音频播放器 =====
      Video({
        src: this.audio.source,
        controller: this.audioController
      })
        .width(1)
        .height(1)
        .opacity(0)
        .controls(false)
        .autoPlay(false)

      // ===== UI =====
      Column({ space: 36 }) {

        Row() {
          DotMandalaLayer()
        }
        .width('100%')
        .justifyContent(FlexAlign.Center)

        Text('放松你的思绪')
          .fontSize(36 * this.fontScale) // 2. 标题变大
          .fontWeight(FontWeight.Medium)

        Text(this.formatTime())
          .fontSize(48 * this.fontScale) // 3. 倒计时数字变大
          .fontWeight(FontWeight.Bold)

        Button(this.isRunning ? '停止正念' : '开始正念')
          .fontSize(30 * this.fontScale) // 4. 按钮文字变大
          .height(60 * this.fontScale)   // 5. 按钮高度也随之变大
          .width(220 * this.fontScale)   // 6. 按钮宽度也随之变大
          .onClick(() => {
            this.toggleMeditation()
          })
      }
      .width('100%')
      .height('100%')
      .justifyContent(FlexAlign.Start)
      .alignItems(HorizontalAlign.Center)
      .padding({ top: 180 })
    }
  }

  toggleMeditation() {
    if (this.isRunning) {
      this.stopMeditation()
    } else {
      this.startMeditation()
    }
    this.isRunning = !this.isRunning
  }

  startMeditation() {
    // ▶️ 播放音频
    this.audioController.start()

    // ⏱️ 开始计时
    if (this.timerId !== undefined) {
      return
    }

    this.timerId = setInterval(() => {
      if (this.duration > 0) {
        this.duration--
      } else {
        this.stopMeditation()
        this.isRunning = false
      }
    }, 1000)
  }

  stopMeditation() {
    // ⏸️ 暂停音频
    this.audioController.pause()

    // ⏸️ 停止计时
    if (this.timerId !== undefined) {
      clearInterval(this.timerId)
      this.timerId = undefined
    }
  }

  formatTime(): string {
    let m = Math.floor(this.duration / 60)
    let s = this.duration % 60
    return `${m}:${s < 10 ? '0' + s : s}`
  }
}