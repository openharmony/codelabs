import { AbilityConstant, ConfigurationConstant, UIAbility, Want } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';
import { router } from '@kit.ArkUI';

const DOMAIN = 0x0000;

/**
 * 路由参数接口
 */
interface ScheduleParams {
  scheduleId: string;
}

export default class EntryAbility extends UIAbility {
  private scheduleIdFromNotification: string | null = null;
  private windowStage: window.WindowStage | null = null;

  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    try {
      this.context.getApplicationContext().setColorMode(ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET);
    } catch (err) {
      hilog.error(DOMAIN, 'testTag', 'Failed to set colorMode. Cause: %{public}s', JSON.stringify(err));
    }
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onCreate');

    // 处理从通知点击传递的参数
    this.handleWantParams(want);
  }

  /**
   * 处理 Want 参数，提取 scheduleId
   */
  private handleWantParams(want: Want): void {
    if (want.parameters && want.parameters['scheduleId']) {
      this.scheduleIdFromNotification = want.parameters['scheduleId'] as string;
      hilog.info(DOMAIN, 'testTag', 'Received scheduleId from notification: %{public}s', this.scheduleIdFromNotification);
    }
  }

  /**
   * 应用已启动时，从通知再次打开应用
   */
  onNewWant(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onNewWant');

    // 处理从通知点击传递的参数
    if (want.parameters && want.parameters['scheduleId']) {
      const scheduleId = want.parameters['scheduleId'] as string;
      hilog.info(DOMAIN, 'testTag', 'onNewWant - Received scheduleId: %{public}s', scheduleId);

      // 应用已在前台，直接跳转到日程详情页
      this.navigateToScheduleDetail(scheduleId);
    }
  }

  /**
   * 跳转到日程详情页
   */
  private navigateToScheduleDetail(scheduleId: string): void {
    try {
      const params: ScheduleParams = { scheduleId: scheduleId };
      router.pushUrl({
        url: 'pages/ScheduleDetailPage',
        params: params
      });
      hilog.info(DOMAIN, 'testTag', 'Navigated to schedule detail: %{public}s', scheduleId);
    } catch (err) {
      hilog.error(DOMAIN, 'testTag', 'Failed to navigate to schedule detail: %{public}s', JSON.stringify(err));
    }
  }

  onDestroy(): void {
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onDestroy');
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    // Main window is created, set main page for this ability
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageCreate');
    this.windowStage = windowStage;

    // 根据是否有 scheduleId 决定加载哪个页面
    let targetPage = 'pages/Index';
    let params: ScheduleParams | undefined = undefined;

    if (this.scheduleIdFromNotification) {
      targetPage = 'pages/ScheduleDetailPage';
      params = { scheduleId: this.scheduleIdFromNotification };
      hilog.info(DOMAIN, 'testTag', 'Loading schedule detail page for: %{public}s', this.scheduleIdFromNotification);
      // 清除已使用的参数
      this.scheduleIdFromNotification = null;
    }

    windowStage.loadContent(targetPage, (err) => {
      if (err.code) {
        hilog.error(DOMAIN, 'testTag', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err));
        return;
      }
      hilog.info(DOMAIN, 'testTag', 'Succeeded in loading the content: %{public}s', targetPage);
      
      // 如果有参数，使用 router 跳转以传递参数
      if (params) {
        setTimeout(() => {
          router.replaceUrl({
            url: targetPage,
            params: params
          });
        }, 100);
      }
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