/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

iimport
UIAbility
from
'@ohos.app.ability.UIAbility'
iimport Want from '@ohos.app.ability.Want'
import AbilityConstant from '@ohos.app.ability.AbilityConstant'
import hilog from '@ohos.hilog'
import window from '@ohos.window'
export default class EntryAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam) {
    hilog.info(0x0000, 'EntryAbility', '%{public}s', 'Ability onCreate')
  }

  onDestroy() {
    hilog.info(0x0000, 'EntryAbility', '%{public}s', 'Ability onDestroy')
  }

  onWindowStageCreate(windowStage: window.WindowStage) {
    windowStage.loadContent('pages/LoginPage', (err, data) => {
      if (err && err.code) {
        hilog.error(
          0x0000,
          'EntryAbility',
          'Failed to load content: %{public}s',
          JSON.stringify(err) ?? ''
        )
        return
      }
      hilog.info(
        0x0000,
        'EntryAbility',
        'Content loaded: %{public}s',
        JSON.stringify(data) ?? ''
      )
    })
  }

  onWindowStageDestroy() {
    hilog.info(0x0000, 'EntryAbility', '%{public}s', 'WindowStage destroyed')
  }

  onForeground() {
    hilog.info(0x0000, 'EntryAbility', '%{public}s', 'Ability onForeground')
  }

  onBackground() {
    hilog.info(0x0000, 'EntryAbility', '%{public}s', 'Ability onBackground')
  }
}
