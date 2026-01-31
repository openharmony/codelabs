/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 *
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
import { AbilityConstant, ConfigurationConstant, UIAbility, Want, abilityAccessCtrl } from '@kit.AbilityKit'
import { hilog } from '@kit.PerformanceAnalysisKit'
import { window } from '@kit.ArkUI'
import {BusinessError} from '@ohos.base'

const DOMAIN: number = 0x0000

export default class EntryAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    try {
      this.context.getApplicationContext().setColorMode(ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET)
    } catch (err) {
      hilog.error(DOMAIN, 'testTag', 'Failed to set colorMode. Cause: %{public}s', JSON.stringify(err))
    }
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onCreate')
  }

  // 封装权限申请方法
  private async requestContactPermissions(): Promise<void> {
    const atManager = abilityAccessCtrl.createAtManager()
    try {
      // 申请读写联系人权限
      const data = await atManager.requestPermissionsFromUser(this.context, [
        'ohos.permission.READ_CONTACTS',
        'ohos.permission.WRITE_CONTACTS'
      ])
      hilog.info(DOMAIN, 'testTag', 'Permission request finished: %{public}s', JSON.stringify(data))
    } catch (err) {
      hilog.error(DOMAIN, 'testTag', 'Permission request failed: %{public}s', JSON.stringify(err))
    }
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageCreate')

    // 在进入页面前，先尝试申请权限
    this.requestContactPermissions().then((): void => {
      windowStage.loadContent('pages/Index', (err: BusinessError<void>): void => {
        if (err.code) {
          hilog.error(DOMAIN, 'testTag', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err))
          return
        }
        hilog.info(DOMAIN, 'testTag', 'Succeeded in loading the content.')
      })
    })
  }

  onWindowStageDestroy(): void {
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageDestroy')
  }

  onForeground(): void {
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onForeground')
  }

  onBackground(): void {
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onBackground')
  }
}
