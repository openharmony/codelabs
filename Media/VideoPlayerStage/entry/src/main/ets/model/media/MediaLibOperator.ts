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

import mediaLibrary from '@ohos.multimedia.mediaLibrary';
import { MediaOperationAsset, MediaType } from '../media/MediaConstants'

export default class MediaLibOperator {
    private sysMediaLib

    public constructor() {
        this.initOperator();
    }

    private initOperator() {
        this.sysMediaLib = mediaLibrary.getMediaLibrary(globalThis.context);
    }

    private getSaveDir(mediaType: MediaType) {
        switch (mediaType) {
            case MediaType.IMAGE:
                return mediaLibrary.DirectoryType.DIR_IMAGE
            case MediaType.AUDIO:
                return mediaLibrary.DirectoryType.DIR_AUDIO
            case MediaType.VIDEO:
                return mediaLibrary.DirectoryType.DIR_VIDEO
            case MediaType.FILE:
                return mediaLibrary.DirectoryType.DIR_DOCUMENTS
        }
    }

    private getSysMediaType(mediaType: MediaType) {
        switch (mediaType) {
            case MediaType.IMAGE:
                return mediaLibrary.MediaType.IMAGE
            case MediaType.AUDIO:
                return mediaLibrary.MediaType.AUDIO
            case MediaType.VIDEO:
                return mediaLibrary.MediaType.VIDEO
            case MediaType.FILE:
                return mediaLibrary.MediaType.FILE
            default:
                return mediaLibrary.MediaType.IMAGE
        }
    }

    /*
     *打开新建媒资操作通道
     *
     *return fileAsset and fd
     */
    async createMediaFile(creationName, mediaType: MediaType): Promise<MediaOperationAsset> {
        let publicPath = await this.sysMediaLib.getPublicDirectory(this.getSaveDir(mediaType));
        let mediaBuilderAsset = await this.sysMediaLib.createAsset(this.getSysMediaType(mediaType), creationName, publicPath)
        let fd = await this.openMediaFileOperation(mediaBuilderAsset, 'rw')
        if (fd > 0) {
            let operationAsset = new MediaOperationAsset()
            operationAsset.setAsset(mediaBuilderAsset)
            operationAsset.setFd(fd)
            return operationAsset
        }
    }

    /*
     *获取媒资操作符操作权限
     *
     *return fd
     */
    async openMediaFileOperation(mediaFileAsset, operation): Promise<number> {
        if (mediaFileAsset != null) {
            let operationAuth = await mediaFileAsset.open(operation);
            if (operationAuth > 0) {
                return operationAuth
            }
        }
    }

    async closeOMediaFileOperation(mediaFileAsset, fd): Promise<void> {
        if (mediaFileAsset != null) {
            mediaFileAsset.close(fd)
        }
    }

    /*
    * 打开指定媒资操作通道
    *
    *return fileAsset and fd
    */
    async openMediaFileOperationById(fileId): Promise<MediaOperationAsset> {
        let fileKeyObj = mediaLibrary.FileKey;
        let fetchOp = {
            selections: fileKeyObj.ID + '= ?',
            selectionArgs: [fileId.toString()],
            order: fileKeyObj.DATE_ADDED + " DESC",
        }
        let fetchFileResult = await this.sysMediaLib.getFileAssets(fetchOp);
        let fileAsset = await fetchFileResult.getFirstObject();
        let fd = await this.openMediaFileOperation(fileAsset, 'r')
        if (fd > 0) {
            let operationAsset = new MediaOperationAsset()
            operationAsset.setAsset(fileAsset)
            operationAsset.setFd(fd)
            return operationAsset
        }
    }

    async getAsset(mediaType, fileId) {
        let fileKeyObj = mediaLibrary.FileKey;
        let fetchOp = {
            selections: fileKeyObj.ID + '= ?',
            selectionArgs: [fileId.toString()],
        };
        let fetchFileResult = await this.sysMediaLib.getFileAssets(fetchOp);
        let fileAsset = await fetchFileResult.getFirstObject();
        return fileAsset
    }

    /*
     *@param mediaType 非必需，不传时查全部类型
     */
    async getAllAssets(mediaTypes?: MediaType[]) {
        if (mediaTypes == null) {
            mediaTypes = []
        }
        if (mediaTypes.length == 0) {
            mediaTypes.push(MediaType.FILE)
            mediaTypes.push(MediaType.IMAGE)
            mediaTypes.push(MediaType.AUDIO)
            mediaTypes.push(MediaType.VIDEO)
        }
        let selections = ''
        let selectionArgs = []
        let fileKeyObj = mediaLibrary.FileKey;
        for (let i = 0; i < mediaTypes.length; i++) {
            selections += ((i == 0 ? '' : ' or ') + fileKeyObj.MEDIA_TYPE + '= ?')
            selectionArgs.push(this.getSysMediaType(mediaTypes[i]).toString())
        }
        let fetchOp = {
            selections: selections,
            selectionArgs: selectionArgs,
            order: fileKeyObj.DATE_ADDED + " DESC",
        };
        let fetchFileResult = await this.sysMediaLib.getFileAssets(fetchOp);
        let fileAssets = await fetchFileResult.getAllObject();
        return fileAssets
    }
}