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

import Ability from '@ohos.application.Ability'
import abilityAccessCtrl from '@ohos.abilityAccessCtrl'
import bundle from '@ohos.bundle'
import data_preferences from '@ohos.data.preferences'

export default class MainAbility extends Ability {

    onCreate(want, launchParam) {
        console.log("[Demo] MainAbility onCreate")
        globalThis.abilityWant = want;
    }

    onDestroy() {
        console.log("[Demo] MainAbility onDestroy")
    }

    onWindowStageCreate(windowStage) {
        // Main window is created, set main page for this ability
        console.log("[Demo] MainAbility onWindowStageCreate")

        windowStage.setUIContent(this.context, "pages/index", null)
    }

    onWindowStageDestroy() {
        // Main window is destroyed, release UI related resources
        console.log("[Demo] MainAbility onWindowStageDestroy")
    }

    onForeground() {
        // Ability has brought to foreground
        console.log("[Demo] MainAbility onForeground")

        // 检查是否有权限
        globalThis.checkPermission = (async() => {
            console.info('AccessPermission[MainAbility] grantPermission')
            // 需要校验的权限
            var permissionNameUser = "ohos.permission.DISTRIBUTED_DATASYNC";
            var bundleFlag = 0;
            // 要校验的目标应用的身份标识
            var tokenID = undefined;
            // userID值，默认100
            var userID = 100;
            // 获取应用信息
            var appInfo = await bundle.getApplicationInfo('com.example.helloworld0218', bundleFlag, userID)
            tokenID = appInfo.accessTokenId;
            console.log("AccessPermission[MainAbility] accessTokenId:" + appInfo.accessTokenId)
            // 获取访问控制模块对象
            var atManager = abilityAccessCtrl.createAtManager();
            // 进行权限校验，0有权限，-1无权限
            var result = await atManager.verifyAccessToken(tokenID, permissionNameUser)
            console.log("AccessPermission[MainAbility] result:" + result)
            return result
        })

        // 已授权权限标识
        globalThis.permissionGranted = abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED

        // 申请权限
        globalThis.requestPermission = (() => {
            console.log("AccessPermission[MainAbility] request permissions from user")
            this.context.requestPermissionsFromUser(['ohos.permission.DISTRIBUTED_DATASYNC'],(result) => {
                // 判断是否授权，用于界面展示
                globalThis.judgePermission()
                // 设置已进行授权操作
                globalThis.putData(true)
                console.log('requestPermissionsFromUserResult:' + JSON.stringify(result));
            });
        })

        // 首选项，保存数据：key: requestPermissionFlag，value：requestPermissionFlag(boolean值)
        globalThis.putData = ((requestPermissionFlag: boolean) => {
            let promise = data_preferences.getPreferences(this.context, 'mystore')
            promise.then((preferences) => {
                let promisePut = preferences.put('requestPermissionFlag', requestPermissionFlag)
                promisePut.then(() => {
                    console.info("Put the value of startup successfully.")
                }).catch((err) => {
                    console.info("Put the value of startup failed, err: " + err)
                })
            }).catch((err) => {
                console.info("Get the preferences failed, err: " + err)
            })
        })

        // 首选项，获取数据
        globalThis.getData = (() => {
            let promise = data_preferences.getPreferences(this.context, 'mystore')
            return promise
        })
    }

    onBackground() {
        // Ability has back to background
        console.log("[Demo] MainAbility onBackground")
    }
};
