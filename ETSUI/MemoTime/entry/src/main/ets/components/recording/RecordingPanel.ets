/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/**
 * 录音面板组件
 * 用于录制语音笔记
 */
import { audioService, RecordingState, RecordingCallback } from '../../service/AudioService'
import { Recording } from '../../model/Recording'
import { Logger } from '../../common/utils/Logger'

const TAG = 'RecordingPanel'

@Component
export struct RecordingPanel {
  @State recordingState: RecordingState = RecordingState.IDLE
  @State recordingDuration: number = 0
  @State showPanel: boolean = false

  // 回调
  onRecordingComplete: (recording: Recording) => void = () => {}
  onCancel: () => void = () => {}

  /**
   * 格式化时长显示
   */
  private formatDuration(seconds: number): string {
    const mins = Math.floor(seconds / 60)
    const secs = Math.floor(seconds % 60)
    const ms = Math.floor((seconds % 1) * 10)
    return `${String(mins).padStart(2, '0')}:${String(secs).padStart(2, '0')}.${ms}`
  }

  /**
   * 开始录音
   */
  private async startRecording(): Promise<void> {
    const callback: RecordingCallback = {
      onDurationUpdate: (duration: number) => {
        this.recordingDuration = duration
      },
      onRecordingComplete: (recording: Recording) => {
        this.onRecordingComplete(recording)
        this.resetState()
      },
      onError: (error: string) => {
        Logger.error(TAG, error)
        this.resetState()
      }
    }

    const success = await audioService.startRecording(callback)
    if (success) {
      this.recordingState = RecordingState.RECORDING
      this.showPanel = true
    }
  }

  /**
   * 暂停/恢复录音
   */
  private async togglePause(): Promise<void> {
    if (this.recordingState === RecordingState.RECORDING) {
      const success = await audioService.pauseRecording()
      if (success) {
        this.recordingState = RecordingState.PAUSED
      }
    } else if (this.recordingState === RecordingState.PAUSED) {
      const success = await audioService.resumeRecording()
      if (success) {
        this.recordingState = RecordingState.RECORDING
      }
    }
  }

  /**
   * 停止并保存录音
   */
  private async stopRecording(): Promise<void> {
    const recording = await audioService.stopRecording()
    if (recording) {
      this.onRecordingComplete(recording)
    }
    this.resetState()
  }

  /**
   * 取消录音
   */
  private async cancelRecording(): Promise<void> {
    await audioService.cancelRecording()
    this.resetState()
    this.onCancel()
  }

  /**
   * 重置状态
   */
  private resetState(): void {
    this.recordingState = RecordingState.IDLE
    this.recordingDuration = 0
    this.showPanel = false
  }

  @Builder
  IdleView() {
    // 开始录音按钮
    Row() {
      Image($r('sys.media.ohos_ic_public_voice'))
        .width(20)
        .height(20)
        .fillColor('#4ECDC4')

      Text('添加语音笔记')
        .fontSize(15)
        .fontColor('#4ECDC4')
        .margin({ left: 8 })
    }
    .padding({ left: 16, right: 16, top: 12, bottom: 12 })
    .backgroundColor('#E8F8F5')
    .borderRadius(24)
    .onClick(() => this.startRecording())
  }

  @Builder
  RecordingView() {
    Column() {
      // 录音时长
      Text(this.formatDuration(this.recordingDuration))
        .fontSize(32)
        .fontWeight(FontWeight.Bold)
        .fontColor('#FF6B6B')
        .margin({ bottom: 16 })

      // 录音波形动画（简化版 - 使用动态圆点）
      Row({ space: 4 }) {
        ForEach([1, 2, 3, 4, 5], (item: number) => {
          Circle()
            .width(8)
            .height(this.recordingState === RecordingState.RECORDING ? 8 + item * 4 : 8)
            .fill('#FF6B6B')
            .animation({
              duration: 300,
              iterations: -1,
              curve: Curve.EaseInOut
            })
        }, (item: number) => item.toString())
      }
      .height(40)
      .margin({ bottom: 24 })

      // 控制按钮
      Row({ space: 32 }) {
        // 取消按钮
        Column() {
          Image($r('sys.media.ohos_ic_public_cancel'))
            .width(24)
            .height(24)
            .fillColor('#999999')

          Text('取消')
            .fontSize(12)
            .fontColor('#999999')
            .margin({ top: 4 })
        }
        .onClick(() => this.cancelRecording())

        // 暂停/恢复按钮
        Column() {
          Circle()
            .width(64)
            .height(64)
            .fill(this.recordingState === RecordingState.RECORDING ? '#FF6B6B' : '#4ECDC4')

          Stack() {
            if (this.recordingState === RecordingState.RECORDING) {
              // 暂停图标
              Row({ space: 6 }) {
                Rect().width(6).height(20).fill('#FFFFFF')
                Rect().width(6).height(20).fill('#FFFFFF')
              }
            } else {
              // 播放图标（继续录音）
              Image($r('sys.media.ohos_ic_public_play'))
                .width(24)
                .height(24)
                .fillColor('#FFFFFF')
            }
          }
          .width(64)
          .height(64)
          .position({ x: 0, y: 0 })

          Text(this.recordingState === RecordingState.RECORDING ? '暂停' : '继续')
            .fontSize(12)
            .fontColor('#666666')
            .margin({ top: 4 })
        }
        .onClick(() => this.togglePause())

        // 完成按钮
        Column() {
          Image($r('sys.media.ohos_ic_public_ok'))
            .width(24)
            .height(24)
            .fillColor('#4ECDC4')

          Text('完成')
            .fontSize(12)
            .fontColor('#4ECDC4')
            .margin({ top: 4 })
        }
        .onClick(() => this.stopRecording())
      }
    }
    .width('100%')
    .padding(24)
    .backgroundColor('#FFFFFF')
    .borderRadius(16)
  }

  build() {
    Column() {
      if (this.showPanel) {
        this.RecordingView()
      } else {
        this.IdleView()
      }
    }
    .width('100%')
  }
}
