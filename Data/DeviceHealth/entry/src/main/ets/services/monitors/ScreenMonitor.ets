import type { IScreenProvider, ScreenEvent } from '../../providers/ScreenProvider';
import { DefaultScreenProvider } from '../../providers/ScreenProvider';
import { EventLevel, EventLog, EventType, nowMs } from '../../model/EventLog';
import type { Emit } from './BatteryMonitor';

/** 显式 payload 接口，避免 any / untyped object literal */
export interface ScreenPayload {
  event: ScreenEvent;
}

/** payload 实现类 */
export class ScreenPayloadImpl implements ScreenPayload {
  event: ScreenEvent;

  constructor(event: ScreenEvent) {
    this.event = event;
  }
}

export class ScreenMonitor {
  private provider: IScreenProvider;
  private emit: Emit | null = null;

  constructor(provider?: IScreenProvider) {
    this.provider = provider ?? new DefaultScreenProvider();
  }

  async start(emit: Emit): Promise<void> {
    this.emit = emit;
    await this.provider.subscribe((e: ScreenEvent): void => {
      this.onEvent(e);
    });
  }

  async stop(): Promise<void> {
    await this.provider.unsubscribe();
    this.emit = null;
  }

  private onEvent(e: ScreenEvent): void {
    if (this.emit === null) {
      return;
    }

    if (e === 'LOCKED') {
      this.emitEvent(EventLevel.INFO, '屏幕已锁定', e);
    } else if (e === 'UNLOCKED') {
      this.emitEvent(EventLevel.INFO, '屏幕已解锁', e);
    } else if (e === 'SCREEN_ON') {
      this.emitEvent(EventLevel.INFO, '屏幕点亮', e);
    } else if (e === 'SCREEN_OFF') {
      this.emitEvent(EventLevel.INFO, '屏幕熄灭', e);
    }
  }

  private emitEvent(level: EventLevel, message: string, event: ScreenEvent): void {
    if (this.emit === null) {
      return;
    }

    const payload: ScreenPayload = new ScreenPayloadImpl(event);

    const log: EventLog = {
      type: EventType.SCREEN,
      level,
      timestamp: nowMs(),
      message,
      payload,
      source: 'ScreenMonitor',
    };

    this.emit(log);
  }
}
