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

import display from '@ohos.display';
import { MediaAsset } from '../model/media/MediaConstants';
import router from '@system.router';
import VideoView from '../component/VideoView'
import VideoController from '../component/VideoController'
import MediaPlayService from '../model/media/MediaPlayService'

@Entry
@Component
struct video_player {
  private videoService: MediaPlayService= new MediaPlayService()
  private curVideoAsset
  @State isAuth: boolean = false

  aboutToAppear() {
    globalThis.requestPermissions().then((isAuth) => {
      this.isAuth = isAuth
    })
    display.getDefaultDisplay((err, data) => {
      if (!err.code) {
        globalThis.display = data;
      }
    });
  }

  aboutToDisappear() {
    this.videoService.release()
  }

  onPageShow() {
    if (router.getParams() != null) {
      let fileId = router.getParams().fileId
      this.loadPlayerAsset('', Number(fileId), true)
    }
  }

  onPageHide() {
    this.videoService.pause()
  }

  private loadPlayerAsset(title: string, url: string | number, isAutoPlay: boolean) {
    let playAsset = new MediaAsset()
    playAsset.setTitle(title)
    playAsset.setSource(url)
    this.videoService.loadAsset(playAsset, isAutoPlay);
  }

  build() {
    Stack() {
      if (this.isAuth) {
        VideoView({
          service: this.videoService,
          isContentFull: false,
          onCreated: (id) => {
            this.loadPlayerAsset('video_test_1.mp4', '../../resources/rawfile/video_test_1.mp4', false)
          }
        })
        VideoController({ service: this.videoService })
      }
    }
    .height('100%').width('100%')
  }
}