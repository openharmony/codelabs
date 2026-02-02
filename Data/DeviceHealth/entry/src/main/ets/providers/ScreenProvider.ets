import commonEventManager from '@ohos.commonEventManager';

export type ScreenEvent = 'LOCKED' | 'UNLOCKED' | 'SCREEN_ON' | 'SCREEN_OFF';
export type ScreenCallback = (e: ScreenEvent) => void;

export interface IScreenProvider {
  subscribe(cb: ScreenCallback): Promise<void>;
  unsubscribe(): Promise<void>;
}

class ScreenSubscriberInfo {
  events: string[];

  constructor() {
    this.events = [
      'usual.event.SCREEN_ON',
      'usual.event.SCREEN_OFF',
      'usual.event.SCREEN_LOCKED',
      'usual.event.SCREEN_UNLOCKED',
    ];
  }
}

type Subscriber = commonEventManager.CommonEventSubscriber;
type CommonEventData = commonEventManager.CommonEventData;

export class DefaultScreenProvider implements IScreenProvider {
  private subscriber: Subscriber | null = null;
  private cb: ScreenCallback | null = null;

  async subscribe(cb: ScreenCallback): Promise<void> {
    this.cb = cb;

    try {
      const info: ScreenSubscriberInfo = new ScreenSubscriberInfo();

      await new Promise<void>((resolve, reject) => {
        commonEventManager.createSubscriber(
          info,
          (err: Error | null, sub: Subscriber) => {
            if (err) {
              reject(err);
              return;
            }

            this.subscriber = sub;

            commonEventManager.subscribe(
              sub,
              (err2: Error | null, data: CommonEventData) => {
                if (err2) {
                  return;
                }

                // CommonEventData 在 ArkTS 中只有 event 字段
                const eventName: string = data.event ?? '';

                if (eventName.indexOf('SCREEN_LOCKED') >= 0) {
                  if (this.cb !== null) this.cb('LOCKED');
                } else if (eventName.indexOf('SCREEN_UNLOCKED') >= 0) {
                  if (this.cb !== null) this.cb('UNLOCKED');
                } else if (eventName.indexOf('SCREEN_ON') >= 0) {
                  if (this.cb !== null) this.cb('SCREEN_ON');
                } else if (eventName.indexOf('SCREEN_OFF') >= 0) {
                  if (this.cb !== null) this.cb('SCREEN_OFF');
                }
              }
            );

            resolve();
          }
        );
      });
    } catch (_e) {
      // 降级：不抛异常
    }
  }

  async unsubscribe(): Promise<void> {
    try {
      if (this.subscriber !== null) {
        commonEventManager.unsubscribe(this.subscriber, (_err: Error | null) => {});
      }
    } catch (_e) {}

    this.subscriber = null;
    this.cb = null;
  }
}
