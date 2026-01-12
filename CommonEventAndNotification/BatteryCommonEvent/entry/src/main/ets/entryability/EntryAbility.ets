import { AbilityConstant, UIAbility, Want } from '@kit.AbilityKit';
import { window } from '@kit.ArkUI';

import { Logger } from '../common/Logger';
import { BatteryEventRepository } from '../repository/BatteryEventRepository';
import { BatteryEventService } from '../service/BatteryEventService';
import type { BatteryEventRawParams } from '../model/BatteryEvent';
import type { BatteryEventListener } from '../service/BatteryEventService';

export default class EntryAbility extends UIAbility {
  private readonly svc: BatteryEventService = new BatteryEventService();
  private initStarted: boolean = false;
  private listenerAdded: boolean = false;

  private readonly onBatteryEvent: BatteryEventListener = (raw: BatteryEventRawParams): void => {
    BatteryEventRepository.inst.onBatteryCommonEvent(raw)
      .catch((err: Object) => {
        Logger.w('handle battery event failed', String(err));
      });
  };

  onCreate(_want: Want, _launchParam: AbilityConstant.LaunchParam): void {
    Logger.i('EntryAbility onCreate');
    this.kickoffInit();
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    Logger.i('EntryAbility onWindowStageCreate');
    // Load UI content ASAP to avoid SetUIContent timeout on some standard-system devices.
    windowStage.loadContent('pages/Index');
  }

  onForeground(): void {
    Logger.i('EntryAbility onForeground');
    this.kickoffInit();
    this.trySubscribe();
  }

  onBackground(): void {
    Logger.i('EntryAbility onBackground');
    this.tryUnsubscribeIfNeeded();
  }

  private kickoffInit(): void {
    if (this.initStarted) {
      return;
    }
    this.initStarted = true;

    BatteryEventRepository.inst.ensureInit(this.context)
      .then(() => {
        Logger.i('Repository init ok, history=%{public}d', BatteryEventRepository.inst.getHistory().length);
        this.trySubscribe();
      })
      .catch((err: Object) => {
        Logger.e('Repository init failed', String(err));
      });
  }

  private trySubscribe(): void {
    const settings = BatteryEventRepository.inst.getSettings();
    if (!settings.autoSubscribe) {
      return;
    }

    if (!this.listenerAdded) {
      this.svc.addListener(this.onBatteryEvent);
      this.listenerAdded = true;
    }

    this.svc.subscribe().catch((err: Object) => {
      Logger.w('subscribe failed', String(err));
    });
  }

  private tryUnsubscribeIfNeeded(): void {
    const settings = BatteryEventRepository.inst.getSettings();
    if (settings.keepInBackground) {
      return;
    }
    if (this.listenerAdded) {
      this.svc.removeListener(this.onBatteryEvent);
      this.listenerAdded = false;
    }

    this.svc.unsubscribe().catch((err: Object) => {
      Logger.w('unsubscribe failed', String(err));
    });
  }
}
