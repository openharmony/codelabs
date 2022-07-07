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

import  SysPermissionUtils from '../util/SysPermissionUtils';
import Ability from '@ohos.application.Ability'

export default class MainAbility extends Ability {
    onCreate(want, launchParam) {
        let abilityInfo = this.context.abilityInfo;
        globalThis.bundleName = abilityInfo.bundleName
        globalThis.abilityWant = want;
        globalThis.context = this.context
        globalThis.LogTag = 'cwq'
        globalThis.isShowLog = true
        globalThis.display
    }


    onWindowStageCreate(windowStage) {
        console.log("[Demo] MainAbility onWindowStageCreate")
        globalThis.requestPermissions = (() => {
            return SysPermissionUtils.requestWithinNecessary(null,
                ["ohos.permission.READ_MEDIA",
                "ohos.permission.WRITE_MEDIA",
                "ohos.permission.MEDIA_LOCATION"
                ]
            )
        })
        windowStage.setUIContent(this.context, "pages/video_player", null)
    }

    onWindowStageDestroy() {
        // Main window is destroyed, release UI related resources
        console.log("[Demo] MainAbility onWindowStageDestroy")
    }

    onForeground() {
        // Ability has brought to foreground
        console.log("[Demo] MainAbility onForeground")
    }

    onBackground() {
        // Ability has back to background
        console.log("[Demo] MainAbility onBackground")
    }
};
