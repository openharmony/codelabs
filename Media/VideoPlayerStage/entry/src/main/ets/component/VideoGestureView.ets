/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

import MediaPlayService from '../model/media/MediaPlayService';
import { MediaPlayerState, VideoPlayerGestureType } from '../model/media/MediaConstants';
import DateTimeUtils from '../util/DateTimeUtils'

@Component
export default struct VideoGestureView {
  private service: MediaPlayService
  private startFocusX: number
  private startFocusY: number
  private gestureValue: number
  private gestureType: VideoPlayerGestureType = VideoPlayerGestureType.IDLE
  @State controlHint: string = ''
  @State controlTypeSrc: Resource = undefined
  @State isShow: boolean = false
  private onStateUpdate: (isShow: boolean) => void

  aboutToAppear() {
    if (this.service != null) {
      this.service.addStatusChangedListener((state, extra) => {
        switch (state) {
          case MediaPlayerState.STOP:
          case MediaPlayerState.IDLE:
          case MediaPlayerState.ERROR:
          case MediaPlayerState.FINISH:
            this.setViewShow(false)
            break;
        }
      })
    }
  }

  private setViewShow(isShow: boolean) {
    if (isShow != this.isShow) {
      this.isShow = isShow
      this.onStateUpdate(isShow)
    }
  }

  private getFocusPoint(event: TouchEvent) {
    let sumX = 0;
    let sumY = 0;
    let count = event.touches.length;
    for (let i = 0;i < event.touches.length; i++) {
      sumX += event.touches[i].x
      sumY += event.touches[i].y
    }
    return { x: sumX / count, y: sumY / count };
  }

  onFingerMove(focusX, focusY) {
    let changedValue
    let offsetX = focusX - this.startFocusX
    let offsetY = focusY - this.startFocusY
    switch (this.gestureType) {
      case VideoPlayerGestureType.IDLE:
        if (Math.abs(offsetX) == Math.abs(offsetY)) {
          break
        } else if (Math.abs(offsetX) > Math.abs(offsetY)) {
          this.gestureType = VideoPlayerGestureType.PROGRESS_CONTROL
        } else if (this.startFocusX > px2vp(globalThis.display.width) / 2) {
          this.gestureType = VideoPlayerGestureType.VOLUME_CONTROL
        } else {
          this.gestureType = VideoPlayerGestureType.BRIGHT_CONTROL
        }
        this.setViewShow(true)
        break;
      case VideoPlayerGestureType.PROGRESS_CONTROL:
        changedValue = this.service.getCurrentTime() + offsetX / px2vp(globalThis.display.width) * this.service.getDuration()
        this.gestureValue = (changedValue <= 0 ? 0 : (changedValue >= this.service.getDuration() ? this.service.getDuration() : changedValue))
        this.controlHint = DateTimeUtils.ms2CountdownTime(Math.round(this.gestureValue / 1000) * 1000) + '/' + DateTimeUtils.ms2CountdownTime(this.service.getDuration())
        this.controlTypeSrc = (offsetX >= 0 ? $r('app.media.ic_forward') : $r('app.media.ic_backward'))
        break;
      case VideoPlayerGestureType.VOLUME_CONTROL:
        changedValue = this.service.getVolume() - offsetY / px2vp(globalThis.display.height)
        this.gestureValue = (changedValue <= 0 ? 0 : (changedValue >= 1 ? 1 : changedValue))
        this.controlHint = Math.round(this.gestureValue * 100) + '%'
        this.controlTypeSrc = $r('app.media.ic_horns')
        break;
      case VideoPlayerGestureType.BRIGHT_CONTROL:
        changedValue = this.service.getBrightness() - offsetY / px2vp(globalThis.display.height)
        this.gestureValue = (changedValue <= 0 ? 0 : (changedValue >= 1 ? 1 : changedValue))
        this.controlHint = Math.round(this.gestureValue * 100) + '%'
        this.controlTypeSrc = $r('app.media.ic_bright')
        break;
    }
  }

  onFingerUp() {
    switch (this.gestureType) {
      case VideoPlayerGestureType.PROGRESS_CONTROL:
        this.service.seek(this.gestureValue)
        break;
      case VideoPlayerGestureType.VOLUME_CONTROL:
        this.service.setVolume(this.gestureValue)
        break;
      case VideoPlayerGestureType.BRIGHT_CONTROL:
        this.service.setBrightness(this.gestureValue)
        break;
    }
    this.gestureType = VideoPlayerGestureType.IDLE
    this.setViewShow(false)
  }

  build() {
    Stack({ alignContent: Alignment.Center }) {
      if (this.isShow) {
        Column() {
          Image(this.controlTypeSrc)
            .width(40)
            .height(40)
            .objectFit(ImageFit.Fill)
          Text(this.controlHint).margin({ top: 15 }).fontColor('#ffffff')
        }
        .width(120)
        .height(120)
        .padding({ left: 10, right: 10, top: 20, bottom: 20 })
        .justifyContent(FlexAlign.Center)
        .alignItems(HorizontalAlign.Center)
        .backgroundColor('#88000000')
        .borderRadius(10)
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#00000000')
    .onTouch((event) => {
      if (this.service != null
      && (this.service.getPlayerState() == MediaPlayerState.PLAY
      || this.service.getPlayerState() == MediaPlayerState.PAUSE)
      ) {
        let focusPoint = this.getFocusPoint(event)
        if (event.type === TouchType.Down) {
          this.startFocusX = focusPoint.x
          this.startFocusY = focusPoint.y
        } else if (event.type === TouchType.Move) {
          this.onFingerMove(focusPoint.x, focusPoint.y)
        } else if (event.type === TouchType.Up) {
          this.onFingerUp()
        }
      }
    })
    .gesture(
    TapGesture({ count: 2 })
      .onAction(() => {
        if (this.service != null) {
          if (this.service.getPlayerState() == MediaPlayerState.PLAY) {
            this.service.pause()
          } else {
            this.service.resume()
          }
        }
      })
    )
  }
}

