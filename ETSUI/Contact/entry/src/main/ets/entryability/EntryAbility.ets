// import { AbilityConstant, ConfigurationConstant, UIAbility, Want } from '@kit.AbilityKit';
// import { hilog } from '@kit.PerformanceAnalysisKit';
// import { window } from '@kit.ArkUI';
//
// const DOMAIN = 0x0000;
//
// export default class EntryAbility extends UIAbility {
//   onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
//     try {
//       this.context.getApplicationContext().setColorMode(ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET);
//     } catch (err) {
//       hilog.error(DOMAIN, 'testTag', 'Failed to set colorMode. Cause: %{public}s', JSON.stringify(err));
//     }
//     hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onCreate');
//   }
//
//   onDestroy(): void {
//     hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onDestroy');
//   }
//
//   onWindowStageCreate(windowStage: window.WindowStage): void {
//     // Main window is created, set main page for this ability
//     hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageCreate');
//
//     windowStage.loadContent('pages/Index', (err) => {
//       if (err.code) {
//         hilog.error(DOMAIN, 'testTag', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err));
//         return;
//       }
//       hilog.info(DOMAIN, 'testTag', 'Succeeded in loading the content.');
//     });
//   }
//
//   onWindowStageDestroy(): void {
//     // Main window is destroyed, release UI related resources
//     hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageDestroy');
//   }
//
//   onForeground(): void {
//     // Ability has brought to foreground
//     hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onForeground');
//   }
//
//   onBackground(): void {
//     // Ability has back to background
//     hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onBackground');
//   }
// }
import { AbilityConstant, ConfigurationConstant, UIAbility, Want, abilityAccessCtrl } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';

const DOMAIN = 0x0000;

export default class EntryAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    try {
      this.context.getApplicationContext().setColorMode(ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET);
    } catch (err) {
      hilog.error(DOMAIN, 'testTag', 'Failed to set colorMode. Cause: %{public}s', JSON.stringify(err));
    }
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onCreate');
  }

  // 封装权限申请方法
  private async requestContactPermissions(): Promise<void> {
    const atManager = abilityAccessCtrl.createAtManager();
    try {
      // 申请读写联系人权限
      const data = await atManager.requestPermissionsFromUser(this.context, [
        'ohos.permission.READ_CONTACTS',
        'ohos.permission.WRITE_CONTACTS'
      ]);
      hilog.info(DOMAIN, 'testTag', 'Permission request finished: %{public}s', JSON.stringify(data));
    } catch (err) {
      hilog.error(DOMAIN, 'testTag', 'Permission request failed: %{public}s', JSON.stringify(err));
    }
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageCreate');

    // 在进入页面前，先尝试申请权限
    this.requestContactPermissions().then(() => {
      windowStage.loadContent('pages/Index', (err) => {
        if (err.code) {
          hilog.error(DOMAIN, 'testTag', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err));
          return;
        }
        hilog.info(DOMAIN, 'testTag', 'Succeeded in loading the content.');
      });
    });
  }

  onWindowStageDestroy(): void {
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageDestroy');
  }

  onForeground(): void {
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onForeground');
  }

  onBackground(): void {
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onBackground');
  }
}