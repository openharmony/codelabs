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

import bundleManager from '@ohos.bundle.bundleManager';
import abilityAccessCtrl from '@ohos.abilityAccessCtrl';
import { CommonConstants } from '../constants/CommonConstants';
import Logger from './Logger';

/**
 * Log tag.
 */
const TAG = '[PermissionUtil]';

export class PermissionUtil {
  /**
   * Applying for Permissions.
   *
   * @returns Permission application result.
   */
  async applyPermission() {
    let atManager = abilityAccessCtrl.createAtManager();
    let data = await atManager.requestPermissionsFromUser(getContext(this), [CommonConstants.PERMISSION]);
    let grantStatus: Array<number> = data.authResults;
    let length: number = grantStatus.length;
    if (length === 0) {
      return 1;
    }
    return grantStatus[0];
  }

  /**
   * Obtaining the permission status.
   *
   * @returns Scan Results.
   */
  async checkPermission() {
    let atManager = abilityAccessCtrl.createAtManager();
    let grantStatus: abilityAccessCtrl.GrantStatus =  abilityAccessCtrl.GrantStatus.PERMISSION_DENIED;
    let tokenId: number = 0;
    try {
      let bundleInfo: bundleManager.BundleInfo =
        await bundleManager.getBundleInfoForSelf(bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION);
      let appInfo: bundleManager.ApplicationInfo = bundleInfo.appInfo;
      tokenId = appInfo.accessTokenId;
    } catch (err) {
      Logger.error(TAG, 'getBundleInfoForSelf failed, code is ${err.code}, message is ${err.message}');
    }
    try {
      grantStatus = await atManager.checkAccessToken(tokenId, CommonConstants.PERMISSION);
    } catch (err) {
      Logger.error(TAG, 'checkAccessToken failed, code is ${err.code}, message is ${err.message}');
    }
    return grantStatus;
  }
}

export default new PermissionUtil();