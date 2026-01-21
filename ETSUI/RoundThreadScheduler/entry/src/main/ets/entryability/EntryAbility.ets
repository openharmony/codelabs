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


import AbilityStage from '@ohos.app.ability.AbilityStage'
import UIAbility from '@ohos.app.ability.UIAbility'
import Window from '@ohos.window'

export default class EntryAbility extends UIAbility {

  onCreate(want: any, launchParam: any): void {
    console.info('[EntryAbility] onCreate')
  }

  onDestroy(): void {
    console.info('[EntryAbility] onDestroy')
  }

  onWindowStageCreate(windowStage: Window.WindowStage): void {
    console.info('[EntryAbility] onWindowStageCreate')

    windowStage.loadContent(
      'pages/Index',
      (err, data) => {
        if (err.code) {
          console.error(
            `[EntryAbility] Failed to load content. Code: ${err.code}`
          )
          return
        }
        console.info('[EntryAbility] Content loaded successfully')
      }
    )
  }

  onWindowStageDestroy(): void {
    console.info('[EntryAbility] onWindowStageDestroy')
  }

  onForeground(): void {
    console.info('[EntryAbility] onForeground')
  }

  onBackground(): void {
    console.info('[EntryAbility] onBackground')
  }
}
