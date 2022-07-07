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
import { MediaPlayerState } from '../model/media/MediaConstants'
import prompt from '@system.prompt';

@Component
export default struct VideoView {
  private service: MediaPlayService
  private playerAreaController = new XComponentController()
  private isContentFull: boolean = false
  @State ratio: number = 0
  private onCreated: (id) => void

  aboutToAppear() {
    if (this.service != null) {
      this.service.addStatusChangedListener((state, extra) => {
        switch (state) {
          case MediaPlayerState.SIZE_CHANGED:
            if (!this.isContentFull) {
              this.ratio = extra.width / extra.height
              this.playerAreaController.setXComponentSurfaceSize({
                surfaceWidth: extra.width,
                surfaceHeight: extra.height
              })
            }
            break;
          case MediaPlayerState.ERROR:
            if (extra.code == -331350011 || extra.code == -331350014 || extra.code == -331350007) {
              prompt.showDialog({ message: '播放错误，请检查播放源，如果是在线视频，请检查网络是否连接。错误码：' + extra.code })
            }
            break;
        }
      })
    }
  }

  build() {
    Stack() {
      XComponent({
        id: '',
        type: 'surface',
        libraryname: '',
        controller: this.playerAreaController
      })
        .onLoad(() => {
          let renderId = this.playerAreaController.getXComponentSurfaceId()
          if (this.service != null) {
            this.service.addSurface(renderId)
          }
          this.onCreated(renderId)
        })
        .aspectRatio(this.ratio)
    }.backgroundColor('#ffffff')
    .width('100%')
    .height('100%')
  }
}