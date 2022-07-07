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

import multimedia_image from '@ohos.multimedia.image';
import { MediaType } from '../model/media/MediaConstants';
import MediaLibOperator from '../model/media/MediaLibOperator';
import PressureEffectButton from '../component/PressureEffectButton'
import DateTimeUtils from '../util/DateTimeUtils'

export { MediaLibView, MediaInfo, MediaLibController }

@Component
struct MediaLibView {
  @State mediaList: Array<MediaInfo> = new Array()
  private controller: MediaLibController
  private mediaTypes: MediaType[]
  private mediaLibOperator: MediaLibOperator = new MediaLibOperator()
  private onItemClick: (item) => void

  aboutToAppear() {
    //系统multimedia_image服务空调用，避免multimedia_image被系统优化掉
    multimedia_image.createPixelMap(
      new ArrayBuffer(4096),
      { size: { height: 1, width: 2 } },
      (err, pixelMap) => {
        if (pixelMap != null) {
          this.initListData()
        }
      })
  }

  private initListData() {
    this.mediaLibOperator.getAllAssets(this.mediaTypes).then(async (assets) => {
      for (let i = 0;i < assets.length; i++) {
        let thumbnail
        switch (assets[i].mediaType) {
          case MediaType.FILE:
            thumbnail = $r('app.media.ic_file')
            break
          case MediaType.IMAGE:
          case MediaType.VIDEO:
            thumbnail = await assets[i].getThumbnail()
            break
          case MediaType.AUDIO:
            thumbnail = $r('app.media.ic_note')
            break
        }
        let mediaInfo: MediaInfo = new MediaInfo(assets[i].id, assets[i].mediaType, assets[i].duration, thumbnail)
        this.mediaList.push(mediaInfo)
      }
      if (this.controller != null) {
        this.controller.setList(this.mediaList)
      }
    })
  }

  build() {
    Grid() {
      ForEach(
        this.mediaList,
        (item) => {
          GridItem() {
            Stack({ alignContent: Alignment.BottomEnd }) {
              PressureEffectButton({
                commonSrc: item.thumbnail,
                width: '100%',
                height: '200',
                bgColor: '#000000',
                bgRadius: 5,
                effectWidth: (item.mType == MediaType.IMAGE || item.mType == MediaType.VIDEO) ? '100%' : '60',
                effectHeight: (item.mType == MediaType.IMAGE || item.mType == MediaType.VIDEO) ? '150' : '75',
              })
              if (item.mType == MediaType.AUDIO || item.mType == MediaType.VIDEO) {
                Text(DateTimeUtils.ms2CountdownTime(item.duration)).fontColor('#ffffff').fontSize(18)
              }
            }
          }.onClick(() => {
            this.onItemClick(item)
          })
        },
        item => item.id)
    }
    .width('100%')
    .height('100%')
    .columnsTemplate('1fr 1fr 1fr 1fr')
    .columnsGap(10)
    .rowsGap(10)
    .padding(20)
  }
}

class MediaInfo {
  public id: number
  public mType: number
  public duration: number
  public thumbnail: string | PixelMap | Resource

  constructor(id, mType, duration, thumbnail) {
    this.id = id
    this.mType = mType
    this.duration = duration
    this.thumbnail = thumbnail
  }
}

class MediaLibController {
  public mediaList: Array<MediaInfo>

  public setList(mediaList: Array<MediaInfo>) {
    this.mediaList = mediaList
  }

  public addItem(item: MediaInfo) {
    if (this.mediaList != null) {
      this.mediaList.unshift(item)
    }
  }
}