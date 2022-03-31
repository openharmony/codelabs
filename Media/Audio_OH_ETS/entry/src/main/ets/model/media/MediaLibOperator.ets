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
import mediaLibrary from '@ohos.multimedia.mediaLibrary';
import DataTypeUtils from '../../util/DataTypeUtils'

let MediaType = {
  FILE: mediaLibrary.MediaType.FILE,
  IMAGE: mediaLibrary.MediaType.IMAGE,
  AUDIO: mediaLibrary.MediaType.AUDIO,
  VIDEO: mediaLibrary.MediaType.VIDEO,
}

export { MediaType }

export class MediaLibOperator {
  private static instance: MediaLibOperator
  private sysMediaLib
  private operationAsset = {
    operationId: -1,
    asset: null
  }
  private constructor() {
    this.initOperator();
  }

  public static getInstance(): MediaLibOperator{
    if (DataTypeUtils.isNull(this.instance)) {
      this.instance = new MediaLibOperator()
    }
    return this.instance
  }

  private initOperator() {
    this.sysMediaLib = mediaLibrary.getMediaLibrary(globalThis.context);
  }

  private getSaveDir(mediaType) {
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

  isAssetOperating(): boolean{
    return this.operationAsset.operationId > 0
  }

  /*
   *打开新建媒资操作通道
   */
  async openCreationOperationChannel(creationName, mediaType) {
    await this.closeOperationChannel()
    let publicPath = await this.sysMediaLib.getPublicDirectory(this.getSaveDir(mediaType));
    let mediaBuilderAsset = await this.sysMediaLib.createAsset(mediaType, creationName, publicPath)
    await this.getOperationAuth(mediaBuilderAsset, 'rw')
    return this.operationAsset
  }

  /*
   *获取媒资操作权限
   */
  async getOperationAuth(fileAsset, operation) {
    if (fileAsset != undefined) {
      let operationAuth = await fileAsset.open(operation);
      if (operationAuth > 0) {
        this.operationAsset.asset = fileAsset
        this.operationAsset.operationId = operationAuth
        return operationAuth
      }
    }
    return -1
  }

  async closeOperationChannel() {
    if (this.isAssetOperating()) {
      this.operationAsset.asset.close(this.operationAsset.operationId).then(() => {
        this.operationAsset.asset = undefined
        this.operationAsset.operationId = -1
      });
    }
  }

  /*
  * 打开指定媒资操作通道
  */
  async openAssetOperationChannel(fileId) {
    await this.closeOperationChannel()
    let fileKeyObj = mediaLibrary.FileKey;
    let fetchOp = {
      selections: fileKeyObj.ID + '= ?',
      selectionArgs: [fileId.toString()],
      order: fileKeyObj.DATE_ADDED + " DESC",
    };
    let fetchFileResult = await this.sysMediaLib.getFileAssets(fetchOp);
    let fileAsset = await fetchFileResult.getFirstObject();
    await this.getOperationAuth(fileAsset, 'r')
    return this.operationAsset
  }

  async getAllAssets(mediaType) {
    let fileKeyObj = mediaLibrary.FileKey;
    let fetchOp = {
      selections: fileKeyObj.MEDIA_TYPE + '= ?',
      selectionArgs: [mediaType.toString()],
      order: fileKeyObj.DATE_ADDED + " DESC",
    };
    let fetchFileResult = await this.sysMediaLib.getFileAssets(fetchOp);
    let fileAssets = await fetchFileResult.getAllObject();
    return fileAssets
  }
}