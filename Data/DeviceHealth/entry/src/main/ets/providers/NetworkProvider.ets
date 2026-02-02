import connection from '@ohos.net.connection';

export interface NetworkStatus {
  connected: boolean;
  networkType?: string;
}

export type NetworkCallback = (s: NetworkStatus) => void;

export interface INetworkProvider {
  getStatus(): Promise<NetworkStatus>;
  subscribe(cb: NetworkCallback): Promise<void>;
  unsubscribe(): Promise<void>;
}

/** 解决 arkts-no-untyped-obj-literals：用 class，而不是直接 `{}` */
export class NetworkStatusImpl implements NetworkStatus {
  connected: boolean = false;
  networkType?: string = undefined;

  constructor(connected: boolean, networkType?: string) {
    this.connected = connected;
    this.networkType = networkType;
  }
}

type TimerHandle = number;

export class DefaultNetworkProvider implements INetworkProvider {
  private cb: NetworkCallback | null = null;
  private timer: TimerHandle | null = null;

  async getStatus(): Promise<NetworkStatus> {
    return await new Promise<NetworkStatus>((resolve) => {
      connection.getDefaultNet((err: Error | null, _netHandle: connection.NetHandle) => {
        const connected: boolean = err === null;
        resolve(new NetworkStatusImpl(connected));
      });
    }).catch((_e: Error) => {
      return new NetworkStatusImpl(false);
    });
  }

  async subscribe(cb: NetworkCallback): Promise<void> {
    this.cb = cb;

    await this.pushStatus();

    if (this.timer === null) {
      this.timer = setInterval((): void => {
        this.pushStatus().catch((_e: Error): void => {});
      }, 10_000);
    }
  }

  async unsubscribe(): Promise<void> {
    if (this.timer !== null) {
      clearInterval(this.timer);
      this.timer = null;
    }
    this.cb = null;
  }

  private async pushStatus(): Promise<void> {
    if (this.cb === null) return;
    const s: NetworkStatus = await this.getStatus();
    if (this.cb !== null) this.cb(s);
  }
}
