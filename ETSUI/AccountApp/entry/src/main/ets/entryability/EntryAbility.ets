import { AbilityConstant, ConfigurationConstant, UIAbility, Want } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';
// 1. 引入工具类
import PreferenceUtil from '../utils/PreferenceUtil';

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

  onDestroy(): void {
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onDestroy');
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageCreate');

    // =========================================================
    // 👇👇👇 修改后：只保留自动登录逻辑，移除应用锁 👇👇👇
    // =========================================================

    // 2. 初始化工具类
    PreferenceUtil.init(this.context).then(async () => {

      // 3. 只检查是否有自动登录信息 (移除了 isLockEnabled)
      const loginInfo = await PreferenceUtil.getLoginInfo();

      // 4. 决定启动哪个页面
      let targetPage = 'pages/LoginPage'; // 默认去登录页

      if (loginInfo) {
        // 如果有自动登录信息 -> 直接去主页
        targetPage = 'pages/Index';
      }

      // 5. 加载最终决定的页面
      windowStage.loadContent(targetPage, (err) => {
        if (err.code) {
          hilog.error(DOMAIN, 'testTag', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err));
          return;
        }
        hilog.info(DOMAIN, 'testTag', 'Succeeded in loading the content.');

        // --- 隐私模式逻辑 (保留) ---
        try {
          let windowClass = windowStage.getMainWindowSync();
          windowClass.setWindowPrivacyMode(true);
          hilog.info(DOMAIN, 'testTag', 'Privacy mode enabled.');
        } catch (exception) {
          hilog.error(DOMAIN, 'testTag', 'Failed to set privacy mode: ' + JSON.stringify(exception));
        }
        // ------------------------------------
      });
    });
    // =========================================================
    // 👆👆👆 修改结束 👆👆👆
    // =========================================================
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