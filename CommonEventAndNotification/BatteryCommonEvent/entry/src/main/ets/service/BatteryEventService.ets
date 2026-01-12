import commonEventManager from '@ohos.commonEventManager';
import { Logger } from '../common/Logger';
import { COMMON_EVENT_BATTERY_CHANGED } from '../common/Const';
import type { BatteryEventRawParams } from '../model/BatteryEvent';

export type BatteryEventListener = (raw: BatteryEventRawParams) => void;

export class BatteryEventService {
  private subscriber?: commonEventManager.CommonEventSubscriber;
  private listeners: Set<BatteryEventListener> = new Set();
  private subscribed: boolean = false;

  addListener(l: BatteryEventListener): void {
    this.listeners.add(l);
  }

  removeListener(l: BatteryEventListener): void {
    this.listeners.delete(l);
  }

  isSubscribed(): boolean {
    return this.subscribed;
  }

  async subscribe(): Promise<void> {
    if (this.subscribed) {
      return;
    }

    const subscribeInfo: commonEventManager.CommonEventSubscribeInfo = {
      events: [COMMON_EVENT_BATTERY_CHANGED]
    };

    await new Promise<void>((resolve, reject) => {
      commonEventManager.createSubscriber(subscribeInfo, (err, subscriber) => {
        if (err) {
          Logger.e('createSubscriber failed', err);
          reject(err);
          return;
        }
        this.subscriber = subscriber;
        resolve();
      });
    });

    if (!this.subscriber) {
      return;
    }

    commonEventManager.subscribe(this.subscriber, (err, data) => {
      if (err) {
        Logger.w('subscribe callback error', err);
        return;
      }
      const params =
        (data && data.parameters) ? (data.parameters as BatteryEventRawParams)
          : ({} as BatteryEventRawParams);
      this.listeners.forEach(l => {
        try {
          l(params);
        } catch (e) {
          Logger.w('listener error', e);
        }
      });
    });

    this.subscribed = true;
    Logger.i('Subscribed battery common event');
  }

  async unsubscribe(): Promise<void> {
    if (!this.subscribed || !this.subscriber) {
      return;
    }
    await commonEventManager.unsubscribe(this.subscriber);
    this.subscribed = false;
    Logger.i('Unsubscribed battery common event');
  }
}
