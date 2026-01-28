/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import { MandalaLayer } from './MandalaLayer'
import router from '@ohos.router'
import { AUDIO_DATA } from '../../model/AudioItem'

@Entry
@Component
export struct MeditationPage {

  @State duration: number = 10 * 60
  @State isRunning: boolean = false

  // 1. 【新增】引入全局字体缩放变量
  @StorageProp('appFontScale') fontScale: number = 1.0;

  private timerId?: number

  // 🎵 音频播放器（用 Video）
  private audioController: VideoController = new VideoController()

  private audio = AUDIO_DATA[1]

  aboutToAppear() {
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
        .opacity(0)          // 👈 完全不可见
        .controls(false)     // 👈 不要任何框
        .autoPlay(false)

      // ===== UI =====
      Column({ space: 36 }) {

        Row() {
          MandalaLayer({ duration: this.duration })
        }
        .width('100%')
        .justifyContent(FlexAlign.Center)
        .padding({ left: 45 })

        Text('放松你的思绪')
          .fontSize(36 * this.fontScale) // 2. 标题变大
          .fontWeight(FontWeight.Medium)

        Text(this.formatTime())
          .fontSize(48 * this.fontScale) // 3. 倒计时变大
          .fontWeight(FontWeight.Bold)

        Button(this.isRunning ? '停止沉思' : '开始沉思')
          .fontSize(30 * this.fontScale) // 4. 按钮文字变大
          .height(60 * this.fontScale)   // 5. 按钮尺寸适配
          .width(220 * this.fontScale)
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