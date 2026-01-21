/**
 * EntryAbility
 *
 * Application entry point.
 * Responsible for application lifecycle management.
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
