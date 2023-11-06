/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
import resourceManager from '@ohos.resourceManager';

import Logger from '../utils/Logger';
import { GlobalContext } from './GlobalContext';

const TAG = '[ResourceManagerUtil]';

/**
 * Resource management tool class.
 */
export class ResourceManagerUtil {
  /**
   * Obtains the description of a media file.
   *
   * @param path Resource path.
   * @param callback Callback function.
   */
  public static getSrcPath(path: string, callback: Function) {
    if (callback === undefined) {
      Logger.error(TAG, 'getSrcPath fail,callback is empty.');
      return;
    }
    try {
      let resourceManager = GlobalContext.getContext()
        .getObject('resourceManager') as resourceManager.ResourceManager;
      resourceManager.getRawFd(path, (error, value) => {
        if (error != null) {
          Logger.error(TAG, `callback getRawFd failed error code: ${error.code}, message: ${error.message}.`);
        } else {
          Logger.info(TAG, `callback getRawFd success fd code: ${value.fd}.`);
          callback(value);
        }
      });
    } catch (error) {
      Logger.error(TAG, `callback getRawFd failed, error code: ${error.code}, message: ${error.message}.`);
    }
  }
}