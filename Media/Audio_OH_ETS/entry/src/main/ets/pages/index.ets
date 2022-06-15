/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
 */
 
import prompt from '@system.prompt';
import SysPermissionUtils from '../util/SysPermissionUtils'
import DateTimeUtils from '../util/DateTimeUtils'
import { AudioPlayer, AudioPlayerState } from '../model/media/AudioPlay'
import { AudioRecorder, RecordState } from '../model/media/AudioRecord'
import { MediaLibOperator, MediaType } from '../model/media/MediaLibOperator'

@Entry
@Component
struct Index {
  @State isCaptureBtnPress: boolean = false
  @State captureBtnSrc: Resource = $r('app.media.ic_record')
  @State audioRecordList: Array<any> = new Array()
  private curRecordInfo
  private player: AudioPlayer
  private recorder: AudioRecorder
  private mediaLibOperator: MediaLibOperator = MediaLibOperator.getInstance()

  aboutToAppear() {
    SysPermissionUtils.check("ohos.permission.MICROPHONE").then((isNeedPermission) => {
      if (isNeedPermission) {
        let permissions =
          ["ohos.permission.MICROPHONE",
          "ohos.permission.READ_MEDIA",
          "ohos.permission.WRITE_MEDIA",
          "ohos.permission.WRITE_USER_STORAGE",
          "ohos.permission.READ_USER_STORAGE",
          "ohos.permission.MEDIA_LOCATION"
          ]
        SysPermissionUtils.request(permissions, (isAuth) => {
          this.init()
        })
      } else {
        this.init()
      }
    })
  }

  aboutToDisappear() {
    this.player.release()
    this.recorder.release()
  }

  init() {
    this.initListData()
    this.initRecorder()
    this.initPlayer()
  }

  private initListData() {
    this.mediaLibOperator.getAllAssets(MediaType.AUDIO).then((assets) => {
      this.audioRecordList = assets
    })
  }

  private initRecorder() {
    this.recorder = new AudioRecorder()
    this.recorder.setOnStatusChangedListener((state, extra) => {
      switch (state) {
        case RecordState.START:
          this.curRecordInfo = extra
          break
        case RecordState.STOP:
          this.audioRecordList.unshift(this.curRecordInfo)
          break
      }
    })
  }

  private initPlayer() {
    this.player = new AudioPlayer()
    this.player.setOnStatusChangedListener((state, extra) => {
      switch (state) {
        case AudioPlayerState.FINISH:
          this.mediaLibOperator.closeOperationChannel()
          break
      }
    })
  }

  @Builder CaptureButton() {
    Image(this.captureBtnSrc)
      .objectFit(ImageFit.Fill)
      .margin({ top: 40 })
      .alignSelf(ItemAlign.Center)
      .width('120px')
      .height('120px')
      .scale({ x: this.isCaptureBtnPress ? 0.8 : 1.0, y: this.isCaptureBtnPress ? 0.8 : 1.0 })
      .opacity(this.isCaptureBtnPress ? 0.6 : 1.0)
      .animation({
        duration: this.isCaptureBtnPress ? 300 : 0,
        curve: Curve.Linear,
      })
      .onTouch((event: TouchEvent) => {
        if (event.type === TouchType.Down) {
          this.isCaptureBtnPress = true
        } else if (event.type === TouchType.Up) {
          this.isCaptureBtnPress = false
          if (this.recorder.isRecording()) {
            this.captureBtnSrc = $r('app.media.ic_record')
            this.recorder.stop()
          } else {
            this.captureBtnSrc = $r('app.media.ic_record_stop')
            this.recorder.start()
          }
        }
      })
  }

  @Styles pressedStyles() {
    .backgroundColor(Color.Grey)
  }

  @Styles normalStyles() {
    .backgroundColor(Color.White)
  }

  @Builder RecordItem(item) {
    Column() {
      Text('录音名称：' + item.displayName)
        .width('100%')
        .fontSize(24)
      Text('录音时间：' + DateTimeUtils.dateFormat(item.dateAdded))
        .width('100%')
        .fontSize(24)
      Text('录音目录：' + item.uri)
        .width('100%')
        .fontSize(24)
    }
    .stateStyles({
      normal: this.normalStyles,
      pressed: this.pressedStyles
    })
    .width('100%')
    .height('200')
    .borderRadius(20)
    .justifyContent(FlexAlign.SpaceBetween)
    .padding({ top: 20, bottom: 20, left: 50, right: 50 })
    .onClick(() => {
      if (this.recorder.isRecording()) {
        prompt.showToast({
          message: 'Please stop record first!',
          duration: 3000,
          bottom: 100
        })
        return;
      }
      this.player.stop()
      this.mediaLibOperator.openAssetOperationChannel(item.id).then((operationAsset) => {
        console.info('cwq getAssetFd success,fd is ' + operationAsset.operationId.toString())
        this.player.play("fd://" + operationAsset.operationId.toString(), 0, true);
      })
    })
  }

  build() {
    Column() {
      List({ space: 20, initialIndex: 0 }) {
        ForEach(this.audioRecordList.map((item, i) => {
          return { index: i, data: item }
        }), item => {
          ListItem() {
            this.RecordItem(item.data)
          }
        }, item => item.index)
      }
      .listDirection(Axis.Vertical)
      .divider({ strokeWidth: 2, color: 0xFFFFFF, startMargin: 20, endMargin: 20 }) // 每行之间的分界线
      .edgeEffect(EdgeEffect.Spring)
      .width('90%')
      .height('80%')

      this.CaptureButton()
    }.width('100%').height('100%').backgroundColor(0xDCDCDC).padding({ top: 20 })
  }
}