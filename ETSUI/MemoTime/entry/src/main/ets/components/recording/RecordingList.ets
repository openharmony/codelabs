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
 * 录音列表组件
 * 显示和播放录音附件
 */
import { audioService, PlaybackState, PlaybackCallback } from '../../service/AudioService'
import { Recording } from '../../model/Recording'
import { Logger } from '../../common/utils/Logger'

const TAG = 'RecordingList'

@Component
export struct RecordingList {
  @Prop recordings: Recording[] = []
  @State currentPlayingId: string = ''
  @State playbackState: PlaybackState = PlaybackState.IDLE
  @State currentProgress: number = 0
  @State currentDuration: number = 0
  // 回调
  onDelete: (recording: Recording) => void = () => {
  }

  /**
   * 格式化时长显示 (mm:ss)
   */
  private formatDuration(seconds: number): string {
    const mins = Math.floor(seconds / 60)
    const secs = Math.floor(seconds % 60)
    return `${String(mins).padStart(2, '0')}:${String(secs).padStart(2, '0')}`
  }

  /**
   * 格式化毫秒时长
   */
  private formatMs(ms: number): string {
    return this.formatDuration(ms / 1000)
  }

  /**
   * 播放录音
   */
  private async playRecording(recording: Recording): Promise<void> {
    const callback: PlaybackCallback = {
      onProgressUpdate: (currentTime: number, duration: number) => {
        this.currentProgress = currentTime
        this.currentDuration = duration
      },
      onPlaybackComplete: () => {
        this.currentPlayingId = ''
        this.playbackState = PlaybackState.IDLE
        this.currentProgress = 0
      },
      onError: (error: string) => {
        Logger.error(TAG, error)
        this.currentPlayingId = ''
        this.playbackState = PlaybackState.IDLE
      }
    }

    const success = await audioService.playRecording(recording, callback)
    if (success) {
      this.currentPlayingId = recording.id
      this.playbackState = PlaybackState.PLAYING
    }
  }

  /**
   * 暂停播放
   */
  private async pausePlayback(): Promise<void> {
    const success = await audioService.pausePlayback()
    if (success) {
      this.playbackState = PlaybackState.PAUSED
    }
  }

  /**
   * 恢复播放
   */
  private async resumePlayback(): Promise<void> {
    const success = await audioService.resumePlayback()
    if (success) {
      this.playbackState = PlaybackState.PLAYING
    }
  }

  /**
   * 停止播放
   */
  private async stopPlayback(): Promise<void> {
    await audioService.stopPlayback()
    this.currentPlayingId = ''
    this.playbackState = PlaybackState.IDLE
    this.currentProgress = 0
  }

  /**
   * 切换播放状态
   */
  private async togglePlayback(recording: Recording): Promise<void> {
    if (this.currentPlayingId === recording.id) {
      // 当前录音正在播放
      if (this.playbackState === PlaybackState.PLAYING) {
        await this.pausePlayback()
      } else if (this.playbackState === PlaybackState.PAUSED) {
        await this.resumePlayback()
      }
    } else {
      // 播放新录音
      await this.playRecording(recording)
    }
  }

  /**
   * 获取播放图标
   */
  private getPlayIcon(recording: Recording): Resource {
    if (this.currentPlayingId === recording.id && this.playbackState === PlaybackState.PLAYING) {
      return $r('sys.media.ohos_ic_public_pause')
    }
    return $r('sys.media.ohos_ic_public_play')
  }

  /**
   * 获取进度百分比
   */
  private getProgressPercent(recording: Recording): number {
    if (this.currentPlayingId === recording.id && this.currentDuration > 0) {
      return (this.currentProgress / this.currentDuration) * 100
    }
    return 0
  }

  @Builder
  RecordingItem(recording: Recording) {
    Column() {
      Row() {
        // 播放按钮
        Stack() {
          Circle()
            .width(44)
            .height(44)
            .fill('#4ECDC4')

          Image(this.getPlayIcon(recording))
            .width(20)
            .height(20)
            .fillColor('#FFFFFF')
        }
        .onClick(() => this.togglePlayback(recording))

        // 录音信息
        Column() {
          Text(recording.fileName)
            .fontSize(15)
            .fontColor('#333333')
            .maxLines(1)
            .textOverflow({ overflow: TextOverflow.Ellipsis })

          Row({ space: 12 }) {
            Text(recording.getDurationText())
              .fontSize(13)
              .fontColor('#999999')

            Text(recording.getFileSizeText())
              .fontSize(13)
              .fontColor('#999999')
          }
          .margin({ top: 4 })
        }
        .layoutWeight(1)
        .alignItems(HorizontalAlign.Start)
        .margin({ left: 12 })

        // 删除按钮
        Image($r('sys.media.ohos_ic_public_remove'))
          .width(20)
          .height(20)
          .fillColor('#FF6B6B')
          .onClick(() => this.onDelete(recording))
      }
      .width('100%')

      // 进度条（仅当前播放项显示）
      if (this.currentPlayingId === recording.id) {
        Column() {
          Progress({ value: this.getProgressPercent(recording), total: 100 })
            .width('100%')
            .height(4)
            .color('#4ECDC4')
            .backgroundColor('#E0E0E0')

          Row() {
            Text(this.formatMs(this.currentProgress))
              .fontSize(11)
              .fontColor('#999999')

            Blank()

            Text(this.formatMs(this.currentDuration))
              .fontSize(11)
              .fontColor('#999999')
          }
          .width('100%')
          .margin({ top: 4 })
        }
        .width('100%')
        .margin({ top: 12, left: 56 })
      }
    }
    .width('100%')
    .padding(12)
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
  }

  build() {
    if (this.recordings.length > 0) {
      Column({ space: 8 }) {
        ForEach(this.recordings, (recording: Recording) => {
          this.RecordingItem(recording)
        }, (recording: Recording) => recording.id)
      }
      .width('100%')
    }
  }
}
