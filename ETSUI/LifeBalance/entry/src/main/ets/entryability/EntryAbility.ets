import { AbilityConstant, ConfigurationConstant, UIAbility, Want } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';
import { UserStore } from '../model/information';
import { RdbManager } from '../common/database/RdbManager';
const DOMAIN = 0x0000;

export default class EntryAbility extends UIAbility {
  async onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): Promise<void> {
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onCreate');

    try {
      await RdbManager.getInstance().init(this.context);
      hilog.info(DOMAIN, 'testTag', 'Database initialized successfully');
    } catch (err) {
      hilog.error(DOMAIN, 'testTag', 'Database init failed: %{public}s', JSON.stringify(err));
    }

    PersistentStorage.persistProp('appFontScale', 1.0);
    PersistentStorage.persistProp('user_gender', 1);
    PersistentStorage.persistProp('user_height', 170);
    PersistentStorage.persistProp('user_weight', 65);
    PersistentStorage.persistProp('user_age', 25);
    PersistentStorage.persistProp('targetCalories', 2000);
    PersistentStorage.persistProp('targetWater', 2000);
  }


  onDestroy(): void {
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onDestroy');
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageCreate');

    UserStore.init();

    windowStage.loadContent('pages/Index', (err) => {
      if (err.code) {
        hilog.error(DOMAIN, 'testTag', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err));
        return;
      }
      hilog.info(DOMAIN, 'testTag', 'Succeeded in loading the content.');
    });
  }

  onWindowStageDestroy(): void {
    // Main window is destroyed, release UI related resources
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageDestroy');
  }

  onForeground(): void {
    // Ability has brought to foreground
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onForeground');
  }

  onBackground(): void {
    // Ability has back to background
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onBackground');
  }
}
