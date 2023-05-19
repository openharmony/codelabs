/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import fs from '@ohos.file.fs';
import image from '@ohos.multimedia.image';
import Logger from './LoggerUtil';
import { CommonConstants } from '../common/constant/CommonConstants';

const TAG: string = 'imageEdit_Decode';

/**
 * Async get resource fd.
 *
 * @return file fd.
 */
async function getResourceFd(filename: string) {
  const resourceMgr = getContext(this).resourceManager;
  const context = getContext(this);
  if (filename === CommonConstants.RAW_FILE_NAME) {
    let imageBuffer = await resourceMgr.getMediaContent($r("app.media.ic_low"))
    let filePath = context.cacheDir + '/' + filename;
    let file = fs.openSync(filePath, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE);
    let writeLen = fs.writeSync(file.fd, imageBuffer.buffer);
    fs.copyFileSync(filePath, context.cacheDir + '/' + CommonConstants.RAW_FILE_NAME_TEST);
    return file.fd;
  } else {
    let filePath = context.cacheDir + '/' + filename;
    let file = fs.openSync(filePath, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE);
    return file.fd;
  }
}

/**
 * Async create pixel map.
 *
 * @return pixelMa.
 */
export async function getPixelMap(fileName: string) {
  const fd = await getResourceFd(fileName);
  const imageSourceApi = image.createImageSource(fd);
  if (!imageSourceApi) {
    Logger.error(TAG, 'imageSourceAPI created failed!');
    return;
  }
  const pixelMap = await imageSourceApi.createPixelMap({
    editable: true
  });
  return pixelMap;
}