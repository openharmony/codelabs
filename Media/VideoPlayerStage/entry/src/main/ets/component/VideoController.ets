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

import MediaPlayService from '../model/media/MediaPlayService'
import VideoGestureView from '../component/VideoGestureView'
import LoadingView from './LoadingView';
import { MediaPlayerState, MediaAsset } from '../model/media/MediaConstants';
import DateTimeUtil from '../util/DateTimeUtils';
import router from '@system.router';

@Component
export default struct VideoController {
  @State isMenuShow: boolean = true
  @State isBuffering: boolean = false
  @State title: string = ''
  @State playBtnSrc: Resource = $r('app.media.ic_play')
  private service: MediaPlayService
  @State curDuration: number = 0
  @State totalDuration: number = 0
  private menuShowTasker: number = -1
  private durationTasker: number = -1
  private dialogController: CustomDialogController = new CustomDialogController({
    builder: SearchResourceDialog({
      folderChosen: () => {
        router.push({ uri: 'pages/video_lib' })
      },
      confirm: (url) => {
        let playAsset = new MediaAsset()
        playAsset.setTitle('在线测试视频')
        playAsset.setSource(url)
        this.service.loadAsset(playAsset, true);
      }
    }),
    autoCancel: true,
    alignment: DialogAlignment.Center
  })

  aboutToAppear() {
    if (this.service != null) {
      this.service.addStatusChangedListener((state, extra) => {
        switch (state) {
          case MediaPlayerState.START:
            break;
          case MediaPlayerState.LOAD:
            this.title = extra.asset.getTitle()
            this.totalDuration = 0
            break;
          case MediaPlayerState.PREPARED:
            this.totalDuration = extra.duration
            break
          case MediaPlayerState.BUFFERING_START:
            this.isBuffering = true
            break;
          case MediaPlayerState.PLAY:
            this.showMenu(5000)
            this.playBtnSrc = $r('app.media.ic_pause')
          case MediaPlayerState.BUFFERING_END:
            this.isBuffering = false
            this.startDurationTasker()
            break;
          case MediaPlayerState.IDLE:
          case MediaPlayerState.ERROR:
            this.stopDurationTasker()
          case MediaPlayerState.FINISH:
            this.curDuration = 0
          case MediaPlayerState.STOP:
          case MediaPlayerState.PAUSE:
            this.showMenu()
            this.playBtnSrc = $r('app.media.ic_play')
            break;

        }
      })
    }
  }

  private startDurationTasker() {
    if (this.service != null && this.durationTasker == -1) {
      this.durationTasker = setInterval(() => {
        if (this.service.getPlayerState() == MediaPlayerState.PLAY) {
          this.curDuration = this.service.getCurrentTime()
        }
      }, 1000)
    }
  }

  private stopDurationTasker() {
    if (this.durationTasker) {
      clearInterval(this.durationTasker)
      this.durationTasker = -1
    }
  }

  private showMenu(duration?: number) {
    clearTimeout(this.menuShowTasker)
    this.isMenuShow = true
    if (duration) {
      this.hideMenu(duration)
    }
  }

  private hideMenu(delayTime?: number) {
    clearTimeout(this.menuShowTasker)
    this.menuShowTasker = setTimeout(() => {
      this.menuShowTasker = -1
      this.isMenuShow = false
    }, delayTime ? delayTime : 0)
  }

  @Builder TopMenu() {
    Stack({ alignContent: Alignment.End }) {
      Text(this.title)
        .fontSize('58px')
        .fontColor('#ffffff')
        .width('100%')
        .height('100%')
        .textAlign(TextAlign.Center)
      Button({ type: ButtonType.Normal, stateEffect: true }) {
        Image($r('app.media.ic_search')).width(30).height(30).objectFit(ImageFit.Fill)
      }
      .width(90)
      .height(90)
      .margin({ right: 20 })
      .backgroundColor('#00000000')
      .onClick(() => {
        this.showMenu(5000)
        this.dialogController.open()
      })
    }
    .position({ x: 0, y: 0 })
    .linearGradient({
      direction: GradientDirection.Bottom,
      colors: [['#CC000000', 0.0], ['#88000000', 0.4], ['#44000000', 0.8], ['#00000000', 1.0]]
    })
    .width('100%')
    .height(this.isMenuShow ? '15%' : '0%')
    .opacity(this.isMenuShow ? 1.0 : 0)
    .animation({
      duration: 300,
      curve: Curve.Smooth,
    })
  }

  @Builder CenterMenu() {
    Stack() {
      if (this.isBuffering) {
        LoadingView()
      } else if (this.isMenuShow) {
        Button({ type: ButtonType.Circle, stateEffect: true }) {
          Image(this.playBtnSrc).width(30).height(30).objectFit(ImageFit.Fill)
        }
        .width('100%')
        .height('100%')
        .backgroundColor('#88000000')
        .onClick(() => {
          this.showMenu()
          if (this.service != null) {
            if (this.service.getPlayerState() == MediaPlayerState.PLAY) {
              this.service.pause()
            } else if (this.service.getPlayerState() == MediaPlayerState.PAUSE) {
              this.service.resume()
            } else {
              this.service.play()
            }
          }
        })
      }
    }.width(this.isBuffering || this.isMenuShow ? 60 : 0)
    .height(this.isBuffering || this.isMenuShow ? 60 : 0)
    .position({ x: '50%', y: '50%' })
    .markAnchor({ x: '50%', y: '50%' })
  }

  @Builder BottomMenu() {
    Row() {
      Text(DateTimeUtil.ms2CountdownTime(this.curDuration))
        .fontSize(25)
        .fontColor(Color.White)
      Slider({ value: this.curDuration / this.service.getDuration() * 100, max: 100, min: 0 })
        .selectedColor(Color.White)
        .layoutWeight(1)
        .trackColor('#5a5a5a')
        .trackThickness(5)
        .selectedColor('#FF6103')
        .onChange((value: number, mode: SliderChangeMode) => {
          switch (mode) {
            case SliderChangeMode.Begin:
              this.showMenu()
              this.stopDurationTasker()
              break;
            case SliderChangeMode.Moving:
              this.curDuration = Math.round(value / 100 * this.service.getDuration() / 1000) * 1000
              break;
            case 3:
              this.curDuration = Math.round(value / 100 * this.service.getDuration() / 1000) * 1000
            case SliderChangeMode.End:
              this.showMenu(5000)
              if (this.service != null) {
                this.service.seek(value / 100 * this.service.getDuration())
              }
              break;
          }
        })
      Text(DateTimeUtil.ms2CountdownTime(this.totalDuration))
        .fontSize(25)
        .fontColor(Color.White)
    }
    .linearGradient({
      direction: GradientDirection.Top,
      colors: [['#CC000000', 0.0], ['#88000000', 0.4], ['#44000000', 0.8], ['#00000000', 1.0]]
    })
    .padding({ left: 20, right: 20 })
    .justifyContent(FlexAlign.Center)
    .width('100%')
    .height(this.isMenuShow ? '15%' : '0%')
    .opacity(this.isMenuShow ? 1.0 : 0)
    .animation({
      duration: 300,
      curve: Curve.Smooth,
    })
  }

  build() {
    Stack({ alignContent: Alignment.Bottom }) {
      VideoGestureView({
        service: this.service,
        onStateUpdate: (isGestureViewShow) => {
            isGestureViewShow ? this.hideMenu() : this.showMenu(5000)
        }
      })
      this.TopMenu()
      this.CenterMenu()
      this.BottomMenu()
    }.height('100%').width('100%').backgroundColor('#00000000')
    .onClick(() => {
      if (this.service != null) {
          this.isMenuShow ? this.hideMenu() : this.showMenu(5000)
      }
    })
  }
}

@CustomDialog
struct SearchResourceDialog {
  controller: CustomDialogController
  confirm: (url) => void
  folderChosen: () => void
  private inputUrl: string = 'https://ss0.bdstatic.com/-0U0bnSm1A5BphGlnYG/cae-legoup-video-target/93be3d88-9fc2-4fbd-bd14-833bca731ca7.mp4'

  build() {
    Column() {
      Row() {
        TextInput({ placeholder: this.inputUrl })
          .inputFilter("/(http:|https:|data)\/\/([\w.]+\/?)\S*/", (value) => {
            this.inputUrl = value
          }).layoutWeight(1)
        Button({ type: ButtonType.Normal, stateEffect: true }) {
          Image($r('app.media.ic_folder'))
            .width(30)
            .height(30)
            .objectFit(ImageFit.Fill)
        }.backgroundColor('#00000000').margin({ left: 10 }).onClick(() => {
          this.controller.close()
          this.folderChosen()
        })
      }.margin({ bottom: 30 })

      Button('confirm')
        .onClick(() => {
          this.controller.close()
          this.confirm(this.inputUrl)
        })
    }.padding({ left: 20, right: 20, top: 40, bottom: 30 })
  }
}

