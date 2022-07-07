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

import bundle from '@ohos.bundle'
import abilityAccessCtrl from '@ohos.abilityAccessCtrl'
import LogUtils from './LogUtils';

export default class SysPermissionUtils {
    static async hasAuth(permission) {
        let bundleFlag = 0
        let userId = 100
        let appInfo = await bundle.getApplicationInfo(globalThis.bundleName, bundleFlag, userId)
        let tokenId = appInfo.accessTokenId
        let atManager = abilityAccessCtrl.createAtManager()
        let result = await atManager.verifyAccessToken(tokenId, permission)
        return result == abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED
    }

    static async request(targetPermissionArray) {
        let needPermissionArray: Array<string> = new Array()
        for (let targetPermission of targetPermissionArray) {
            let isAuth = await this.hasAuth(targetPermission)
            if (!isAuth) {
                needPermissionArray.push(targetPermission)
            }
        }
        if (needPermissionArray.length > 0) {
            globalThis.context.requestPermissionsFromUser(needPermissionArray)
        }
    }

    static async requestWithinNecessary(targetPermissionArray: string[], necessaryPermissionArray?: string[]) {
        let isNecessaryPermissionAuth: boolean = true
        let needPermissionArray: Array<string> = new Array()
        if (targetPermissionArray != null) {
            for (let targetPermission of targetPermissionArray) {
                let isTargetAuth = await this.hasAuth(targetPermission)
                if (!isTargetAuth) {
                    needPermissionArray.push(targetPermission)
                }
            }
        }
        if (necessaryPermissionArray != null) {
            for (let necessaryPermission of necessaryPermissionArray) {
                let isNecessaryAuth = await this.hasAuth(necessaryPermission)
                if (!isNecessaryAuth) {
                    needPermissionArray.push(necessaryPermission)
                    isNecessaryPermissionAuth = false
                }
            }
        }
        if (needPermissionArray.length > 0) {
            let result = await globalThis.context.requestPermissionsFromUser(needPermissionArray)
            LogUtils.info('SysPermissionUtils', 'request is called,result is ' + JSON.stringify(result))
            if (!isNecessaryPermissionAuth) {
                isNecessaryPermissionAuth = true
                out: for (let i = 0; i < needPermissionArray.length; i++) {
                    for (let j = 0; j < necessaryPermissionArray.length; j++) {
                        if (needPermissionArray[i] == necessaryPermissionArray[j] && (result.authResults[i] == abilityAccessCtrl.GrantStatus.PERMISSION_DENIED)) {
                            isNecessaryPermissionAuth = false
                            break out
                        }
                    }
                }
            }
        }
        return isNecessaryPermissionAuth
    }
}