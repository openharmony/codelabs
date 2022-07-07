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

import fileIO from '@ohos.fileio';
import MediaLibOperator from './MediaLibOperator';
import { MediaAsset, MediaOperationAsset, MediaSourceType } from './MediaConstants';

/*
 *音视频播放服务类
 */
export default class MediaAssetBuilder {
    private mediaLibOperator: MediaLibOperator
    private mediaLibAsset: MediaOperationAsset
    private mediaAsset: MediaAsset
    private realUrl
    private sourceType: MediaSourceType = MediaSourceType.DEFAULT
    private fileDescription: number

    constructor() {
        this.mediaLibOperator = new MediaLibOperator()
    }

    public async build(asset: MediaAsset) {
        this.mediaAsset = asset
        let originSource = this.mediaAsset.getSource()
        if (!isNaN(originSource)) {
            //Src为数字=fileId
            this.mediaLibAsset = await this.mediaLibOperator.openMediaFileOperationById(originSource)
            this.fileDescription = this.mediaLibAsset.getFd()
            this.sourceType = MediaSourceType.MEDIA_LIB_FILE
            this.mediaAsset.setTitle(this.mediaLibAsset.getAsset().displayName)
            this.realUrl = "fd://" + this.mediaLibAsset.getFd()
        } else if (originSource.startsWith('data/') || originSource.startsWith('/data')) {
            let fd = fileIO.openSync(originSource)
            this.fileDescription = fd
            this.sourceType = MediaSourceType.ABSOLUTE_PATH_FILE
            this.realUrl = "fd://" + fd
        } else if (originSource.indexOf('../resources/rawfile/') >= 0) {
            let rawfileFd = await globalThis.context.resourceManager.getRawFileDescriptor(originSource)
            this.fileDescription = rawfileFd.fd
            this.sourceType = MediaSourceType.RAWFILE_FILE
            this.realUrl = "fd://" + rawfileFd.fd
        } else {
            this.realUrl = originSource
        }
        return this.mediaAsset
    }

    public getRealUrl() {
        return this.realUrl
    }

    public async release() {
        switch (this.sourceType) {
            case MediaSourceType.MEDIA_LIB_FILE:
                if (this.mediaLibAsset != null) {
                    await this.mediaLibOperator.closeOMediaFileOperation(this.mediaLibAsset.getAsset(), this.fileDescription)
                    this.mediaLibAsset = null
                }
                break
            case MediaSourceType.ABSOLUTE_PATH_FILE:
                fileIO.closeSync(this.fileDescription)
                break
            case MediaSourceType.RAWFILE_FILE:
                globalThis.context.resourceManager.closeRawFileDescriptor(this.mediaAsset.getSource())
                break
        }
        this.mediaAsset = null
        this.sourceType = MediaSourceType.DEFAULT
        this.fileDescription = -1
    }
}

