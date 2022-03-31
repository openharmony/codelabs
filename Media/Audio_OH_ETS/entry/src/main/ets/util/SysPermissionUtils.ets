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
import bundle from '@ohos.bundle'
import abilityAccessCtrl from '@ohos.abilityAccessCtrl'

export default class SysPermissionUtils {
  static async check(permission) {
    let bundleFlag = 0
    let userId = 100
    let appInfo = await bundle.getApplicationInfo('com.huawei.cookbook', bundleFlag, userId)
    let tokenId = appInfo.accessTokenId
    console.info(`cwq grantPermission,tokenId=${tokenId}`)
    let atManager = abilityAccessCtrl.createAtManager()
    let result = await atManager.verifyAccessToken(tokenId, permission)
    console.info(`cwq grantPermission,verifyPermission,result=${result}`)
    return result == abilityAccessCtrl.GrantStatus.PERMISSION_DENIED
  }

  static request(targetPermissionArray, callback) {
    if (targetPermissionArray instanceof Array && targetPermissionArray.length > 0) {
      console.info('cwq SysPermissionUtils requestPermissionsFromUser start. length: ' + targetPermissionArray.length);
      globalThis.context.requestPermissionsFromUser(targetPermissionArray, (result) => {
        console.info('cwq SysPermissionUtils requestPermissionsFromUser successful. result: ' + JSON.stringify(result));
        callback(result.code === 0)
      })
    }
  }
}